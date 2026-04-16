@echo off
SET /A "ip = 10.192.75.217"
SET /A "port = 9000"
SET /A "dir = ../../Client/data/"
SET /A "index = 1"
SET /A "count = 100"

CD ./x64/Release

:while
if %index% leq %count% (
        START /MIN Client.exe
    SET /A index = %index% + 1
    @echo %index%
    goto :while
)
