@echo off
gcc spotifind.c tdas\list.c tdas\extra.c tdas\hashmap.c -o spotifind
spotifind.exe

pause