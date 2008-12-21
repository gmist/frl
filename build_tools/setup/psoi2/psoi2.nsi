SetCompressor /SOLID lzma

# Included files
!include Sections.nsh
!include MUI.nsh
!include "..\template\frl_macro.nsh"

# Defines
Name "$(^TranslatedName)"
!define out_directory "..\..\..\output\setup"
!define REGKEY "SOFTWARE\Psoi2OPC"
!define COMPANY "Serg Baburin (serg.baburin@gmail.com)"
!define URL "frl.googlecode.com"

# MUI defines
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\box-install.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "..\..\..\doc\frl_logos\logo_text.bmp"
!define MUI_PAGE_DIRECTORY
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER Psoi2OPC
!define MUI_FINISHPAGE_RUN $INSTDIR\server\opc_psoi2.exe
!define MUI_FINISHPAGE_RUN_PARAMETERS
!define MUI_FINISHPAGE_SHOWREADME $INSTDIR\server\readme.rtf
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\box-uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define MUI_LANGDLL_ALWAYSSHOW

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
!insertmacro MUI_LANGUAGE English
!insertmacro MUI_LANGUAGE Russian

# Installer attributes
!insertmacro CREATE_OUT_DIR "${out_directory}"
!insertmacro BUILDING_SVN_OUT_NAME ${out_directory} "psoi2_opc_w_src" "../../../"

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
	File ..\..\..\doc\projects\opc\psoi2\readme.rtf
	Exec "$INSTDIR\server\opc_psoi2.exe -r_silent"
    WriteRegStr HKLM "${REGKEY}\Components" "OPC server" 1
SectionEnd

Section /o "$(^SimulatorName)" SEC0001
    SetOutPath $INSTDIR\simulator
    SetOverwrite on
    File ..\..\..\output\test\psoi2_sender\test_psoi2_sender.exe
    File ..\..\..\output\test\psoi2_sender\config.xml
    WriteRegStr HKLM "${REGKEY}\Components" "$(^SimulatorName)" 1
SectionEnd

Section /o "$(^SourcesName)" SEC0002
    SetOutPath $INSTDIR\src\build_tools
    !system "ruby ../template/get_frl_files_wo_svn.rb -g"
	!include "files.lst"
	!system "ruby ../template/get_frl_files_wo_svn.rb -d"
    WriteRegStr HKLM "${REGKEY}\Components" "$(^SourcesName)" 1
SectionEnd

Section -post SEC0003
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $SMPROGRAMS\$StartMenuGroup
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk" $INSTDIR\uninstall.exe
	CreateShortcut "$SMPROGRAMS\$StartMenuGroup\ReadMe.lnk" $INSTDIR\server\readme.rtf
	CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^ShortTranslatedName).lnk" $INSTDIR\server\opc_psoi2.exe
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
Section /o "-un.$(^SourcesName)" UNSEC0002
    RmDir /r /REBOOTOK $INSTDIR\src\test
    RmDir /r /REBOOTOK $INSTDIR\src\src
    RmDir /r /REBOOTOK $INSTDIR\src\projects
    RmDir /r /REBOOTOK $INSTDIR\src\include
    RmDir /r /REBOOTOK $INSTDIR\src\doc
    RmDir /r /REBOOTOK $INSTDIR\src\dependency
    RmDir /r /REBOOTOK $INSTDIR\src\build_tools
    DeleteRegValue HKLM "${REGKEY}\Components" "$(^SourcesName)"
SectionEnd

Section /o "-un.$(^SimulatorName)" UNSEC0001
    Delete /REBOOTOK $INSTDIR\simulator\config.xml
    Delete /REBOOTOK $INSTDIR\simulator\test_psoi2_sender.exe
	RmDir /r /REBOOTOK $INSTDIR\simulator
    DeleteRegValue HKLM "${REGKEY}\Components" "$(^SimulatorName)"
SectionEnd

Section /o "-un.OPC server" UNSEC0000
	ExecWait "$INSTDIR\server\opc_psoi2.exe -u_silent"
    Delete /REBOOTOK $INSTDIR\server\config.xml
    Delete /REBOOTOK $INSTDIR\server\opc_psoi2.exe
	Delete /REBOOTOK $INSTDIR\server\readme.rtf

	Delete /REBOOTOK $INSTDIR\server\*.log
	Delete /REBOOTOK $APPDATA\Psoi2OPC\*.log

    DeleteRegValue HKLM "${REGKEY}\Components" "OPC server"
SectionEnd

Section -un.post UNSEC0003
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"

    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\ReadMe.lnk"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\$(^ShortTranslatedName).lnk"

    Delete /REBOOTOK $INSTDIR\uninstall.exe

    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"

    RmDir /r /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RmDir /r /REBOOTOK $INSTDIR
	RmDir /r /REBOOTOK $APPDATA\Psoi2OPC
    Push $R0
    StrCpy $R0 $StartMenuGroup 1
    StrCmp $R0 ">" no_smgroup
no_smgroup:
    Pop $R0
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir
	!insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

# Uninstaller functions
Function un.onInit
	!insertmacro MUI_LANGDLL_DISPLAY
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION "OPC server" ${UNSEC0000}
    !insertmacro SELECT_UNSECTION "$(^SimulatorName)" ${UNSEC0001}
    !insertmacro SELECT_UNSECTION "$(^SourcesName)" ${UNSEC0002}
FunctionEnd

# Installer Language Strings
# TODO Update the Language Strings with the appropriate translations.

LangString ^TranslatedName ${LANG_RUSSIAN} "OPC сервер для газоанализатора ПСОИ-02"
LangString ^TranslatedName ${LANG_ENGLISH} "OPC server for PSOI-02 gas analyzer"

LangString ^ShortTranslatedName ${LANG_RUSSIAN} "OPC сервер"
LangString ^ShortTranslatedName ${LANG_ENGLISH} "OPC server"

LangString ^UninstallLink ${LANG_RUSSIAN} "Деисталировать $(^Name)"
LangString ^UninstallLink ${LANG_ENGLISH} "Uninstall $(^Name)"

LangString ^SimulatorName ${LANG_RUSSIAN} "Имитатор ПСОИ-02"
LangString ^SimulatorName ${LANG_ENGLISH} "PSOI-02 simulator"

LangString ^SourcesName ${LANG_RUSSIAN} "Исходные тексты (Toolkit)"
LangString ^SourcesName ${LANG_ENGLISH} "Sources (Toolkit)"
