#define AppName "x14-2"
#define SourcePath "..\Release\"
#define AppPublisher "North Edge"
#define AppVersion GetFileVersion("..\Release\x14-2core.x86.dll")
#define AppURL "http://x14.north-edge.com/x14-2"
#define AppExeName "Launcher.exe"
#define use_vc2015

#define SignToolExe "C:\Program Files (x86)\Windows Kits\10\bin\x64\signtool.exe"
#define SignToolCmd "sign /f {%CODE_SIGNING_CERT} /p {%CODE_SIGNING_PWD} /t http://timestamp.verisign.com/scripts/timstamp.dll"

#include "scripts\products.iss"

#include "scripts\products\stringversion.iss"
#include "scripts\products\winversion.iss"
#include "scripts\products\fileversion.iss"
#include "scripts\products\vcredist2015.iss"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{CA5A07B0-3C04-40AD-8E79-B1F2BB411DFC}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
AppUpdatesURL={#AppURL}
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
AllowNoIcons=yes
LicenseFile=license.rtf
WizardSmallImageFile=x14-logo.bmp
WizardImageFile=x14-banner.bmp
OutputDir=.\
OutputBaseFilename=x14-setup-{#AppVersion}
SetupIconFile=..\Resources\x14.ico
Compression=lzma
SolidCompression=yes

[LangOptions]
DialogFontName=Arial

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon";    Description: "{cm:CreateDesktopIcon}";      GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: quicklaunchicon;  Description: "{cm:CreateQuickLaunchIcon}";  GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Components]
Name: "x14Base";                      Description: "{#AppName} base files"; Types: full compact custom; Flags: fixed
Name: "BootstrapPlugins";             Description: "Bootstrap plugins"; Types: full custom
Name: "BootstrapPlugins/AutoLogin";   Description: "AutoLogin {#GetFileVersion("..\Release\Plugins\x86\AutoLogin.dll")} — Auto fill the login form";          Types: full custom

;;;;; x86
Name: "x14PluginsX86";                   Description: "{#AppName} Plugins (32-bit)"; Types: full custom
Name: "x14PluginsX86/AutoBlacklist";     Description: "AutoBlacklist {#GetFileVersion("..\Release\Plugins\x86\AutoBlacklist.dll")} — Blacklist spammers automatically";  Types: full custom
;Name: "x14PluginsX86/Distance";          Description: "Distance {#GetFileVersion("..\Release\Plugins\x86\Distance.dll")} — Distance from player to target";              Types: full custom
Name: "x14PluginsX86/ChatLog";           Description: "ChatLog {#GetFileVersion("..\Release\Plugins\x86\ChatLog.dll")} — Log the game chat to a file";                   Types: full custom
;Name: "x14PluginsX86/ExpWatch";          Description: "ExpWatch {#GetFileVersion("..\Release\Plugins\x86\ExpWatch.dll")} — Keep track of experience earned";             Types: full custom
Name: "x14PluginsX86/TellDetect";        Description: "TellDetect {#GetFileVersion("..\Release\Plugins\x86\TellDetect.dll")} — Play a custom sound on tell";             Types: full custom
Name: "x14PluginsX86/Timestamp";         Description: "Timestamp {#GetFileVersion("..\Release\Plugins\x86\Timestamp.dll")} — Add a timestamp to the game chat";          Types: full custom
;Name: "x14PluginsX86/Gatherer";          Description: "Gatherer {#GetFileVersion("..\Release\Plugins\x86\Gatherer.dll")} — A helper for gathering unspoiled nodes";      Types: full custom

;;;;; x64

Name: "x14PluginsX64";                   Description: "{#AppName} Plugins (64-bit)"; Check: IsWin64; Types: full custom;
Name: "x14PluginsX64/AutoBlacklist";     Description: "AutoBlacklist {#GetFileVersion("..\Release\Plugins\x64\AutoBlacklist.dll")} — Blacklist spammers automatically";  Types: full custom
;Name: "x14PluginsX64/Distance";          Description: "Distance {#GetFileVersion("..\Release\Plugins\x64\Distance.dll")} — Distance from player to target";              Types: full custom
Name: "x14PluginsX64/ChatLog";           Description: "ChatLog {#GetFileVersion("..\Release\Plugins\x64\ChatLog.dll")} — Log the game chat to a file";                   Types: full custom
;Name: "x14PluginsX64/ExpWatch";          Description: "ExpWatch {#GetFileVersion("..\Release\Plugins\x64\ExpWatch.dll")} — Keep track of experience earned";             Types: full custom
Name: "x14PluginsX64/TellDetect";        Description: "TellDetect {#GetFileVersion("..\Release\Plugins\x64\TellDetect.dll")} — Play a custom sound on tell";             Types: full custom
Name: "x14PluginsX64/Timestamp";         Description: "Timestamp {#GetFileVersion("..\Release\Plugins\x64\Timestamp.dll")} — Add a timestamp to the game chat";          Types: full custom
;Name: "x14PluginsX64/Gatherer";          Description: "Gatherer {#GetFileVersion("..\Release\Plugins\x64\Gatherer.dll")} — A helper for gathering unspoiled nodes";      Types: full custom

[Files]
;;;;; x86
Source: "{#SourcePath}Plugins\x86\AutoLogin.dll";     DestDir: "{app}/plugins/x86/"; Components: BootstrapPlugins/AutoLogin; Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\x86\AutoBlacklist.dll"; DestDir: "{app}/plugins/x86/"; Components: x14PluginsX86/AutoBlacklist;   Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\x86\ChatLog.dll";       DestDir: "{app}/plugins/x86/"; Components: x14PluginsX86/ChatLog;         Flags: replacesameversion skipifsourcedoesntexist
;Source: "{#SourcePath}Plugins\x86\Distance.dll";      DestDir: "{app}/plugins/x86/"; Components: x14PluginsX86/Distance;        Flags: replacesameversion skipifsourcedoesntexist
;Source: "{#SourcePath}Plugins\x86\ExpWatch.dll";      DestDir: "{app}/plugins/x86/"; Components: x14PluginsX86/ExpWatch;        Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\x86\TellDetect.dll";    DestDir: "{app}/plugins/x86/"; Components: x14PluginsX86/TellDetect;      Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\x86\Timestamp.dll";     DestDir: "{app}/plugins/x86/"; Components: x14PluginsX86/Timestamp;       Flags: replacesameversion skipifsourcedoesntexist
;Source: "{#SourcePath}Plugins\x86\Gatherer.dll";      DestDir: "{app}/plugins/x86/"; Components: x14PluginsX86/Gatherer;        Flags: replacesameversion skipifsourcedoesntexist

;;;;; x64
Source: "{#SourcePath}Plugins\x64\AutoLogin.dll";     DestDir: "{app}/plugins/x64/"; Components: BootstrapPlugins/AutoLogin; Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\x64\AutoBlacklist.dll"; DestDir: "{app}/plugins/x64/"; Components: x14PluginsX64/AutoBlacklist;   Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\x64\ChatLog.dll";       DestDir: "{app}/plugins/x64/"; Components: x14PluginsX64/ChatLog;         Flags: replacesameversion skipifsourcedoesntexist
;Source: "{#SourcePath}Plugins\x64\Distance.dll";      DestDir: "{app}/plugins/x64/"; Components: x14PluginsX64/Distance;        Flags: replacesameversion skipifsourcedoesntexist
;Source: "{#SourcePath}Plugins\x64\ExpWatch.dll";      DestDir: "{app}/plugins/x64/"; Components: x14PluginsX64/ExpWatch;        Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\x64\TellDetect.dll";    DestDir: "{app}/plugins/x64/"; Components: x14PluginsX64/TellDetect;      Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\x64\Timestamp.dll";     DestDir: "{app}/plugins/x64/"; Components: x14PluginsX64/Timestamp;       Flags: replacesameversion skipifsourcedoesntexist
;Source: "{#SourcePath}Plugins\x64\Gatherer.dll";      DestDir: "{app}/plugins/x64/"; Components: x14PluginsX64/Gatherer;        Flags: replacesameversion skipifsourcedoesntexist

Source: "{#SourcePath}x14-2core.x64.dll";         DestDir: "{app}"; Components: x14Base; Check: IsWin64; Flags: replacesameversion
Source: "{#SourcePath}x14-2core.x86.dll";         DestDir: "{app}"; Components: x14Base; Flags: replacesameversion
Source: "{#SourcePath}bootstrap.dll";             DestDir: "{app}"; Components: x14Base; Flags: replacesameversion
Source: "{#SourcePath}Launcher.exe";              DestDir: "{app}"; Components: x14Base; Flags: replacesameversion
Source: "{#SourcePath}tell.wav";                  DestDir: "{app}"; Components: x14Pluginsx86/TellDetect x14Pluginsx64/TellDetect

[Icons]
Name: "{group}\{#AppName}";                       Filename: "{app}/{#AppExeName}"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}";               Filename: "{app}/{#AppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}/{#AppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(AppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent; Parameters: "/firstrun"

[InstallDelete]
Type: files; Name: "{app}/WindowerConfig.exe";
Type: files; Name: "{app}/Windowerx14.dll";
Type: files; Name: "{app}/x14-2core.dll";
Type: files; Name: "{app}/plugins/*.dll";

[CustomMessages]
win_sp_title=Windows %1 Service Pack %2

[Code]
function InitializeSetup(): boolean;
begin
	//init windows version
	initwinversion();  
  vcredist2015();
  
  Result := true;
end;

[InnoIDE_PreCompile]
Name: ""{#SignToolExe}""; Parameters: "{#SignToolCmd} {#SourcePath}\..\Release\Launcher.exe"; Flags: CmdPrompt RunMinimized AbortOnError; 
Name: ""{#SignToolExe}""; Parameters: "{#SignToolCmd} {#SourcePath}\..\Release\bootstrap.dll"; Flags: CmdPrompt RunMinimized AbortOnError; 
Name: ""{#SignToolExe}""; Parameters: "{#SignToolCmd} {#SourcePath}\..\Release\x14-2core.x86.dll"; Flags: CmdPrompt RunMinimized AbortOnError; 
Name: ""{#SignToolExe}""; Parameters: "{#SignToolCmd} {#SourcePath}\..\Release\x14-2core.x64.dll"; Flags: CmdPrompt RunMinimized AbortOnError; 
Name: ""{#SignToolExe}""; Parameters: "{#SignToolCmd} {#SourcePath}\..\Release\plugins\x86\*.dll"; Flags: CmdPrompt RunMinimized AbortOnError; 
Name: ""{#SignToolExe}""; Parameters: "{#SignToolCmd} {#SourcePath}\..\Release\plugins\x64\*.dll"; Flags: CmdPrompt RunMinimized AbortOnError; 

[InnoIDE_PostCompile]
Name: ""{#SignToolExe}""; Parameters: "{#SignToolCmd} {#SourcePath}\x14-setup-*.exe"; Flags: CmdPrompt RunMinimized AbortOnError;
