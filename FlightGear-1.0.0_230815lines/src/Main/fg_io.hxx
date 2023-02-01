// fg_io.hxx -- Higher level I/O managment routines
//
// Written by Curtis Olson, started November 1999.
//
// Copyright (C) 1999  Curtis L. Olson - http://www.flightgear.org/~curt
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// $Id: fg_io.hxx,v 1.4 2006-02-21 01:19:18 mfranz Exp $


#ifndef _FG_IO_HXX
#define _FG_IO_HXX


#include <simgear/compiler.h>
#include <simgear/structure/subsystem_mgr.hxx>

#include <vector>
#include STL_STRING

SG_USING_STD(vector);
SG_USING_STD(string);

class FGProtocol;

class FGIO : public SGSubsystem
{
public:
    FGIO();
    ~FGIO();

    void init();
    void bind();
    void unbind();
    void update( double dt );

    void shutdown_all();

private:

    FGProtocol* parse_port_config( const string& cfgstr );

private:

    // define the global I/O channel list
    //io_container global_io_list;
    vector< FGProtocol* > io_channels;
};


#endif // _FG_IO_HXX
