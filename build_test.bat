@echo off
set GCC=D:\MSYS2\ucrt64\bin\gcc.exe
%GCC% -std=c11 -Wall -Wextra -I Component\Algorithm ^
      -o TEST\pid_test.exe TEST\test_pid.c Component\Algorithm\pid.c -lm
if errorlevel 1 ( echo BUILD FAILED & exit /b 1 )
TEST\pid_test.exe
echo exit=%errorlevel%