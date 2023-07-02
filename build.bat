@echo off

set COMMON_FLAGS=/Od /DUNICODE /W4 /Z7 /EHsc /wd4996 /nologo /MT
set BUILD_FLAGS=%COMMON_FLAGS%  /link /CETCOMPAT User32.lib Gdi32.lib
cl main.cpp /Felol_auto_accept.exe %BUILD_FLAGS% 


del *.ilk
del *.obj