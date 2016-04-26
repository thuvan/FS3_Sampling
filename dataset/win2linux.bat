ECHO OFF
IF "%~1"=="" GOTO help
IF "%~2"=="" GOTO help

ECHO BEGIN CONVERTING...
ECHO SOURCE FILE:  %1
ECHO DEST FILE:  %2

TR "\r" "\n" < %1 > %2

ECHO FINISHED. PRESS ENTER TO EXIT.
PAUSE
GOTO end

:help
ECHO Wrong parameters.
ECHO %0  <source_file>  <dest_file>
PAUSE

:end
