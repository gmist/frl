Name "OPC server for PSOI2 device"

# Defines
!define REGKEY "SOFTWARE\$(^Name)"
!define COMPANY "Serg Baburin (serg.baburin@gmail.com)"
!define URL "frl.googlecode.com"

# MUI defines
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\box-install.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER Psoi2OPC
!define MUI_FINISHPAGE_RUN $INSTDIR\server\opc_psoi2.exe
!define MUI_FINISHPAGE_RUN_PARAMETERS
!define MUI_FINISHPAGE_SHOWREADME $INSTDIR\help\readme.odt
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\box-uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include Sections.nsh
!include MUI.nsh

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE Russian
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile psoi2_setup.exe
InstallDir $PROGRAMFILES\Psoi2OPC
CRCCheck on
XPStyle on
ShowInstDetails show
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show

# Installer sections
Section "-OPC server" SEC0000
    SetOutPath $INSTDIR\server
    SetOverwrite on
    File ..\..\..\output\projects\opc\psoi2\opc_psoi2.exe
    File ..\..\..\output\projects\opc\psoi2\config.xml
	Exec "$INSTDIR\server\opc_psoi2.exe -r_silent"
    WriteRegStr HKLM "${REGKEY}\Components" "OPC server" 1
SectionEnd

Section /o ReadMe SEC0001
    SetOutPath $INSTDIR\help
    SetOverwrite on
    File ..\..\..\doc\projects\opc\psoi2\readme.odt
    WriteRegStr HKLM "${REGKEY}\Components" ReadMe 1
SectionEnd

Section /o "Psoi2 simulator" SEC0002
    SetOutPath $INSTDIR\simulator
    SetOverwrite on
    File ..\..\..\output\test\psoi2_sender\test_psoi2_sender.exe
    File ..\..\..\output\test\psoi2_sender\config.xml
    WriteRegStr HKLM "${REGKEY}\Components" "Psoi2 simulator" 1
SectionEnd

Section /o Sources SEC0003
    SetOutPath $INSTDIR\src\build_tools
    SetOverwrite on
    File /r ..\..\*
    SetOutPath $INSTDIR\src\dependency
    File /r ..\..\..\dependency\*
    SetOutPath $INSTDIR\src\doc
    File /r ..\..\..\doc\*
    SetOutPath $INSTDIR\src\include
    File /r ..\..\..\include\*
    SetOutPath $INSTDIR\src\projects
    File /r ..\..\..\projects\*
    SetOutPath $INSTDIR\src\src
    File /r ..\..\..\src\*
    SetOutPath $INSTDIR\src\test
    File /r ..\..\..\test\*
    WriteRegStr HKLM "${REGKEY}\Components" Sources 1
SectionEnd

Section -post SEC0004
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $SMPROGRAMS\$StartMenuGroup
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk" $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o -un.Sources UNSEC0003
    RmDir /r /REBOOTOK $INSTDIR\src\test
    RmDir /r /REBOOTOK $INSTDIR\src\src
    RmDir /r /REBOOTOK $INSTDIR\src\projects
    RmDir /r /REBOOTOK $INSTDIR\src\include
    RmDir /r /REBOOTOK $INSTDIR\src\doc
    RmDir /r /REBOOTOK $INSTDIR\src\dependency
    RmDir /r /REBOOTOK $INSTDIR\src\build_tools
    DeleteRegValue HKLM "${REGKEY}\Components" Sources
SectionEnd

Section /o "-un.Psoi2 simulator" UNSEC0002
    Delete /REBOOTOK $INSTDIR\simulator\config.xml
    Delete /REBOOTOK $INSTDIR\simulator\test_psoi2_sender.exe
    DeleteRegValue HKLM "${REGKEY}\Components" "Psoi2 simulator"
SectionEnd

Section /o -un.ReadMe UNSEC0001
    Delete /REBOOTOK $INSTDIR\help\readme.odt
    DeleteRegValue HKLM "${REGKEY}\Components" ReadMe
SectionEnd

Section /o "-un.OPC server" UNSEC0000
	ExecWait "$INSTDIR\server\opc_psoi2.exe -u_silent"
    Delete /REBOOTOK $INSTDIR\server\config.xml
    Delete /REBOOTOK $INSTDIR\server\opc_psoi2.exe
	Delete /REBOOTOK $INSTDIR\server\*.log
    DeleteRegValue HKLM "${REGKEY}\Components" "OPC server"
SectionEnd

Section -un.post UNSEC0004
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RmDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RmDir /REBOOTOK $INSTDIR
    Push $R0
    StrCpy $R0 $StartMenuGroup 1
    StrCmp $R0 ">" no_smgroup
no_smgroup:
    Pop $R0
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir
FunctionEnd

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION "OPC server" ${UNSEC0000}
    !insertmacro SELECT_UNSECTION ReadMe ${UNSEC0001}
    !insertmacro SELECT_UNSECTION "Psoi2 simulator" ${UNSEC0002}
    !insertmacro SELECT_UNSECTION Sources ${UNSEC0003}
FunctionEnd

# Installer Language Strings
# TODO Update the Language Strings with the appropriate translations.

LangString ^UninstallLink ${LANG_RUSSIAN} "Uninstall $(^Name)"
LangString ^UninstallLink ${LANG_ENGLISH} "Uninstall $(^Name)"
