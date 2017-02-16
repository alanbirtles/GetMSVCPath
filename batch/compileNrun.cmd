@echo off
for /f %%I in ('cmd /c dir /b /s %windir%\Microsoft.NET\Framework\csc.exe') DO SET CSC=%%I
%CSC% /out:%~dp0\VSConfig.exe %~dp0..\cs\VSConfig.cs > nul
%~dp0\VSConfig.exe