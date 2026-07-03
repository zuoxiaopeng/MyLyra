@echo off

set UE=E:\UnrealEngine-5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe
set PROJ=E:\UnrealEngine-5.7\Samples\Games\MyLyra\MyLyra.uproject

echo Generating TypeScript declaration files (.d.ts)...

"%UE%" "%PROJ%" /Engine/Maps/Entry ^
-TestExitExecCmds="Puerts.Gen" ^
-unattended ^
-NoCompileShaders ^
-NoLoadStartupPackages ^
-nullrhi ^
-log ^
-nosplash ^
-nosound ^
-nopauseonsuccess

echo TypeScript declaration files generated successfully!
echo Files location: Typing\ue\

pause