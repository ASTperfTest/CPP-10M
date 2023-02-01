// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "stdafx.h"

CMPlugin g_plugin;

/////////////////////////////////////////////////////////////////////////////////////////

PLUGININFOEX pluginInfoEx={
	sizeof(PLUGININFOEX),
	__PLUGIN_NAME,
	PLUGIN_MAKE_VERSION(__MAJOR_VERSION, __MINOR_VERSION, __RELEASE_NUM, __BUILD_NUM),
	__DESCRIPTION,
	__AUTHOR,
	__COPYRIGHT,
	__AUTHORWEB,
	UNICODE_AWARE,
	// {12D8FAAD-78AB-4E3C-9854-320E9EA5CC9F}
	{0x12d8faad, 0x78ab, 0x4e3c, {0x98, 0x54, 0x32, 0xe, 0x9e, 0xa5, 0xcc, 0x9f}}
};

CMPlugin::CMPlugin() :
	PLUGIN<CMPlugin>(MODULENAME, pluginInfoEx)
{}

/////////////////////////////////////////////////////////////////////////////////////////

int ModulesLoaded(WPARAM, LPARAM)
{
	// this calls ReadPrivkeyFiles (above) to set filename values (also called on ME_FOLDERS_PATH_CHANGED)
	InitOptions();

	InitDBFilter();
	InitMenu();

	InitSRMM();

	HookEvent(ME_MSG_WINDOWEVENT, WindowEvent);

	if (options.bHaveSecureIM && !g_plugin.getByte("sim_warned", 0)) {
		g_plugin.setByte("sim_warned", 1);
		options.default_policy = OTRL_POLICY_MANUAL_MOD;
		SaveOptions();
		MessageBox(nullptr, TranslateW(LANG_OTR_SECUREIM_TEXT), TranslateW(LANG_OTR_SECUREIM_TITLE), 0x30);
	}
	return 0;
}

int CMPlugin::Load()
{
	DEBUGOUTA("LOAD MIROTR");
	{
		mir_cslock lck(lib_cs);
		otr_user_state = otrl_userstate_create();
	}
	InitIcons();

	OTRL_INIT;
	INITCOMMONCONTROLSEX icce = {0};
	icce.dwSize = sizeof(icce);
	icce.dwICC = ICC_LISTVIEW_CLASSES|ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&icce);

	db_set_resident(MODULENAME, "TrustLevel");

	////////////////////////////////////////////////////////////////////////////
	// init plugin
	Proto_RegisterModule(PROTOTYPE_ENCRYPTION, MODULENAME);

	// remove us as a filter to all contacts - fix filter type problem
	if(g_plugin.getByte("FilterOrderFix", 0) != 2) {
		for (auto &hContact : Contacts())
			Proto_RemoveFromContact(hContact, MODULENAME);
		g_plugin.setByte("FilterOrderFix", 2);
	}

	// create our services
	CreateProtoServiceFunction(MODULENAME, PSS_MESSAGE, SVC_OTRSendMessage);
	CreateProtoServiceFunction(MODULENAME, PSR_MESSAGE, SVC_OTRRecvMessage);

	// hook modules loaded for updater support
	HookEvent(ME_SYSTEM_MODULESLOADED, ModulesLoaded);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

int CMPlugin::Unload()
{
	DEBUGOUTA("UNLOAD MIROTR");
	DeinitSRMM();
	DeinitDBFilter();

	mir_cslock lck(lib_cs);
	otrl_userstate_free(otr_user_state);
	return 0;
}
