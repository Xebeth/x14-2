@echo off
if not exist "F:\Final Fantasy XIV\plugins" mkdir "F:\Final Fantasy XIV\plugins"
copy "%1plugins\*.dll" "F:\Final Fantasy XIV\plugins"
if exist "%1WindowerX14.dll" copy "%1WindowerX14.dll" "F:\Final Fantasy XIV\"