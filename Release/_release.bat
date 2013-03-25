@echo off

if not exist windowerx14-2 md windowerx14-2

copy /y WindowerConfig.exe windowerx14-2
copy /y Launcher.exe windowerx14-2
copy /y windowerx14.dll windowerx14-2
copy /y bootstrap.dll windowerx14-2
copy /y tell.wav windowerx14-2

if not exist windowerx14-2\plugins md windowerx14-2\plugins

copy /y plugins\*.dll windowerx14-2\plugins

@pause