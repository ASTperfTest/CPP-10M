// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
// 
// Copyright � 2000-2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright � 2001-2002 Jon Keating, Richard Hughes
// Copyright � 2002-2004 Martin �berg, Sam Kothari, Robert Rainwater
// Copyright � 2004-2010 Joe Kucera
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// -----------------------------------------------------------------------------
//
// File name      : $URL: https://miranda.googlecode.com/svn/branches/stable/miranda/protocols/IcqOscarJ/capabilities.h $
// Revision       : $Revision: 11353 $
// Last change on : $Date: 2010-03-09 19:21:03 -0500 (Tue, 09 Mar 2010) $
// Last change by : $Author: jokusoftware $
//
// DESCRIPTION:
//
//  Contains helper functions to handle OSCAR user capabilities.
//
// -----------------------------------------------------------------------------


#ifndef __CAPABILITIES_H
#define __CAPABILITIES_H


// capabilities
typedef BYTE capstr[BINARY_CAP_SIZE];
typedef BYTE shortcapstr[BINARY_SHORT_CAP_SIZE];

extern const capstr capShortCaps;

capstr* MatchCapability(BYTE *buf, int bufsize, const capstr *cap, int capsize = BINARY_CAP_SIZE);
capstr* MatchShortCapability(BYTE *buf, int bufsize, const shortcapstr *cap);


#endif /* __CAPABILITIES_H */
