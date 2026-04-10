@echo off
SET /A "ip = 127.0.0.1"
SET /A "port = 23500"
SET /A "index = 1"
SET /A "count = 25"

CD ./x64/

:while
if %index% leq %count% (
        START /MIN Client.exe --ip=%ip% --port=%port%
    SET /A index = %index% + 1
    @echo %index%
    goto :while
)
