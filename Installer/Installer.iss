#define AppName "x14-2"
#define SourcePath "..\Release\"
#define AppPublisher "North Edge"
#define AppVersion GetFileVersion("..\Release\x14-2core.dll")
#define AppURL "http://x14.north-edge.com/x14-2"
#define AppExeName "Launcher.exe"

#define use_vc2013

#include "scripts\products.iss"

#include "scripts\products\stringversion.iss"
#include "scripts\products\winversion.iss"
#include "scripts\products\fileversion.iss"
#include "scripts\products\vcredist2013.iss"

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
; SignTool=signtool sign /f E:\~dev\cpp\x14-2\certificates\north-edge-os.pfx /p 7a6iI26*wn^&t0ET /t http://timestamp.verisign.com/scripts/timstamp.dll E:\~dev\cpp\x14-2\Release\*.*

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
Name: "BootstrapPlugins/AutoLogin";   Description: "AutoLogin {#GetFileVersion("..\Release\Plugins\AutoLogin.dll")} — Auto fill the login form";          Types: full custom
Name: "x14Plugins";                   Description: "{#AppName} Plugins"; Types: full custom
Name: "x14Plugins/AutoBlacklist";     Description: "AutoBlacklist {#GetFileVersion("..\Release\Plugins\AutoBlacklist.dll")} — Blacklist spammers automatically";  Types: full custom
Name: "x14Plugins/Distance";          Description: "Distance {#GetFileVersion("..\Release\Plugins\Distance.dll")} — Distance from player to target";              Types: full custom
Name: "x14Plugins/ChatLog";           Description: "ChatLog {#GetFileVersion("..\Release\Plugins\ChatLog.dll")} — Log the game chat to a file";                   Types: full custom
Name: "x14Plugins/ExpWatch";          Description: "ExpWatch {#GetFileVersion("..\Release\Plugins\ExpWatch.dll")} — Keep track of experience earned";             Types: full custom
Name: "x14Plugins/TellDetect";        Description: "TellDetect {#GetFileVersion("..\Release\Plugins\TellDetect.dll")} — Play a custom sound on tell";             Types: full custom
Name: "x14Plugins/Timestamp";         Description: "Timestamp {#GetFileVersion("..\Release\Plugins\Timestamp.dll")} — Add a timestamp to the game chat";          Types: full custom

[Files]
Source: "{#SourcePath}Plugins\AutoLogin.dll";     DestDir: "{app}\plugins"; Components: BootstrapPlugins/AutoLogin; Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\AutoBlacklist.dll"; DestDir: "{app}\plugins"; Components: x14Plugins/AutoBlacklist;   Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\ChatLog.dll";       DestDir: "{app}\plugins"; Components: x14Plugins/ChatLog;         Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\Distance.dll";      DestDir: "{app}\plugins"; Components: x14Plugins/Distance;        Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\ExpWatch.dll";      DestDir: "{app}\plugins"; Components: x14Plugins/ExpWatch;        Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\TellDetect.dll";    DestDir: "{app}\plugins"; Components: x14Plugins/TellDetect;      Flags: replacesameversion skipifsourcedoesntexist
Source: "{#SourcePath}Plugins\Timestamp.dll";     DestDir: "{app}/plugins"; Components: x14Plugins/Timestamp;       Flags: replacesameversion skipifsourcedoesntexist

Source: "{#SourcePath}x14-2core.dll";             DestDir: "{app}"; Components: x14Base; Flags: replacesameversion
Source: "{#SourcePath}bootstrap.dll";             DestDir: "{app}"; Components: x14Base; Flags: replacesameversion
Source: "{#SourcePath}Launcher.exe";              DestDir: "{app}"; Components: x14Base; Flags: replacesameversion
Source: "{#SourcePath}tell.wav";                  DestDir: "{app}"; Components: x14Plugins/TellDetect       

[Icons]
Name: "{group}\{#AppName}";                       Filename: "{app}/{#AppExeName}"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}";               Filename: "{app}/{#AppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}/{#AppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(AppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent; Parameters: "/firstrun"

[InstallDelete]
Type: files; Name: "{app}/WindowerConfig.exe";
Type: files; Name: "{app}/Windowerx14.dll";

[CustomMessages]
win_sp_title=Windows %1 Service Pack %2

[Code]
function InitializeSetup(): boolean;
begin
	//init windows version
	initwinversion();  
  vcredist2013();
  
  Result := true;
end;

[InnoIDE_PreCompile]
Name: """C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin\signtool.exe"""; Parameters: "sign /f E:\~dev\cpp\x14-2\certificates\north-edge-os.pfx /p 7a6iI26*wn&t0ET /t http://timestamp.verisign.com/scripts/timstamp.dll E:\~dev\cpp\x14-2\Release\Launcher.exe"; Flags: CmdPrompt RunMinimized AbortOnError; 
Name: """C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin\signtool.exe"""; Parameters: "sign /f E:\~dev\cpp\x14-2\certificates\north-edge-os.pfx /p 7a6iI26*wn&t0ET /t http://timestamp.verisign.com/scripts/timstamp.dll E:\~dev\cpp\x14-2\Release\bootstrap.dll"; Flags: CmdPrompt RunMinimized AbortOnError; 
Name: """C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin\signtool.exe"""; Parameters: "sign /f E:\~dev\cpp\x14-2\certificates\north-edge-os.pfx /p 7a6iI26*wn&t0ET /t http://timestamp.verisign.com/scripts/timstamp.dll E:\~dev\cpp\x14-2\Release\x14-2core.dll"; Flags: CmdPrompt RunMinimized AbortOnError; 
Name: """C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin\signtool.exe"""; Parameters: "sign /f E:\~dev\cpp\x14-2\certificates\north-edge-os.pfx /p 7a6iI26*wn&t0ET /t http://timestamp.verisign.com/scripts/timstamp.dll E:\~dev\cpp\x14-2\Release\plugins\*.dll"; Flags: CmdPrompt RunMinimized AbortOnError; 

[InnoIDE_PostCompile]
Name: """C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin\signtool.exe"""; Parameters: "sign /f E:\~dev\cpp\x14-2\certificates\north-edge-os.pfx /p 7a6iI26*wn&t0ET /t http://timestamp.verisign.com/scripts/timstamp.dll E:\~dev\cpp\x14-2\Installer\*.exe"; Flags: CmdPrompt RunMinimized AbortOnError; 
