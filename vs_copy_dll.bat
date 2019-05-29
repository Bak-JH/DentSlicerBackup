xcopy /s/Y %~dp0\utils\winsparkle\WinSparkle.dll %~dp0\%1
xcopy /s/Y %~dp0\utils\winsparkle\WinSparkle.lib %~dp0\%1
xcopy /s/Y %~dp0\utils\winsparkle\WinSparkle.pdb %~dp0\%1

xcopy /s/Y %CGAL_DIR%\auxiliary\gmp\lib\libgmp-10.lib %~dp0\%1
xcopy /s/Y %CGAL_DIR%\auxiliary\gmp\lib\libgmp-10.dll %~dp0\%1

IF "%1"=="Debug" (
	xcopy /s/Y %CGAL_DIR%\build\lib\%1\CGAL-vc140-mt-gd-4.14.lib %~dp0\%1
	xcopy /s/Y %CGAL_DIR%\build\bin\%1\CGAL-vc140-mt-gd-4.14.dll %~dp0\%1
	xcopy /s/Y %CGAL_DIR%\build\bin\%1\CGAL-vc140-mt-gd-4.14.pdb %~dp0\%1
) ELSE (
	xcopy /s/Y %CGAL_DIR%\build\lib\%1\CGAL-vc140-mt-4.14.lib %~dp0\%1
	xcopy /s/Y %CGAL_DIR%\build\bin\%1\CGAL-vc140-mt-4.14.dll %~dp0\%1
	xcopy /s/Y %CGAL_DIR%\build\bin\%1\CGAL-vc140-mt-4.14.pdb %~dp0\%1
)






