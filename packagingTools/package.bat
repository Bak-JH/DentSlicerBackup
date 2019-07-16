ECHO off
ECHO Welcome to DSP: DentSlicer Packager
ECHO DSP: running windeployqt
%DS_COMPILER_DIR%\windeployqt.exe --dir %DS_PACKAGE_INPUT% --compiler-runtime --release --qmldir %DS_PACKAGE_SOURCE%\Qml %DS_RELEASE_BUILD%\DentSlicer.exe
ECHO running vs_copy_dll for copying dependent libraries
start vs_copy_dll_mingw.bat release %DS_PACKAGE_INPUT% %DS_PACKAGE_SOURCE%
ECHO copy DentSlicer.exe
xcopy /s/Y %DS_RELEASE_BUILD%\DentSlicer.exe %DS_PACKAGE_INPUT%
