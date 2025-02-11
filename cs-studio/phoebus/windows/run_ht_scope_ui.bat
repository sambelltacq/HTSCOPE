@echo off

set SCRIPT_DIR=%~dp0
for %%A in ("%SCRIPT_DIR%\..") do set "PARENT_DIR=%%~fA"

set "PHOEBUS_JAR=C:\Users\sam\Downloads\phoebus-win\phoebus-4.7.4-SNAPSHOT\product-4.7.4-SNAPSHOT.jar"
set "LAUNCHER=%PARENT_DIR%\CSS\ht_scope_launcher.bob"
set "SETTINGS=%SCRIPT_DIR%settings.ini"

java -Dfile.encoding=UTF-8 -jar %PHOEBUS_JAR% -nosplash -settings "%SETTINGS%" -resource "%LAUNCHER%" -layout null