@echo off
SETLOCAL enabledelayedexpansion enableextensions
cd /d %~dp0

set "length=19"
SET Program="C:\Program Files\MiKTeX 2.9\miktex\bin\x64\epstopdf.exe"

for %%A in (*.eps) do (


set "curDate=%%~tA"
set "trimpath=!curDate:~0,%length%!"

if exist %%~nA.lastmodified (


FOR /F "eol=; tokens=* delims=, " %%i in (%%~nA.lastmodified) do (set "curDate=%%i")

set "lastDate=!curDate:~0,%length%!"

if not "!trimpath!"=="!lastDate!"  ( 
ECHO %%~nA
%Program% --outfile=%%~nA.pdf %%~nA.eps
ECHO %%~tA > %%~nA.lastmodified
)

) else (
ECHO %%~nA
%Program% --outfile=%%~nA.pdf %%~nA.eps
ECHO %%~tA > %%~nA.lastmodified
)



)


