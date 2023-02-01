/*
FTP File YM plugin
Copyright (C) 2007-2010 Jan Holub

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"

DeleteTimer deleteTimer;

void DeleteTimer::init()
{
	timerId = 0;
	if (opt.bAutoDelete)
		start();
}

void DeleteTimer::deinit()
{
	stop();
}

void DeleteTimer::start()
{
	if (!timerId)
		timerId = SetTimer(nullptr, 0, 1000 * 60 * 5, (TIMERPROC)AutoDeleteTimerProc);
}

void DeleteTimer::stop()
{
	if (timerId) {
		KillTimer(nullptr, timerId);
		timerId = 0;
	}
}

void CALLBACK DeleteTimer::AutoDeleteTimerProc(HWND, UINT, UINT_PTR, DWORD)
{
	mir_cslock lock(DBEntry::mutexDB);

	DBEntry *entry = DBEntry::getFirst();
	while (entry != nullptr) {
		if (entry->m_deleteTS > 0 && entry->m_deleteTS < time(0)) {
			DeleteJob *job = new DeleteJob(new DBEntry(entry), nullptr);
			job->start();
		}

		entry = DBEntry::getNext(entry);
	}
}
