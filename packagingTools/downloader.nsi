!define PRODUCT_NAME "DentSlicerDownloader"
!define PRODUCT_VERSION "1.0.0"
!define PRODUCT_PUBLISHER "HiX Inc."
!define PRODUCT_WEB_SITE "http://hix.co.kr"


; MUI 1.67 compatible ------
!include "MUI.nsh"


; MUI Settings
; !define MUI_ABORTWARNING
!define MUI_ICON "..\icon.ico"
; !define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Language Selection Dialog Settings
; !define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
; !define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
; !define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "license.txt"
; Directory page
; !define MUI_DIRECTORYPAGE_TEXT_TOP "${PRODUCT_NAME} will be installed in the following folder.$\r$\n Setting files and user configs will be installed in:   ${HIX_SETTINGS_DIR}"
; !insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
; !define MUI_FINISHPAGE_RUN "$INSTDIR\DentSlicer.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Korean"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
; OutFile "DentSlicerSetup.exe"
; InstallDir "$PROGRAMFILES64\DentSlicer"
; InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
; ShowInstDetails show
; ShowUnInstDetails show

; Function .onInit
;   ExecWait "TaskKill /F /IM DentSlicer.exe"
;   !insertmacro MUI_LANGDLL_DISPLAY 
;   ReadRegStr $0 HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
;   ${If} $0 != ""
;     !insertmacro UninstallExisting $0 $0
;   ${EndIf}
; FunctionEnd



Section "MainSection" SEC01
;   SetOverwrite try
;   SetOutPath "$INSTDIR"

; File /r "setup\*.*"
;   SetOverwrite off
;   SetOutPath "${HIX_SETTINGS_DIR}"
;   File setup\*.json
;   SetOverwrite on
;   SetOutPath "${HIX_SETTINGS_DIR}\PrinterPresets"
;   File setup\PrinterPresets\*.json
;   CreateDirectory "${HIX_SETTINGS_DIR}\LocalSettings"


;   Push "${HIX_SETTINGS_DIR}" 
;   Push "\"
;   Call StrSlash
;   Pop $R0
;   ;Now $R0 contains 'c:/this/and/that/filename.htm'

;   nsJSON::Set /value `{}`
;   nsJSON::Set `version` /value `"${PRODUCT_VERSION}"`
;   nsJSON::Set `settingsDir` /value `"$R0"`
;   nsJSON::Serialize /format /file $INSTDIR\deploy.json


    Delete $TEMP/DentSlicerSetup.exe
    inetc::get https://services.hix.co.kr/setup/get_file/dentslicer/DentSlicerSetup.exe  $TEMP/DentSlicerSetup.exe
    ; Pop $0
    ; StrCmp "$0" "ok" downloadSuccess
    ; downloadSuccess:
    ; nsExec::Exec "$EXEDir/DentSlicerSetup.exe"
    ExecShell "runas" "$TEMP/DentSlicerSetup.exe"

SectionEnd



; Function un.onUninstSuccess
;   HideWindow
;   MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
; FunctionEnd

; Function un.onInit
; !insertmacro MUI_UNGETLANGUAGE
;   MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
;   Abort
; FunctionEnd

; Section Uninstall
;   Delete "$INSTDIR\${PRODUCT_NAME}.url"
;   Delete "$INSTDIR\uninst.exe"
  
;   Delete "$SMPROGRAMS\DentSlicer\Uninstall.lnk"
;   Delete "$SMPROGRAMS\DentSlicer\Website.lnk"
;   Delete "$DESKTOP\DentSlicer.lnk"
;   Delete "$SMPROGRAMS\DentSlicer\DentSlicer.lnk"

;   RMDir "$SMPROGRAMS\DentSlicer"
;   RMDir /r "$INSTDIR"

;   DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
;   DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
;   SetAutoClose true
; SectionEnd

