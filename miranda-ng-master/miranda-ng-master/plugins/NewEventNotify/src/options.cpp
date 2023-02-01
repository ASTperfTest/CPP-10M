/*
	Name: NewEventNotify - Plugin for Miranda IM
	File: options.c - Manages Option Dialogs and Settings
	Version: 0.0.4
	Description: Notifies you about some events
	Author: icebreaker, <icebreaker@newmail.net>
	Date: 22.07.02 13:06 / Update: 16.09.02 17:45
	Copyright: (C) 2002 Starzinger Michael

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "stdafx.h"

void CMPlugin::OptionsRead(void)
{
	bPreview = getBool(OPT_PREVIEW, true);
	bMergePopup = getBool(OPT_MERGEPOPUP, true);
	bMsgWindowCheck = getBool(OPT_MSGWINDOWCHECK, true);
	bMsgReplyWindow = getBool(OPT_MSGREPLYWINDOW, false);

	msg.bDefault = getBool(OPT_COLDEFAULT_MESSAGE, false);
	msg.bWindows = getBool(OPT_COLWIN_MESSAGE, false);
	msg.backColor = getDword(OPT_COLBACK_MESSAGE, DEFAULT_COLBACK);
	msg.textColor = getDword(OPT_COLTEXT_MESSAGE, DEFAULT_COLTEXT);
	msg.iDelay = getDword(OPT_DELAY_MESSAGE, DEFAULT_DELAY);

	file.bDefault = getBool(OPT_COLDEFAULT_FILE, false);
	file.bWindows = getBool(OPT_COLWIN_FILE, false);
	file.backColor = getDword(OPT_COLBACK_FILE, DEFAULT_COLBACK);
	file.textColor = getDword(OPT_COLTEXT_FILE, DEFAULT_COLTEXT);
	file.iDelay = getDword(OPT_DELAY_FILE, DEFAULT_DELAY);

	err.bDefault = getBool(OPT_COLDEFAULT_ERR, false);
	err.bWindows = getBool(OPT_COLWIN_ERR, false);
	err.backColor = getDword(OPT_COLBACK_ERR, DEFAULT_COLBACK);
	err.textColor = getDword(OPT_COLTEXT_ERR, DEFAULT_COLTEXT);
	err.iDelay = getDword(OPT_DELAY_ERR, DEFAULT_DELAY);

	other.bDefault = getBool(OPT_COLDEFAULT_OTHERS, false);
	other.bWindows = getBool(OPT_COLWIN_OTHERS, false);
	other.backColor = getDword(OPT_COLBACK_OTHERS, DEFAULT_COLBACK);
	other.textColor = getDword(OPT_COLTEXT_OTHERS, DEFAULT_COLTEXT);
	other.iDelay = getDword(OPT_DELAY_OTHERS, DEFAULT_DELAY);

	maskNotify = getByte(OPT_MASKNOTIFY, MASK_MESSAGE | MASK_ERROR | MASK_FILE | MASK_OTHER);
	maskActL = getByte(OPT_MASKACTL, MASK_OPEN | MASK_REMOVE | MASK_DISMISS);
	maskActR = getByte(OPT_MASKACTR, MASK_REMOVE | MASK_DISMISS);
	maskActTE = getByte(OPT_MASKACTTE, MASK_DISMISS);
	
	iDelayDefault = DBGetContactSettingRangedWord(NULL, "Popup", "Seconds", SETTING_LIFETIME_DEFAULT, SETTING_LIFETIME_MIN, SETTING_LIFETIME_MAX);

	bShowDate = getBool(OPT_SHOW_DATE, true);
	bShowTime = getBool(OPT_SHOW_TIME, true);
	bShowHeaders = getBool(OPT_SHOW_HEADERS, false);
	iNumberMsg = getByte(OPT_NUMBER_MSG, 1);
	bShowON = getBool(OPT_SHOW_ON, true);
	bHideSend = getBool(OPT_HIDESEND, true);
	bNoRSS = getBool(OPT_NORSS, false);
	bReadCheck = getBool(OPT_READCHECK, true);
}

void CMPlugin::OptionsWrite(void)
{
	setByte(OPT_PREVIEW, bPreview);
	setByte(OPT_MERGEPOPUP, bMergePopup);
	setByte(OPT_MSGWINDOWCHECK, bMsgWindowCheck);
	setByte(OPT_MSGREPLYWINDOW, bMsgReplyWindow);

	setByte(OPT_COLWIN_MESSAGE, msg.bWindows);
	setByte(OPT_COLDEFAULT_MESSAGE, msg.bDefault);
	setDword(OPT_COLBACK_MESSAGE, msg.backColor);
	setDword(OPT_COLTEXT_MESSAGE, msg.textColor);
	setDword(OPT_DELAY_MESSAGE, msg.iDelay);

	setByte(OPT_COLWIN_FILE, file.bWindows);
	setByte(OPT_COLDEFAULT_FILE, file.bDefault);
	setDword(OPT_COLBACK_FILE, file.backColor);
	setDword(OPT_COLTEXT_FILE, file.textColor);
	setDword(OPT_DELAY_FILE, file.iDelay);

	setByte(OPT_COLWIN_ERR, err.bWindows);
	setByte(OPT_COLDEFAULT_ERR, err.bDefault);
	setDword(OPT_COLBACK_ERR, err.backColor);
	setDword(OPT_COLTEXT_ERR, err.textColor);
	setDword(OPT_DELAY_ERR, err.iDelay);

	setByte(OPT_COLWIN_OTHERS, other.bWindows);
	setByte(OPT_COLDEFAULT_OTHERS, other.bDefault);
	setDword(OPT_COLBACK_OTHERS, other.backColor);
	setDword(OPT_COLTEXT_OTHERS, other.textColor);
	setDword(OPT_DELAY_OTHERS, other.iDelay);

	setDword(OPT_LIMITPREVIEW, iLimitPreview);

	setByte(OPT_MASKNOTIFY, (uint8_t)maskNotify);
	setByte(OPT_MASKACTL, (uint8_t)maskActL);
	setByte(OPT_MASKACTR, (uint8_t)maskActR);
	setByte(OPT_MASKACTTE, (uint8_t)maskActTE);

	setByte(OPT_SHOW_DATE, bShowDate);
	setByte(OPT_SHOW_TIME, bShowTime);
	setByte(OPT_SHOW_HEADERS, bShowHeaders);
	setByte(OPT_NUMBER_MSG, (uint8_t)iNumberMsg);
	setByte(OPT_SHOW_ON, bShowON);
	setByte(OPT_HIDESEND, bHideSend);
	setByte(OPT_NORSS, bNoRSS);
	setByte(OPT_READCHECK, bReadCheck);
}

static void EnableDlgItem(HWND hWnd, int iCtrl, BOOL bEnable)
{
	EnableWindow(GetDlgItem(hWnd, iCtrl), bEnable);
}

/////////////////////////////////////////////////////////////////////////////////////////

class COptionsBaseDlg : public CDlgBase
{
	void OnFinish(CDlgBase *)
	{
		g_plugin.OptionsWrite();
	}

public:
	COptionsBaseDlg(int iDlg) :
		CDlgBase(g_plugin, iDlg)
	{
		m_OnFinishWizard = Callback(this, &COptionsBaseDlg::OnFinish);
	}

	void OnReset() override
	{
		g_plugin.OptionsRead();
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class COptionsMainDlg : public COptionsBaseDlg
{
	CCtrlSpin spinLimit;
	CCtrlCheck chkLimit;
	CCtrlTreeOpts m_opts;

public:
	COptionsMainDlg() :
		COptionsBaseDlg(IDD_OPT_MAIN),
		m_opts(this, IDC_OPT_TREE),
		chkLimit(this, IDC_LIMITPREVIEW),
		spinLimit(this, IDC_MESSAGEPREVIEWLIMITSPIN, 1000)
	{
		auto *pwszSection = TranslateT("General options");
		m_opts.AddOption(pwszSection, TranslateT("Show preview of event in popup"), g_plugin.bPreview);
		m_opts.AddOption(pwszSection, TranslateT("Enable event notifications for instant messages"), g_plugin.bPopups);
		m_opts.AddOption(pwszSection, TranslateT("Enable event notifications for group chats"), g_plugin.bMucPopups);

		pwszSection = TranslateT("Notify me of...");
		m_opts.AddOption(pwszSection, TranslateT("Message"), g_plugin.maskNotify, MASK_MESSAGE);
		m_opts.AddOption(pwszSection, TranslateT("Error"), g_plugin.maskNotify, MASK_ERROR);
		m_opts.AddOption(pwszSection, TranslateT("File"), g_plugin.maskNotify, MASK_FILE);
		m_opts.AddOption(pwszSection, TranslateT("Others"), g_plugin.maskNotify, MASK_OTHER);

		pwszSection = TranslateT("Left click actions");
		m_opts.AddOption(pwszSection, TranslateT("Dismiss popup"), g_plugin.maskActL, MASK_DISMISS);
		m_opts.AddOption(pwszSection, TranslateT("Open event"), g_plugin.maskActL, MASK_OPEN);
		m_opts.AddOption(pwszSection, TranslateT("Dismiss event"), g_plugin.maskActL, MASK_REMOVE);

		pwszSection = TranslateT("Right click actions");
		m_opts.AddOption(pwszSection, TranslateT("Dismiss popup"), g_plugin.maskActR, MASK_DISMISS);
		m_opts.AddOption(pwszSection, TranslateT("Open event"), g_plugin.maskActR, MASK_OPEN);
		m_opts.AddOption(pwszSection, TranslateT("Dismiss event"), g_plugin.maskActR, MASK_REMOVE);

		pwszSection = TranslateT("Timeout actions");
		m_opts.AddOption(pwszSection, TranslateT("Dismiss popup"), g_plugin.maskActTE, MASK_DISMISS);
		m_opts.AddOption(pwszSection, TranslateT("Open event"), g_plugin.maskActTE, MASK_OPEN);
		m_opts.AddOption(pwszSection, TranslateT("Dismiss event"), g_plugin.maskActTE, MASK_REMOVE);

		pwszSection = TranslateT("Misc options");
		m_opts.AddOption(pwszSection, TranslateT("No popups for RSS contacts"), g_plugin.bNoRSS);
		m_opts.AddOption(pwszSection, TranslateT("No popups for read messages (conflicts with AntiSpam plugin)"), g_plugin.bReadCheck);

		chkLimit.OnChange = Callback(this, &COptionsMainDlg::onChange_Limit);
	}

	bool OnInitDialog() override
	{
		chkLimit.SetState(g_plugin.iLimitPreview > 0);
		spinLimit.SetPosition(g_plugin.iLimitPreview);
		return true;
	}

	bool OnApply() override
	{
		g_plugin.iLimitPreview = (chkLimit.GetState()) ? spinLimit.GetPosition() : 0;
		m_opts.OnApply();
		return true;
	}

	void onChange_Limit(CCtrlCheck *)
	{
		bool bEnabled = chkLimit.GetState();
		spinLimit.Enable(bEnabled);
		EnableDlgItem(m_hwnd, IDC_MESSAGEPREVIEWLIMIT, bEnabled);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class COptionsEventsDlg : public COptionsBaseDlg
{
	CCtrlSpin spinMessage, spinFile, spinErr, spinOther;
	CCtrlCheck chkWinColorMsg, chkWinColorFile, chkWinColorErr, chkWinColorOthers;
	CCtrlCheck chkDefaultColorMsg, chkDefaultColorFile, chkDefaultColorErr, chkDefaultColorOthers;
	CCtrlColor clrBackMessage, clrTextMessage, clrBackFile, clrTextFile, clrBackErr, clrTextErr, clrBackOther, clrTextOther;
	CCtrlButton btnPreview;

	void GrabData()
	{
		// update options
		g_plugin.msg.bWindows = chkWinColorMsg.IsChecked();
		g_plugin.msg.bDefault = chkDefaultColorMsg.IsChecked();
		g_plugin.msg.backColor = clrBackMessage.GetColor();
		g_plugin.msg.textColor = clrTextMessage.GetColor();
		g_plugin.msg.iDelay = spinMessage.GetPosition();

		g_plugin.file.bWindows = chkWinColorFile.IsChecked();
		g_plugin.file.bDefault = chkDefaultColorFile.IsChecked();
		g_plugin.file.backColor = clrBackFile.GetColor();
		g_plugin.file.textColor = clrTextFile.GetColor();
		g_plugin.file.iDelay = spinFile.GetPosition();

		g_plugin.err.bWindows = chkWinColorErr.IsChecked();
		g_plugin.err.bDefault = chkDefaultColorErr.IsChecked();
		g_plugin.err.backColor = clrBackErr.GetColor();
		g_plugin.err.textColor = clrTextErr.GetColor();
		g_plugin.err.iDelay = spinErr.GetPosition();

		g_plugin.other.bWindows = chkWinColorOthers.IsChecked();
		g_plugin.other.bDefault = chkDefaultColorOthers.IsChecked();
		g_plugin.other.backColor = clrBackOther.GetColor();
		g_plugin.other.textColor = clrTextOther.GetColor();
		g_plugin.other.iDelay = spinOther.GetPosition();
	}

public:
	COptionsEventsDlg() :
		COptionsBaseDlg(IDD_OPT_EVENT_TYPES),
		btnPreview(this, IDC_PREVIEW),
		clrBackMessage(this, IDC_COLBACK_MESSAGE),
		clrTextMessage(this, IDC_COLTEXT_MESSAGE),
		clrBackFile(this, IDC_COLBACK_FILE),
		clrTextFile(this, IDC_COLTEXT_FILE),
		clrBackErr(this, IDC_COLBACK_ERR),
		clrTextErr(this, IDC_COLTEXT_ERR),
		clrBackOther(this, IDC_COLBACK_OTHERS),
		clrTextOther(this, IDC_COLTEXT_OTHERS),
		chkWinColorMsg(this, IDC_CHKWINCOL_MESSAGE),
		chkWinColorFile(this, IDC_CHKWINCOL_FILE),
		chkWinColorErr(this, IDC_CHKWINCOL_ERR),
		chkWinColorOthers(this, IDC_CHKWINCOL_OTHERS),
		chkDefaultColorMsg(this, IDC_CHKDEFAULTCOL_MESSAGE),
		chkDefaultColorFile(this, IDC_CHKDEFAULTCOL_FILE),
		chkDefaultColorErr(this, IDC_CHKDEFAULTCOL_ERR),
		chkDefaultColorOthers(this, IDC_CHKDEFAULTCOL_OTHERS),
		spinErr(this, IDC_SPIN_ERR, 1000, -1),
		spinFile(this, IDC_SPIN_FILE, 1000, -1),
		spinOther(this, IDC_SPIN_OTHERS, 1000, -1),
		spinMessage(this, IDC_SPIN_MESSAGE, 1000, -1)
	{
		btnPreview.OnClick = Callback(this, &COptionsEventsDlg::onClick_Preview);

		chkDefaultColorMsg.OnChange = chkWinColorMsg.OnChange = Callback(this, &COptionsEventsDlg::onChange_DefaultMsg);
		chkDefaultColorFile.OnChange = chkWinColorFile.OnChange = Callback(this, &COptionsEventsDlg::onChange_DefaultFile);
		chkDefaultColorErr.OnChange = chkWinColorErr.OnChange = Callback(this, &COptionsEventsDlg::onChange_DefaultErr);
		chkDefaultColorOthers.OnChange = chkWinColorOthers.OnChange = Callback(this, &COptionsEventsDlg::onChange_DefaultOther);
	}

	bool OnInitDialog() override
	{
		// make dialog represent the current options
		clrBackMessage.SetColor(g_plugin.msg.backColor);
		clrBackFile.SetColor(g_plugin.file.backColor);
		clrBackErr.SetColor(g_plugin.err.backColor);
		clrBackOther.SetColor(g_plugin.other.backColor);

		clrTextMessage.SetColor(g_plugin.msg.textColor);
		clrTextFile.SetColor(g_plugin.file.textColor);
		clrTextErr.SetColor(g_plugin.err.textColor);
		clrTextOther.SetColor(g_plugin.other.textColor);

		chkDefaultColorMsg.SetState(g_plugin.msg.bDefault);
		chkDefaultColorFile.SetState(g_plugin.file.bDefault);
		chkDefaultColorErr.SetState(g_plugin.err.bDefault);
		chkDefaultColorOthers.SetState(g_plugin.other.bDefault);

		chkWinColorMsg.SetState(g_plugin.msg.bWindows);
		chkWinColorFile.SetState(g_plugin.file.bWindows);
		chkWinColorErr.SetState(g_plugin.err.bWindows);
		chkWinColorOthers.SetState(g_plugin.other.bWindows);

		spinMessage.SetPosition(g_plugin.msg.iDelay);
		spinFile.SetPosition(g_plugin.file.iDelay);
		spinErr.SetPosition(g_plugin.err.iDelay);
		spinOther.SetPosition(g_plugin.other.iDelay);

		OnChange();
		return true;
	}

	bool OnApply() override
	{
		GrabData();
		return true;
	}

	void onClick_Preview(CCtrlButton *)
	{
		GrabData();
		PopupShow(0, 0, EVENTTYPE_MESSAGE);
		PopupShow(0, 0, EVENTTYPE_ERRMSG);
		PopupShow(0, 0, EVENTTYPE_FILE);
		PopupShow(0, 0, -1);
	}

	void onChange_DefaultMsg(CCtrlCheck *)
	{
		bool b1 = !chkDefaultColorMsg.IsChecked(), b2 = !chkWinColorMsg.IsChecked();
		chkWinColorMsg.Enable(b1);
		chkDefaultColorMsg.Enable(b2);
		clrBackMessage.Enable(b1 && b2);
		clrTextMessage.Enable(b1 && b2);
	}

	void onChange_DefaultFile(CCtrlCheck *)
	{
		bool b1 = !chkDefaultColorFile.IsChecked(), b2 = !chkWinColorFile.IsChecked();
		chkWinColorFile.Enable(b1);
		chkDefaultColorFile.Enable(b2);
		clrBackFile.Enable(b1 && b2);
		clrTextFile.Enable(b1 && b2);
	}

	void onChange_DefaultErr(CCtrlCheck *)
	{
		bool b1 = !chkDefaultColorErr.IsChecked(), b2 = !chkWinColorErr.IsChecked();
		chkWinColorErr.Enable(b1);
		chkDefaultColorErr.Enable(b2);
		clrBackErr.Enable(b1 && b2);
		clrTextErr.Enable(b1 && b2);
	}

	void onChange_DefaultOther(CCtrlCheck *)
	{
		bool b1 = !chkDefaultColorOthers.IsChecked(), b2 = !chkWinColorOthers.IsChecked();
		chkWinColorOthers.Enable(b1);
		chkDefaultColorOthers.Enable(b2);
		clrBackOther.Enable(b1 && b2);
		clrTextOther.Enable(b1 && b2);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class COptionsMessageDlg : public COptionsBaseDlg
{
	CCtrlSpin spinMsgNumber;

	void GrabData()
	{
		g_plugin.iNumberMsg = spinMsgNumber.GetPosition();

		g_plugin.bMergePopup = IsDlgButtonChecked(m_hwnd, IDC_CHKMERGEPOPUP);
		g_plugin.bMsgWindowCheck = IsDlgButtonChecked(m_hwnd, IDC_CHKWINDOWCHECK);
		g_plugin.bMsgReplyWindow = IsDlgButtonChecked(m_hwnd, IDC_CHKREPLYWINDOW);
		g_plugin.bShowDate = IsDlgButtonChecked(m_hwnd, IDC_CHKSHOWDATE);
		g_plugin.bShowTime = IsDlgButtonChecked(m_hwnd, IDC_CHKSHOWTIME);
		g_plugin.bShowHeaders = IsDlgButtonChecked(m_hwnd, IDC_CHKSHOWHEADERS);
		g_plugin.bShowON = IsDlgButtonChecked(m_hwnd, IDC_RDOLD);
		g_plugin.bShowON = BST_UNCHECKED == IsDlgButtonChecked(m_hwnd, IDC_RDNEW);
		g_plugin.bHideSend = IsDlgButtonChecked(m_hwnd, IDC_CHKHIDESEND);
	}

public:
	COptionsMessageDlg() :
		COptionsBaseDlg(IDD_OPT_MESSAGE),
		spinMsgNumber(this, IDC_SPIN_MSGNUMBER, 10, 1)
	{}

	bool OnInitDialog() override
	{
		spinMsgNumber.SetPosition(g_plugin.iNumberMsg);

		CheckDlgButton(m_hwnd, IDC_CHKMERGEPOPUP, g_plugin.bMergePopup);
		CheckDlgButton(m_hwnd, IDC_CHKWINDOWCHECK, g_plugin.bMsgWindowCheck);
		CheckDlgButton(m_hwnd, IDC_CHKREPLYWINDOW, g_plugin.bMsgReplyWindow);
		CheckDlgButton(m_hwnd, IDC_CHKSHOWDATE, g_plugin.bShowDate);
		CheckDlgButton(m_hwnd, IDC_CHKSHOWTIME, g_plugin.bShowTime);
		CheckDlgButton(m_hwnd, IDC_CHKSHOWHEADERS, g_plugin.bShowHeaders);
		CheckDlgButton(m_hwnd, IDC_RDNEW, !g_plugin.bShowON);
		CheckDlgButton(m_hwnd, IDC_RDOLD, g_plugin.bShowON);
		CheckDlgButton(m_hwnd, IDC_CHKHIDESEND, g_plugin.bHideSend);

		OnChange();
		return true;
	}
	
	void OnChange() override
	{
		GrabData();

		// disable merge messages options when is not using
		EnableDlgItem(m_hwnd, IDC_CHKSHOWDATE, g_plugin.bMergePopup);
		EnableDlgItem(m_hwnd, IDC_CHKSHOWTIME, g_plugin.bMergePopup);
		EnableDlgItem(m_hwnd, IDC_CHKSHOWHEADERS, g_plugin.bMergePopup);
		EnableDlgItem(m_hwnd, IDC_NUMBERMSG, g_plugin.bMergePopup);
		EnableDlgItem(m_hwnd, IDC_LBNUMBERMSG, g_plugin.bMergePopup);
		EnableDlgItem(m_hwnd, IDC_RDNEW, g_plugin.bMergePopup && g_plugin.iNumberMsg);
		EnableDlgItem(m_hwnd, IDC_RDOLD, g_plugin.bMergePopup && g_plugin.iNumberMsg);
	}

	bool OnApply() override
	{
		GrabData();
		return true;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////
// Module entry point

int OptionsAdd(WPARAM addInfo, LPARAM)
{
	OPTIONSDIALOGPAGE odp = {};
	odp.flags = ODPF_BOLDGROUPS;
	odp.szTitle.a = LPGEN("Event Notify");
	odp.szGroup.a = LPGEN("Popups");

	odp.szTab.a = LPGEN("Main options");
	odp.pDialog = new COptionsMainDlg();
	g_plugin.addOptions(addInfo, &odp);

	odp.szTab.a = LPGEN("Message events");
	odp.pDialog = new COptionsMessageDlg();
	g_plugin.addOptions(addInfo, &odp);

	odp.szTab.a = LPGEN("Event types");
	odp.pDialog = new COptionsEventsDlg();
	g_plugin.addOptions(addInfo, &odp);
	return 0;
}
