#define AppName "x14-2"
#define SourcePath "..\Release\"
#define AppPublisher "North Edge"
#define AppVersion GetFileVersion("..\Release\Windowerx14.dll")
#define AppURL "http://x14.north-edge.com/x14-2"
#define AppExeName "WindowerConfig.exe"

#define use_vc2010

#include "scripts\products.iss"

#include "scripts\products\stringversion.iss"
#include "scripts\products\winversion.iss"
#include "scripts\products\fileversion.iss"
#include "scripts\products\vcredist2010.iss"

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
;InfoBeforeFile=before-install.txt
;InfoAfterFile=after-install.txt
WizardSmallImageFile=x14-logo.bmp
WizardImageFile=x14-banner.bmp
OutputDir=.\
OutputBaseFilename=x14-setup-{#AppVersion}
SetupIconFile=..\Common\x14.ico
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
Name: "Windower";                     Description: "x14-2 base files"; Types: full compact custom; Flags: fixed
Name: "BootstrapPlugins";             Description: "Bootstrap plugins"; Types: full custom
Name: "BootstrapPlugins/AutoLogin";   Description: "AutoLogin {#GetFileVersion("..\Release\Plugins\AutoLogin.dll")} — Auto fill the login form";          Types: full custom
Name: "WindowerPlugins";              Description: "Plugins"; Types: full custom
Name: "WindowerPlugins/Distance";     Description: "Distance {#GetFileVersion("..\Release\Plugins\Distance.dll")} — Distance from player to target";      Types: full custom
Name: "WindowerPlugins/ChatLog";      Description: "ChatLog {#GetFileVersion("..\Release\Plugins\ChatLog.dll")} — Log the game chat to a file";           Types: full custom
Name: "WindowerPlugins/ExpWatch";     Description: "ExpWatch {#GetFileVersion("..\Release\Plugins\ExpWatch.dll")} — Keep track of experience earned";     Types: full custom
Name: "WindowerPlugins/TellDetect";   Description: "TellDetect {#GetFileVersion("..\Release\Plugins\TellDetect.dll")} — Play a custom sound on tell";     Types: full custom
Name: "WindowerPlugins/Timestamp";    Description: "Timestamp {#GetFileVersion("..\Release\Plugins\Timestamp.dll")} — Add a timestamp to the game chat";  Types: full custom

[Files]
Source: "{#SourcePath}Plugins\AutoLogin.dll";   DestDir: "{app}\plugins"; Components: BootstrapPlugins/AutoLogin; Flags: replacesameversion
Source: "{#SourcePath}Plugins\ChatLog.dll";     DestDir: "{app}\plugins"; Components: WindowerPlugins/ChatLog;    Flags: replacesameversion
Source: "{#SourcePath}Plugins\Distance.dll";    DestDir: "{app}\plugins"; Components: WindowerPlugins/Distance;   Flags: replacesameversion
Source: "{#SourcePath}Plugins\ExpWatch.dll";    DestDir: "{app}\plugins"; Components: WindowerPlugins/ExpWatch;   Flags: replacesameversion
Source: "{#SourcePath}Plugins\TellDetect.dll";  DestDir: "{app}\plugins"; Components: WindowerPlugins/TellDetect; Flags: replacesameversion
Source: "{#SourcePath}Plugins\Timestamp.dll";   DestDir: "{app}/plugins"; Components: WindowerPlugins/Timestamp;  Flags: replacesameversion

Source: "{#SourcePath}WindowerConfig.exe";  DestDir: "{app}"; Components: Windower; Flags: replacesameversion
Source: "{#SourcePath}Windowerx14.dll";     DestDir: "{app}"; Components: Windower; Flags: replacesameversion
Source: "{#SourcePath}bootstrap.dll";       DestDir: "{app}"; Components: Windower; Flags: replacesameversion
Source: "{#SourcePath}Launcher.exe";        DestDir: "{app}"; Components: Windower; Flags: replacesameversion
Source: "{#SourcePath}tell.wav";            DestDir: "{app}"; Components: WindowerPlugins/TellDetect

[Icons]
Name: "{group}\{#AppName}";                       Filename: "{app}/{#AppExeName}"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}";               Filename: "{app}/{#AppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}/{#AppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(AppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent; Parameters: "/firstrun"

[UninstallDelete]
Type: files; Name: "{app}/config.ini"

[CustomMessages]
win_sp_title=Windows %1 Service Pack %2

[Code]
function InitializeSetup(): boolean;
begin
	//init windows version
	initwinversion();  
  vcredist2010();
  
  Result := true;
end;
