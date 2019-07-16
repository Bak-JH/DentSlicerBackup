xcopy /s/Y %3\utils\winsparkle\WinSparkle.dll %2
xcopy /s/Y %3\utils\winsparkle\WinSparkle.lib %2
xcopy /s/Y %3\utils\winsparkle\WinSparkle.pdb %2

xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\auxiliary\gmp\lib\libgmp-10.lib %2
xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\auxiliary\gmp\lib\libgmp-10.dll %2
xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\auxiliary\gmp\lib\libmpfr-4.lib %2
xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\auxiliary\gmp\lib\libmpfr-4.dll %2

IF "%1"=="Debug" (
	xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\build\bin\libCGAL.dll %2
	xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\build\bin\libCGAL_Core.dll %2
	xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\build\bin\libCGAL_ImageIO.dll %2
	xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\build\bin\libCGAL_Qt5.dll %2
) ELSE (
	xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\build\bin\libCGAL.dll %2
	xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\build\bin\libCGAL_Core.dll %2
	xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\build\bin\libCGAL_ImageIO.dll %2
	xcopy /s/Y %CGAL_DIR_MINGW_COMPILED%\build\bin\libCGAL_Qt5.dll %2
)






