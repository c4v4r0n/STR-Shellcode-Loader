@echo off

del loader.exe
rc.exe resources\strings.rc
cvtres /MACHINE:x64 /OUT:resources.o resources\strings.res
cl.exe /nologo /Ox /MT /W0 /GS- /DNDEBUG /TC main.c /link /OUT:loader.exe /SUBSYSTEM:CONSOLE /MACHINE:x64 resources.o

del resources\*.res
del *.obj
del *.o