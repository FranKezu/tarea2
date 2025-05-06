@echo off
chcp 65001 >nul
gcc tarea2.c tdas\list.c tdas\map.c tdas\extra.c -o tarea2
tarea2.exe

pause