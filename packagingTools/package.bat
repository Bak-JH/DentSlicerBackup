ECHO off
ECHO Welcome to DSP: DentSlicer Packager
del "DSBuild.nsi" >nul 2>&1
del "DentSlicerSetup.exe" >nul 2>&1

ECHO DSP: running windeployqt
mkdir setup

%QT_MSVC2017_64%\bin\windeployqt.exe --dir setup --compiler-runtime --release --qmldir ..\Qml ..\release\DentSlicer.exe
ECHO running vs_copy_dll for copying dependent libraries
start ..\vs_copy_dll.bat release setup ..\
ECHO copy DentSlicer.exe
xcopy /s/Y ..\release\DentSlicer.exe setup
ECHO DSP: create NSI build script
start DSNSISBuilder.exe config.json
ECHO DSP: create setup.exe
start makensis.exe DSBuild.nsi
