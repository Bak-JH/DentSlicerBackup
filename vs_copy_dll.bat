xcopy /s/Y %3\utils\winsparkle\WinSparkle.dll %2
xcopy /s/Y %3\utils\winsparkle\WinSparkle.lib %2
xcopy /s/Y %3\utils\winsparkle\WinSparkle.pdb %2

xcopy /s/Y %CGAL_DIR%\auxiliary\gmp\lib\libgmp-10.lib %2
xcopy /s/Y %CGAL_DIR%\auxiliary\gmp\lib\libgmp-10.dll %2

IF "%1"=="Debug" (
	xcopy /s/Y %CGAL_DIR%\build\lib\%1\CGAL-vc140-mt-gd-4.14.lib %2
	xcopy /s/Y %CGAL_DIR%\build\bin\%1\CGAL-vc140-mt-gd-4.14.dll %2
	xcopy /s/Y %CGAL_DIR%\build\bin\%1\CGAL-vc140-mt-gd-4.14.pdb %2
) ELSE (
	xcopy /s/Y %CGAL_DIR%\build\lib\%1\CGAL-vc140-mt-4.14.lib %2
	xcopy /s/Y %CGAL_DIR%\build\bin\%1\CGAL-vc140-mt-4.14.dll %2
)






