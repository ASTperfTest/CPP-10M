// native.cxx -- FGFS "Native" protocal class
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
// $Id: native.cxx,v 1.3 2006-02-21 01:19:47 mfranz Exp $


#include <simgear/debug/logstream.hxx>
#include <simgear/io/iochannel.hxx>

#include "native.hxx"


FGNative::FGNative() {
}

FGNative::~FGNative() {
}


// open hailing frequencies
bool FGNative::open() {
    if ( is_enabled() ) {
	SG_LOG( SG_IO, SG_ALERT, "This shouldn't happen, but the channel " 
		<< "is already in use, ignoring" );
	return false;
    }

    SGIOChannel *io = get_io_channel();

    if ( ! io->open( get_direction() ) ) {
	SG_LOG( SG_IO, SG_ALERT, "Error opening channel communication layer." );
	return false;
    }

    set_enabled( true );

    return true;
}


// process work for this port
bool FGNative::process() {
    SGIOChannel *io = get_io_channel();
    int length = sizeof(*cur_fdm_state);

    if ( get_direction() == SG_IO_OUT ) {
	// cout << "size of cur_fdm_state = " << length << endl;
	buf = *cur_fdm_state;
	if ( ! io->write( (char *)(& buf), length ) ) {
	    SG_LOG( SG_IO, SG_ALERT, "Error writing data." );
	    return false;
	}
    } else if ( get_direction() == SG_IO_IN ) {
	if ( io->get_type() == sgFileType ) {
	    if ( io->read( (char *)(& buf), length ) == length ) {
		SG_LOG( SG_IO, SG_DEBUG, "Success reading data." );
		*cur_fdm_state = buf;
	    }
	} else {
	    while ( io->read( (char *)(& buf), length ) == length ) {
		SG_LOG( SG_IO, SG_DEBUG, "Success reading data." );
		*cur_fdm_state = buf;
	    }
	}
    }

    return true;
}


// close the channel
bool FGNative::close() {
    SGIOChannel *io = get_io_channel();

    set_enabled( false );

    if ( ! io->close() ) {
	return false;
    }

    return true;
}
