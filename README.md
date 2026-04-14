# Command Line Controller Tester

This is a simple command line app for testing controllers using the SDL2 joystick API

## Build

### Linux
For linux, SDL2 libaries must be installed
Build using `bash ./compile.sh`

### Windows
Build using `compile`
The file `SDL2.dll` must be present at run-time for the application to work.

## Controls

`q / CTRL+C`
    Quit the application

`1` ... `9`
    Swap the controller index to 1 through to 9

`w` / `s`
    Scroll vertically

`a` / `TAB` / `d`
    Scroll horizontally

`r`
    Rumble the current controller

`t`
    Rumble the current controller's triggers

`z`
    Set the current controller's LED to red

`x`
    Set the current controller's LED to green

`c`
    Set the current controller's LED to blue

`n`
    Set the current controller's LED to black

`m`
    Set the current controller's LED to white