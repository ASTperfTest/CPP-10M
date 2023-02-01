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

// Please be aware that the CMPI C++ API is NOT a standard currently.

#ifndef CWS_DIRECTORYCONTAINSFILE_H
#define CWS_DIRECTORYCONTAINSFILE_H

#include <Pegasus/Provider/CMPI/CmpiAssociationMI.h>

class CWS_DirectoryContainsFile : public CmpiAssociationMI
{
 public:  
  CWS_DirectoryContainsFile(const CmpiBroker &mbp, const CmpiContext& ctx);

  virtual ~CWS_DirectoryContainsFile();
	
  virtual int isUnloadable() const;
	
  virtual CmpiStatus associators
    (const CmpiContext& ctx, CmpiResult& rslt,
     const CmpiObjectPath& op, const char* asscClass, const char* resultClass,
     const char* role, const char* resultRole, const char** properties);
  virtual CmpiStatus associatorNames
    (const CmpiContext& ctx, CmpiResult& rslt,
     const CmpiObjectPath& op, const char* assocClass, const char* resultClass,
     const char* role, const char* resultRole);
   virtual CmpiStatus references
     (const CmpiContext& ctx, CmpiResult& rslt,
      const CmpiObjectPath& op, const char* resultClass, const char* role ,
      const char** properties);
   virtual CmpiStatus referenceNames
     (const CmpiContext& ctx, CmpiResult& rslt,
      const CmpiObjectPath& op, const char* resultClass, const char* role);

 private:
  CmpiBroker cppBroker;

};

#endif
