ECHO off
ECHO Welcome to DSP: DentSlicer Packager
del /F /Q setup 2>nul
del "DSBuild.nsi" >nul 2>&1
del "DentSlicerSetup.exe" >nul 2>&1

ECHO DSP: running windeployqt
mkdir setup

%QT_MSVC2017_64%\bin\windeployqt.exe --dir setup --compiler-runtime --release --qmldir ..\Qml ..\release\DentSlicer.exe
ECHO running vs_copy_dll for copying dependent libraries
START /wait "cmd" CMD /c ..\vs_copy_dll.bat release setup ..\
START /wait "cmd" CMD /c ..\vs_copy_bonjour.bat release setup ..\
START /wait "cmd" CMD /c ..\vs_copy_resources.bat release setup ..\
START /wait "cmd" CMD /c copy_pcl.bat
START /wait "cmd" CMD /c copy_redist.bat

ECHO copy DentSlicer.exe
xcopy /s/Y ..\release\DentSlicer.exe setup
xcopy /s/Y ..\release\wincorkDLL.dll setup
xcopy /s/Y ..\release\glfw3.dll setup

ECHO copy CrashReporter.exe
xcopy /s/Y ..\release\CrashReporter.exe setup

ECHO DSP: create NSI build script
start /WAIT /B  DSNSISBuilder.exe config.json
ECHO DSP: create setup.exe
start /WAIT /B makensis.exe DSBuild.nsi
ECHO Packaging done...
exit
