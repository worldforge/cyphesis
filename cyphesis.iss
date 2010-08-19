; -- Example3.iss --
; Same as Example1.iss, but creates some registry entries too.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=Cyphesis
AppVersion=0.5.23
DefaultDirName={pf}\Cyphesis
DefaultGroupName=WorldForge
UninstallDisplayIcon={app}\cyphesis.exe
; OutputDir=userdocs:Inno Setup Examples Output

[Dirs]
Name: "{app}\var"
Name: "{app}\var\tmp"
Name: "{app}\etc"
Name: "{app}\etc\cyphesis"
Name: "{app}\etc\cyphesis\mason.d"
Name: "{app}\share"
Name: "{app}\share\cyphesis"
Name: "{app}\share\cyphesis\rulesets"
Name: "{app}\share\cyphesis\rulesets\basic"
Name: "{app}\share\cyphesis\rulesets\mason"
Name: "{app}\share\cyphesis\scripts"
Name: "{app}\share\cyphesis\scripts\cyphesis"

[Files]
Source: "server/cyphesis.exe"; DestDir: "{app}"
Source: "client/cyclient.exe"; DestDir: "{app}"
Source: "tools/cyaddrules.exe"; DestDir: "{app}"
Source: "tools/cycmd.exe"; DestDir: "{app}"
Source: "tools/cyconfig.exe"; DestDir: "{app}"
Source: "tools/cyconvertrules.exe"; DestDir: "{app}"
Source: "tools/cydb.exe"; DestDir: "{app}"
Source: "tools/cydumprules.exe"; DestDir: "{app}"
Source: "tools/cyloadrules.exe"; DestDir: "{app}"
Source: "tools/cypasswd.exe"; DestDir: "{app}"
Source: "tools/cypython.exe"; DestDir: "{app}"
Source: "data/cyphesis.vconf"; DestDir: "{app}\etc\cyphesis"
Source: "data/*.xml"; Excludes: "acorn.xml,werewolf.xml"; DestDir: "{app}\etc\cyphesis\mason.d"
; Source: "README"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\Cyphesis"; Filename: "{app}\cyphesis.exe"

[Registry]
; Start "Software\My Company\My Program" keys under HKEY_CURRENT_USER
; and HKEY_LOCAL_MACHINE. The flags tell it to always delete the
; "My Program" keys upon uninstall, and delete the "My Company" keys
; if there is nothing left in them.
Root: HKCU; Subkey: "Software\WorldForge"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "Software\WorldForge\WorldForge"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\WorldForge"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\WorldForge\Cyphesis"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\WorldForge\Cyphesis\Settings"; ValueType: string; ValueName: "Path"; ValueData: "{app}"
