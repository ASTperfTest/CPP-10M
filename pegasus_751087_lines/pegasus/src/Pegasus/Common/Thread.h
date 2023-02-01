//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Thread_h
#define Pegasus_Thread_h

#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/AtomicInt.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/List.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/Semaphore.h>
#include <Pegasus/Common/TSDKey.h>
#include <Pegasus/Common/Threads.h>

#if defined(PEGASUS_HAVE_PTHREADS)
# include <signal.h>
#endif

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE cleanup_handler : public Linkable
{
public:
    cleanup_handler(void (*routine) (void *), void *arg):_routine(routine),
        _arg(arg)
    {
    }
    ~cleanup_handler()
    {;
    }

private:

    void execute()
    {
        _routine(_arg);
    }

    cleanup_handler();

    void (*_routine)(void*);
    void *_arg;

    friend class Thread;
};

///////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE thread_data : public Linkable
{
public:

    static void default_delete(void *data);

    thread_data(const char *key) : _delete_func(NULL), _data(NULL), _size(0)
    {
        PEGASUS_ASSERT(key != NULL);
        size_t keysize = strlen(key);
        _key.reset(new char[keysize + 1]);
        memcpy(_key.get(), key, keysize);
        _key.get()[keysize] = 0x00;
    }

    thread_data(const char *key, size_t size) :
        _delete_func(default_delete), _size(size)
    {
        PEGASUS_ASSERT(key != NULL);
        size_t keysize = strlen(key);
        _key.reset(new char[keysize + 1]);
        memcpy(_key.get(), key, keysize);
        _key.get()[keysize] = 0x00;
        _data =::operator  new(_size);
    }

    thread_data(const char *key, size_t size, void *data)
        : _delete_func(default_delete), _size(size)
    {
        PEGASUS_ASSERT(key != NULL);
        PEGASUS_ASSERT(data != NULL);
        size_t keysize = strlen(key);

        _key.reset(new char[keysize + 1]);
        memcpy(_key.get(), key, keysize);
        _key.get()[keysize] = 0x00;
        _data =::operator  new(_size);
        memcpy(_data, data, size);
    }

    ~thread_data()
    {
        if (_data != NULL)
            if (_delete_func != NULL)
            {
                _delete_func(_data);
            }
    }

    /**
     * This function is used to put data in thread space.
     *
     * Be aware that there is NOTHING in place to stop
     * other users of the thread to remove this data.
     * Or change the data.
     *
     * You, the developer has to make sure that there are
     * no situations in which this can arise (ie, have a
     * lock for the function which manipulates the TSD.
     *
     * @exception NullPointer
    */
    void put_data(void (*del) (void *), size_t size, void* data)
    {
        if (_data != NULL)
            if (_delete_func != NULL)
                _delete_func(_data);

        _delete_func = del;
        _data = data;
        _size = size;
        return;
    }

    size_t get_size()
    {
        return _size;
    }

    /**
        This function is used to retrieve data from the
        TSD, the thread specific data.

        Be aware that there is NOTHING in place to stop
        other users of the thread to change the data you
        get from this function.

        You, the developer has to make sure that there are
        no situations in which this can arise (ie, have a
        lock for the function which manipulates the TSD.
     */
    void get_data(void** data, size_t* size)
    {
        if (data == NULL || size == NULL)
            throw NullPointer();

        *data = _data;
        *size = _size;
    }

    // @exception NullPointer
    void copy_data(void** buf, size_t* size)
    {
        if ((buf == NULL) || (size == NULL))
            throw NullPointer();
        *buf =::operator  new(_size);
        *size = _size;
        memcpy(*buf, _data, _size);
    }

    inline Boolean operator==(const void* key) const
    {
        if (!strcmp(_key.get(), reinterpret_cast < const char *>(key)))
            return true;
        return false;
    }

    inline Boolean operator==(const thread_data& b) const
    {
        return operator==(b._key.get());
    }

    static bool equal(const thread_data* node, const void* key)
    {
        return ((thread_data *) node)->operator==(key);
    }

  private:
    void (*_delete_func) (void* data);
    thread_data();
    void* _data;
    size_t _size;
    AutoArrayPtr<char> _key;

    friend class Thread;
};


enum ThreadStatus
{
    PEGASUS_THREAD_OK = 1,      /* No problems */
    PEGASUS_THREAD_INSUFFICIENT_RESOURCES,      /* Can't allocate a thread.
                                                   Not enough memory. Try
                                                   again later */
    PEGASUS_THREAD_SETUP_FAILURE,       /* Could not allocate into the thread
                                           specific data storage. */
    PEGASUS_THREAD_UNAVAILABLE  /* Service is being destroyed and no new
                                   threads can be provided. */
};

///////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE Thread : public Linkable
{
public:

    Thread(
        ThreadReturnType(PEGASUS_THREAD_CDECL* start) (void*),
        void* parameter,
        Boolean detached);

     ~Thread();

      /** Start the thread.
          @return PEGASUS_THREAD_OK if the thread is started successfully,
          PEGASUS_THREAD_INSUFFICIENT_RESOURCES if the resources necessary
          to start the thread are not currently available.
          PEGASUS_THREAD_SETUP_FAILURE if the thread could not
          be create properly - check the 'errno' value for specific operating
          system return code.
       */
    ThreadStatus run();

    // get the user parameter
    inline void *get_parm()
    {
        return _thread_parm;
    }

    // cancellation must be deferred (not asynchronous)
    // for user-level threads the thread itself can decide
    // when it should die.
    void cancel();

    // cancel if there is a pending cancellation request
    void test_cancel();

    Boolean is_cancelled();

    // for user-level threads - put the calling thread
    // to sleep and jump to the thread scheduler.
    // platforms with preemptive scheduling and native threads
    // can define this to be a no-op.
    // platforms without preemptive scheduling like NetWare
    // or gnu portable threads will have an existing
    // routine that can be mapped to this method

    void thread_switch();

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
    // suspend this thread
    void suspend();

    // resume this thread
    void resume();
#endif

    static void sleep(Uint32 msec);

    // block the calling thread until this thread terminates
    void join();
    void thread_init();

    // thread routine needs to call this function when
    // it is ready to exit
    void exit_self(ThreadReturnType return_code);

    // stack of functions to be called when thread terminates
    // will be called last in first out (LIFO)
    // @exception IPCException
    void cleanup_push(void (*routine) (void *), void* parm);

    // @exception IPCException
    void cleanup_pop(Boolean execute = true);

    // create and initialize a tsd
    // @exception IPCException
    inline void create_tsd(const char* key, int size, void* buffer)
    {
        AutoPtr<thread_data> tsd(new thread_data(key, size, buffer));
        _tsd.insert_front(tsd.get());
        tsd.release();
    }

    // get the buffer associated with the key
    // NOTE: this call leaves the tsd LOCKED !!!!
    // @exception IPCException
    inline void *reference_tsd(const char* key)
    {
        _tsd.lock();
        thread_data *tsd = _tsd.find(thread_data::equal, key);
        if (tsd != NULL)
            return (void *) (tsd->_data);
        else
            return NULL;
    }

    // @exception IPCException
    inline void *try_reference_tsd(const char *key)
    {
        _tsd.try_lock();
        thread_data *tsd = _tsd.find(thread_data::equal, key);
        if (tsd != NULL)
            return (void *) (tsd->_data);
        else
            return NULL;
    }


    // release the lock held on the tsd
    // NOTE: assumes a corresponding and prior call to reference_tsd() !!!
    // @exception IPCException
    inline void dereference_tsd()
    {
        _tsd.unlock();
    }

    // delete the tsd associated with the key
    // @exception IPCException
    inline void delete_tsd(const char *key)
    {
        AutoPtr < thread_data > tsd(_tsd.remove(thread_data::equal, key));
    }

    // @exception IPCException
    inline void empty_tsd()
    {
        _tsd.clear();
    }

    // create or re-initialize tsd associated with the key
    // if the tsd already exists, delete the existing buffer
    // @exception IPCException
    void put_tsd(
        const char* key,
        void (*delete_func) (void *),
        Uint32 size,
        void* value)
    {
        PEGASUS_ASSERT(key != NULL);
        AutoPtr<thread_data> tsd;
        // This may throw an IPCException
        tsd.reset(_tsd.remove(thread_data::equal, key));
        tsd.reset();
        AutoPtr<thread_data> ntsd(new thread_data(key));
        ntsd->put_data(delete_func, size, value);
        // This may throw an IPCException
        _tsd.insert_front(ntsd.get());
        ntsd.release();
    }

    inline ThreadReturnType get_exit()
    {
        return _exit_code;
    }
    inline ThreadType self()
    {
        return Threads::self();
    }

    ThreadHandle getThreadHandle()
    {
        return _handle;
    }

    void detach();

    //
    // Gets the Thread object associated with the caller's thread.
    // Note: this may return NULL if no Thread object is associated
    // with the caller's thread.
    //
    static Thread *getCurrent();

    //
    // Sets the Thread object associated with the caller's thread.
    // Note: the Thread object must be placed on the heap.
    //
    static void setCurrent(Thread* thrd);

    //
    // Gets the AcceptLanguageList object associated with the caller's
    // Thread.
    // Note: this may return NULL if no Thread object, or no
    // AcceptLanguageList object, is associated with the caller's thread.
    //
    static AcceptLanguageList* getLanguages();

    //
    // Sets the AcceptLanguageList object associated with the caller's
    // Thread.
    // Note: a Thread object must have been previously associated with
    // the caller's thread.
    // Note: the AcceptLanguageList object must be placed on the heap.
    //
    static void setLanguages(AcceptLanguageList* langs);

    //
    // Removes the AcceptLanguageList object associated with the caller's
    // Thread.
    //
    static void clearLanguages();

private:
    Thread();

    static Sint8 initializeKey();

    // @exception IPCException
    inline void create_tsd(const char *key)
    {
        AutoPtr<thread_data> tsd(new thread_data(key));
        _tsd.insert_front(tsd.get());
        tsd.release();
    }

    ThreadHandle _handle;
    Boolean _is_detached;
    Boolean _cancel_enabled;
    Boolean _cancelled;

    // always pass this * as the void * parameter to the thread
    // store the user parameter in _thread_parm

    ThreadReturnType(PEGASUS_THREAD_CDECL* _start) (void *);
    List<cleanup_handler, Mutex> _cleanup;
    List<thread_data, Mutex> _tsd;

    void* _thread_parm;
    ThreadReturnType _exit_code;
    static Boolean _signals_blocked;
    static TSDKeyType _platform_thread_key;
    static Boolean _key_initialized;
    static Boolean _key_error;
};

PEGASUS_NAMESPACE_END

#endif // Pegasus_Thread_h
