@echo off
if not exist "J:\Final Fantasy XIV\plugins" mkdir "J:\Final Fantasy XIV\plugins"
copy "%1plugins\*.dll" "J:\Final Fantasy XIV\plugins"
if exist "%1WindowerX14.dll" copy "%1WindowerX14.dll" "J:\Final Fantasy XIV\"