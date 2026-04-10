@echo off
@REM SET /A "ip = 127.0.0.1"
@REM SET /A "port = 23500"
SET /A "index = 1"
SET /A "count = 100"

CD ./x64/

:while
@echo %time%
     :spawnloop
     if %index% leq %count% (
          START /MIN Client.exe
          SET /A index = %index% + 1
          @echo %index%
          goto :spawnloop
          )
     timeout 250 > NUL
     SET /A index = 1
     goto :while
