#define MyAppName "ThreeFingerDrag"
#define MyAppVersion "1.3.0"
#define MyAppExeName "ThreeFingerDrag.exe"

[Setup]
#ifdef TestInstall
Uninstallable=no
CreateUninstallRegKey=no
#endif
AppId={{1D896AD4-D9F6-4A5F-9F96-69B369E0A015}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher=Cake-Herio
AppPublisherURL=https://github.com/Cake-Herio/ThreeFingerDrag
DefaultDirName={autopf}\{#MyAppName}
DisableDirPage=no
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
LicenseFile=LICENSE
OutputDir=installers
OutputBaseFilename=ThreeFingerDrag.Setup.{#MyAppVersion}
SetupIconFile=ThreeFingerDrag\ThreeFingerDrag.ico
UninstallDisplayIcon={app}\{#MyAppExeName}
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
CloseApplications=yes
RestartApplications=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; Flags: unchecked
Name: "startup"; Description: "Start ThreeFingerDrag when I sign in to Windows"; Flags: unchecked

[Files]
Source: "build\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "packaging\config.ini"; DestDir: "{app}"; Permissions: users-modify; Flags: onlyifdoesntexist uninsneveruninstall
Source: "packaging\log.txt"; DestDir: "{app}"; Permissions: users-modify; Flags: onlyifdoesntexist uninsneveruninstall
Source: "packaging\version.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "INSTALL.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "LICENSE"; DestDir: "{app}"; Flags: ignoreversion

#ifndef TestInstall
[Icons]
Name: "{autoprograms}\ThreeFingerDrag"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"
Name: "{autodesktop}\ThreeFingerDrag"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; Tasks: desktopicon

[Registry]
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "ThreeFingerDrag"; ValueData: """{app}\{#MyAppExeName}"""; Tasks: startup

[Run]
Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; Description: "Launch ThreeFingerDrag"; Flags: nowait postinstall skipifsilent runasoriginaluser
#endif

[Code]
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  Command: String;
begin
  if CurUninstallStep = usUninstall then
  begin
    if RegQueryStringValue(HKCU, 'Software\Microsoft\Windows\CurrentVersion\Run',
      'ThreeFingerDrag', Command) then
      if CompareText(Command, '"' + ExpandConstant('{app}\{#MyAppExeName}') + '"') = 0 then
        RegDeleteValue(HKCU, 'Software\Microsoft\Windows\CurrentVersion\Run', 'ThreeFingerDrag');
  end;
end;
