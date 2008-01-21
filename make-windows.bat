SET OLDPATH=%PATH%
PATH=C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Perl\bin\;D:\GnuWin32\bin;C:\Qt\4.2.1\bin;C:\mingw\bin;D:\Common Files\GTK\2.0\bin
qmake
CD build
qmake
CD ..
mingw32-make
COPY bin\ds1edit_loader.exe .
PATH=%OLDPATH%
