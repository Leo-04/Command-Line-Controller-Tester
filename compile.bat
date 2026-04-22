@echo off
cls

set "gcc=d:\\apps\\mingw64\\bin\\gcc.exe"
set "warning_options=-Wall -pedantic-errors -Wunused-result -Wextra -Werror -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wno-unused-parameter"

set bitmode=64
set "include_paths=-Iinclude -Isrc -Isrc/scenes"
set "libs=-Llib -lmingw32 -lSDL2main -lSDL2"
set "options=-fdiagnostics-color=always -mwindows -mconsole -fmax-errors=1 -Wfatal-errors -Wl,--gc-sections"

set "defines=-D_"

set "files=*.c"
set "output=out/main.exe"

cls && %gcc% -g -fdata-sections -ffunction-sections -m%bitmode% %defines% %warning_options% %include_paths% %files% -o %output% %libs% %options%
