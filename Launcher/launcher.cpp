/**************************************************************************
	created		:	2011-05-09
	filename	: 	launcher.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower x14-2 DLL injector
**************************************************************************/
#include "stdafx.h"

#include <InjectModule.h>
#include <SettingsManager.h>
#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "InjectModule.x64d.lib")
	#else
		#pragma comment(lib, "InjectModule.x86d.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "InjectModule.x64.lib")
	#else
		#pragma comment(lib, "InjectModule.x86.lib")
	#endif
#endif

// application entry point
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					   LPTSTR lpCmdLine, int nCmdShow)
{
	DWORD CreationFlags(CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT);
	string_t DLL32Path, ExePath, CmdLine, GamePath;
	TCHAR DirPath[_MAX_PATH] = { '\0' };
	PROCESS_INFORMATION ProcessInfo;
	const TCHAR *pCmdLine = NULL;

	GetCurrentDirectory(_MAX_PATH, DirPath);

	Windower::SettingsManager SettingsMgr(DirPath, _T("config.ini"));

	if (SettingsMgr.IsGamePathValid())
	{
		GamePath = SettingsMgr.GetGamePath();

		// default injection chain through boot
		format(ExePath, _T("%sboot\\ffxivboot.exe"), GamePath.c_str());
		format(DLL32Path, _T("%s\\bootstrap.dll"), DirPath);

		if (lpCmdLine != NULL && _tcslen(lpCmdLine) > 0U)
		{
			if (lpCmdLine != NULL)
				CmdLine = lpCmdLine;

			std::list<string_t> Parameters;
			unsigned int ParamsCount = tokenize<TCHAR>(CmdLine, Parameters, _T(" "), _T("\""));

			if (ParamsCount > 1U)
			{
				string_t Token;

				while(Parameters.empty() == false)
				{
					Token = Parameters.front();
					Parameters.pop_front();

					if (_tcsicmp(Token.c_str(), _T("/profile")) == 0 && Parameters.empty() == false)
					{
						// generate the name of the profile
						format(Token, _T("%s%s"), PROFILE_PREFIX, Parameters.front().c_str());
						Parameters.pop_front();

						// if it exits, set it as default
						if (SettingsMgr.GetSettings(Token.c_str()) != NULL)
						{
							SettingsMgr.SetDefaultProfile(Token.c_str());
							SettingsMgr.Save();
						}
					}
					else if (_tcsicmp(Token.c_str(), _T("/SID")) == 0 && Parameters.empty() == false)
					{
						Windower::WindowerProfile Settings;
						FILE *pVersionFile = NULL;						
						string_t VerFile;

						// retrieve the SID value
						Token = Parameters.front();
						Parameters.pop_front();
						// retrieve the game version
						format(VerFile, _T("%sgame\\ffxivgame.ver"), GamePath.c_str());
						_tfopen_s(&pVersionFile, VerFile.c_str(), _T("r"));
						
						if (pVersionFile != NULL && SettingsMgr.LoadDefaultProfile(Settings))
						{
							TCHAR GameVersion[24];

							memset(GameVersion, 0, 24);

							if (_fgetts(GameVersion, 24, pVersionFile))
							{
								// inject directly into the game
								format(ExePath, _T("%sgame\\ffxiv.exe"), SettingsMgr.GetGamePath());
								format(DLL32Path, _T("%s\\Windowerx14.dll"), DirPath);
								// create the command line parameters
								format(CmdLine, _T("\"DEV.TestSID=%s\"")
												_T(" \"DEV.UseSqPack=1\" \"DEV.DataPathType=1\"")
												_T(" \"DEV.LobbyHost=neolobby01.ffxiv.com\"")
												_T(" \"SYS.LobbyHost=neolobby01.ffxiv.com\"")
												_T(" \"SYS.Region=3\"")
												_T(" \"language=%ld\"")
												_T(" \"ver=%s\""), 
												Token.c_str(), Settings.GetLanguage(), GameVersion);
								// set the command line
								pCmdLine = CmdLine.c_str();
							}
							// cleanup
							fclose(pVersionFile);
						}
					}
				}				
			}
		}

		return (InjectModule::CreateProcessEx(ExePath, ProcessInfo, pCmdLine,
											  CreationFlags, DLL32Path.c_str()) == FALSE);
	}
	else
	{
		MessageBox(NULL, _T("The configuration file is missing or invalid.\n")
						 _T("Starting the configuration utility..."),
						 _T("Error!"), MB_OK | MB_ICONWARNING);
		// start the configuration tool
		ShellExecute(NULL, NULL, _T("WindowerConfig.exe"), NULL, DirPath, SW_SHOW);
	}

	return -1;
}