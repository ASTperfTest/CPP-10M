/*
CmdLine plugin for Miranda IM

Copyright © 2007 Cristian Libotean

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef M_CMDLINE_UTILS_H
#define M_CMDLINE_UTILS_H

#include <stdarg.h>

#define ANCHOR_LEFT     0x000001
#define ANCHOR_RIGHT		0x000002
#define ANCHOR_TOP      0x000004
#define ANCHOR_BOTTOM   0x000008
#define ANCHOR_ALL      ANCHOR_LEFT | ANCHOR_RIGHT | ANCHOR_TOP | ANCHOR_BOTTOM

int Info(char *title, char *format, ...);

char *BinToHex(int size, uint8_t *data);
void HexToBin(char *inData, ULONG &size, uint8_t* &outData);

void ScreenToClient(HWND hWnd, LPRECT rect);
void AnchorMoveWindow(HWND window, const WINDOWPOS *parentPos, int anchors);
RECT AnchorCalcPos(HWND window, const RECT *rParent, const WINDOWPOS *parentPos, int anchors);

int GetStringFromDatabase(MCONTACT hContact, char *szModule, char *szSettingName, char *szError, char *szResult, size_t size);
int GetStringFromDatabase(MCONTACT hContact, char *szModule, char *szSettingName, wchar_t *szError, wchar_t *szResult, size_t count);
int GetStringFromDatabase(char *szSettingName, char *szError, char *szResult, size_t size);
int GetStringFromDatabase(char *szSettingName, wchar_t *szError, wchar_t *szResult, size_t count);

wchar_t* GetContactName(MCONTACT hContact, char *szProto);
wchar_t* GetContactID(MCONTACT hContact, char *szProto);
MCONTACT GetContactFromID(wchar_t *szID, char *szProto);
void Proto_GetBaseAccountName(MCONTACT hContact, char *szProto, size_t size);

int MyPUShowMessage(char *lpzText, uint8_t kind);

#endif