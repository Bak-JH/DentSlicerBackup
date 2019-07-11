xcopy /s/Y utils\winsparkle\WinSparkle.dll %1
xcopy /s/Y utils\winsparkle\WinSparkle.lib %1
xcopy /s/Y utils\winsparkle\WinSparkle.pdb %1

xcopy /s/Y %CGAL_DIR%\auxiliary\gmp\lib\libgmp-10.lib %1
xcopy /s/Y %CGAL_DIR%\auxiliary\gmp\lib\libgmp-10.dll %1

IF "%1"=="Debug" (
	xcopy /s/Y %CGAL_DIR%\build\lib\%1\CGAL-vc140-mt-gd-4.14.lib %1
	xcopy /s/Y %CGAL_DIR%\build\bin\%1\CGAL-vc140-mt-gd-4.14.dll %1
	xcopy /s/Y %CGAL_DIR%\build\bin\%1\CGAL-vc140-mt-gd-4.14.pdb %1
) ELSE (
	xcopy /s/Y %CGAL_DIR%\build\lib\%1\CGAL-vc140-mt-4.14.lib %1
	xcopy /s/Y %CGAL_DIR%\build\bin\%1\CGAL-vc140-mt-4.14.dll %1
)






