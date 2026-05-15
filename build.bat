@echo off
setlocal enabledelayedexpansion

echo Building Fob kernel...

REM 如果 fob.elf 存在且无法删除（被占用），提示关闭 QEMU
if exist fob.elf (
    del /f /q fob.elf 2>nul
    if exist fob.elf (
        echo ERROR: fob.elf is locked. Please close QEMU and try again.
        pause
        exit /b 1
    )
)

REM 编译 boot/boot.s
i686-elf-as boot/boot.s -o boot/boot.o
if errorlevel 1 goto error

REM 自动编译 kernel 目录下所有 .c 文件，输出 .o 到 kernel 目录
for %%f in (kernel\*.c) do (
    echo Compiling %%f...
    i686-elf-gcc -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I. -Ikernel -c %%f -o kernel\%%~nf.o
    if errorlevel 1 goto error
)

REM 收集所有的 .o 文件
set OBJS=boot\boot.o
for %%f in (kernel\*.o) do (
    set OBJS=!OBJS! %%f
)

REM 链接
echo Linking...
i686-elf-gcc -T linker.ld -ffreestanding -O2 -nostdlib -lgcc -o fob.elf %OBJS%
if errorlevel 1 goto error

REM 链接成功，删除所有 .o 文件（强制删除，忽略错误）
echo Cleaning object files...
del /f /q boot\boot.o 2>nul
del /f /q kernel\*.o 2>nul

echo Build successful! Run 'qemu-system-x86_64 -kernel fob.elf'
goto end

:error
echo Build failed.
pause
exit /b 1

:end
pause