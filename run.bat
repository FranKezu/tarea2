@echo off
$OutputEncoding = [Console]::OutputEncoding = [Text.UTF8Encoding]::new()
gcc spotifind.c tdas\list.c tdas\extra.c tdas\hashmap.c -o spotifind
spotifind.exe

pause