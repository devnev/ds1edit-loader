REM SET OLDPATH=%PATH%
PATH=C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Perl\bin\;D:\GnuWin32\bin;C:\Qt\4.2.1\bin;C:\mingw\bin;D:\Common Files\GTK\2.0\bin
qmake
CD build
qmake
CD ..
mingw32-make
IF ERRORLEVEL 1 (
	REM PATH=%OLDPATH%
	GOTO ERROR
)
REM PATH=%OLDPATH%

SET BINDIST=dist\bindist
SET SRCDIST=dist\srcdist
DEL /Q /F dist
MKDIR dist

MKDIR %BINDIST%
COPY "bin\ds1edit_loader.exe" %BINDIST%
COPY COPYING %BINDIST%
REM COPY res\loader_default.ini %BINDIST%\ds1edit_loader.ini
COPY maplist.txt %BINDIST%
COPY "readmefirst!!!.html" %BINDIST%
COPY pp_1_1.jpg %BINDIST%
CD %BINDIST%
D:\7-Zip\7z a ..\ds1edit_loader-bin-nodll.7z *
D:\GnuWin32\bin\zip -9 -r ..\ds1edit_loader-bin-nodll.zip *
CD ..\..
XCOPY dlls\* %BINDIST%
CD %BINDIST%
D:\7-Zip\7z a ..\ds1edit_loader-bin.7z *
D:\GnuWin32\bin\zip -9 -m -r ..\ds1edit_loader-bin.zip *
CD ..\..
RMDIR %BINDIST%

MKDIR %SRCDIST%
COPY ds1editloader.pro %SRCDIST%
COPY "ds1edit loader.cbp" %SRCDIST%
COPY "ds1edit loader.layout" %SRCDIST%
COPY doxygen.cfg %SRCDIST%
REM COPY autogen.sh %SRCDIST%
COPY COPYING %SRCDIST%
REM COPY res\loader_default.ini %SRCDIST%\ds1edit_loader.ini
COPY ds1editloader.qrc %SRCDIST%
COPY ds1editloader.rc %SRCDIST%
COPY *.ui %SRCDIST%
COPY maplist.txt %SRCDIST%
COPY "readmefirst!!!.html" %SRCDIST%
MKDIR %SRCDIST%\src
XCOPY src\*.h %SRCDIST%\src
XCOPY src\*.cpp %SRCDIST%\src
REM XCOPY src\src.pro %SRCDIST%\src
REM DEL %SRCDIST%\src\moc_*
REM DEL %SRCDIST%\src\qrc_*
MKDIR %SRCDIST%\res
COPY "res\ds1 files.ico" %SRCDIST%\res
COPY "res\ds1edit loader.png" %SRCDIST%\res
COPY "res\ds1edit loader.ico" %SRCDIST%\res
COPY res\ds1edit_default.ini %SRCDIST%\res
COPY res\loader_default.ini %SRCDIST%\res
MKDIR %SRCDIST%\build
COPY build\build.pro %SRCDIST%\build
CD %SRCDIST%
D:\7-Zip\7z a ..\ds1edit_loader-src.7z *
D:\GnuWin32\bin\zip -9 -m -r ..\ds1edit_loader-src.zip *
CD ..\..
RMDIR %SRCDIST%

CD dist
COPY "..\readmefirst!!!.html" .
COPY ..\pp_1_1.jpg .
CD ..

goto DONE

:ERROR
pause
:DONE
