// fix.hxx -- fix class
//
// Written by Curtis Olson, started April 2000.
//
// Copyright (C) 2000  Curtis L. Olson - http://www.flightgear.org/~curt
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
// $Id: fix.hxx,v 1.6 2006-02-21 01:19:37 mfranz Exp $


#ifndef _FG_FIX_HXX
#define _FG_FIX_HXX


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <simgear/compiler.h>
#include <simgear/misc/sgstream.hxx>

#ifdef SG_HAVE_STD_INCLUDES
#  include <istream>
#elif defined( __BORLANDC__ ) || (__APPLE__)
#  include <iostream>
#else
#  include <istream.h>
#endif

SG_USING_STD(istream);

#include STL_STRING
SG_USING_STD(string);

// SG_USING_STD(cout);
// SG_USING_STD(endl);


class FGFix {

    string ident;
    double lon, lat;

public:

    inline FGFix(void);
    inline ~FGFix(void) {}

    inline const string& get_ident() const { return ident; }
    inline double get_lon() const { return lon; }
    inline double get_lat() const { return lat; }

    friend istream& operator>> ( istream&, FGFix& );
};


inline
FGFix::FGFix()
  : ident(""),
    lon(0.0),
    lat(0.0)
{
}


inline istream&
operator >> ( istream& in, FGFix& f )
{
    in >> f.lat;

    if ( f.lat > 95.0 ) {
        return in >> skipeol;
    }
    in >> f.lon >> f.ident;

    // cout << "id = " << f.ident << endl;

    return in >> skipeol;
}


#endif // _FG_FIX_HXX
