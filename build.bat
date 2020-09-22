@echo off

:: ███    ███  █████  ██ ███    ██
:: ████  ████ ██   ██ ██ ████   ██
:: ██ ████ ██ ███████ ██ ██ ██  ██
:: ██  ██  ██ ██   ██ ██ ██  ██ ██
:: ██      ██ ██   ██ ██ ██   ████

setlocal EnableDelayedExpansion

set config=%1

if "%config%"=="" (
        set config=debug
)

:: C compiler (for mingw32-make)
set "CC=gcc"

::

:: Action[n].CMD -- Command to execute.
:: Action[n].OK  -- OK value of '%ERRORLEVEL%'.

set "Action[0].CMD=premake5 gmake2"
set "Action[0].OK=1"
set "Action[1].CMD=mingw32-make config=%config%"
set "Action[1].OK=0"

call :struct_array_last_element Action CMD
set "ActionLast=%ERRORLEVEL%"

::

set /a "TotalActions=%ActionLast%+1"
echo Total actions: %TotalActions%

for /L %%i IN (0, 1, %ActionLast%) DO (
        call :print "Executing action %%i.."
        call :execute_action %%i
        if NOT %ERRORLEVEL% == !Action[%%i].OK! (
                call :print ". -- ERROR (%ERRORLEVEL%)"
                echo.
                call :handle_error %%i
                goto exit
        ) else (
                call :print ". -- OK (%ERRORLEVEL%)"
                echo.
        )
)

:exit

del build_out.log >nul 2>nul
del build_err.log >nul 2>nul

exit /b 0

:: ███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████
:: ██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██
:: █████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████
:: ██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
:: ██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████

:execute_action& rem [ActionIndex]
        set "ActionIndex=%~1"
        call !Action[%ActionIndex%].CMD! 1>build_out.log 2>build_err.log
exit /b %ERRORLEVEL%

:handle_error& rem [ActionIndex]
        echo.
        echo Failed command: !Action[%~1].CMD!
        echo.
        echo.
        echo  --- [STDOUT] ---
        echo.
        type build_out.log
        echo.
        echo  --- [ END  ] ---
        echo.
        echo.
        echo  --- [STDERR] ---
        echo.
        type build_err.log
        echo.
        echo  --- [ END  ] ---
        echo.
        echo.
        echo Press any key to terminate this script and clear logs...
        pause >nul
exit /b 0

:print& rem [Message]
        :: Same as echo, but prints without newline.
        echo|set /p"=%~1"
exit /b 0

:array_last_element& rem [Array]
        set __last=0
        :array_last_element_loop
                if NOT "!%~1[%__last%]!"=="" (
                        set /a __last+=1
                        goto array_last_element_loop
                )
        set /a __last-=1
exit /b %__last%

:struct_array_last_element& rem [Array]& rem [AnyMember]
        set __last=0
        :struct_array_last_element_loop
                if NOT "!%~1[%__last%].%~2!"=="" (
                        set /a __last+=1
                        goto struct_array_last_element_loop
                )
        set /a __last-=1
exit /b %__last%