// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
// 
// Copyright � 2000-2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright � 2001-2002 Jon Keating, Richard Hughes
// Copyright � 2002-2004 Martin �berg, Sam Kothari, Robert Rainwater
// Copyright � 2004-2008 Joe Kucera
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// -----------------------------------------------------------------------------
//
// File name      : $URL: https://miranda.googlecode.com/svn/branches/stable/miranda/protocols/IcqOscarJ/icq_popups.h $
// Revision       : $Revision: 7484 $
// Last change on : $Date: 2008-03-21 05:43:39 -0400 (Fri, 21 Mar 2008) $
// Last change by : $Author: ghazan $
//
// DESCRIPTION:
//
//  Headers for PopUp Plugin support
//
// -----------------------------------------------------------------------------

#ifndef __ICQ_POPUPS_H
#define __ICQ_POPUPS_H


#define POPTYPE_SPAM    254  // this is for spambots


void InitPopUps();
void InitPopupOpts(WPARAM wParam);


#endif /* __ICQ_POPUPS_H */
