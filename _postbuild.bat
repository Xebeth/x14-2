@echo off
if not exist "J:\Final Fantasy XIV\boot\plugins" mkdir "J:\Final Fantasy XIV\boot\plugins"
copy "%1plugins\*.dll" "J:\Final Fantasy XIV\boot\plugins"
if exist "%1WindowerX14.dll" copy "%1WindowerX14.dll" "J:\Final Fantasy XIV\boot\"