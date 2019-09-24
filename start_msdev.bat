call env.bat

SET PNAME=dos

del Debug\*.* /Q
del Release\*.* /Q
del %PNAME%.ncb

"%MSDevdir%\Bin\MSDev.exe" %PNAME%.dsw

IF EXIST %OUTPUT_PATH%\%PNAME%.exe (
del %PNAME%.exe
copy %OUTPUT_PATH%\%PNAME%.exe %PNAME%.exe
)

del Debug\*.* /Q
del Release\*.* /Q
del %PNAME%.ncb
