#include <stdio.h>
#include <stdlib.h>

#include "init.h"
#include "joysticks.h"
#include "console.h"
#include "display.h"

#define INIT_IMPLEMENTATION
#include "init.h"
#define JOYSTICKS_IMPLEMENTATION
#include "joysticks.h"
#define DISPLAY_IMPLEMENTATION
#include "display.h"
#define CONSOLE_IMPLEMENTATION
#include "console.h"

#include "logger.h"
#include "command_line_arguments.h"

#include "SDL2/SDL.h"

static int Main(void){
    Init();
    SetTerminalInputMode();
    printf("\033[?25l"); // Hide cursor

    JoyStickList list = {NULL, 0, 0};
    size_t controller_index = 0;
    int scroll_index = 0;
    SDL_Joystick* current_joystick = NULL;
    struct Size size = GetTerminalSize();

    // Clear screen
    Clear(0, 0, size.w, size.h);


    while (!quit){
        // Get size
        size = GetTerminalSize();

        // Handle add and remove event
        SDL_Event event;
        while (SDL_PollEvent( &event ) != 0){
            if (HandleJoystickEvents(&list, event)){
                LogErrorBacktraceAndReturn(1);
            }
        }

        // Check if any controllers connected
        if (list.length == 0){
            printf("No controllers detected\n\r");
            current_joystick = NULL;
        } else {
            // Get Current joystick
            current_joystick = list.joysticks[controller_index];
        }
        
        // If controller size changes,
        // need to reset to 0
        if (current_joystick != NULL && controller_index >= list.length){
            controller_index = 0;
            scroll_index = 0;
        }
        
        //Goto 0,0
        printf("\033[0;0H");

        // Update graphic
        SDL_JoystickUpdate();
        int lines = 0;
        if (current_joystick){
            lines = UpdateControllerGraphic(controller_index, list.length, current_joystick, scroll_index, size.w, size.h);
        }
        

        // Handle input
        if (kbhit()) {
            int c = getch();

            // Handle quit
            if (c == 'q' || c == 3) quit = true;

            // Handle controller swapping
            else if ('1' <= c && c <= '9' && (size_t) c - '1' < list.length){
                controller_index = c - '1';
                scroll_index = 0;
            }

            // Handle right / next
            else if ((c == '\t' || c == 'd')){
                if (controller_index == list.length - 1) controller_index = 0;
                else controller_index++;
                scroll_index = 0;
            }

            // Handle left / back
            else if (c == 'a'){
                if (controller_index == 0) controller_index = list.length - 1;
                else controller_index--;
                scroll_index = 0;
            }

            // Handle scrolling down
            else if (c == 's'){
                if (scroll_index < GetMaxScroll(current_joystick) - (size.h - lines)){
                    scroll_index++;
                }
            }

            // Handle scrolling up
            else if (c == 'w'){
                if (scroll_index != 0) scroll_index--;
            }

            // Handle rumble
            else if (c == 'r'){
                SDL_JoystickRumble(current_joystick, 0xFFFF, 0xFFFF, 100);
            }

            // Handle rumble triggers
            else if (c == 't'){
                SDL_JoystickRumbleTriggers(current_joystick, 0xFFFF, 0xFFFF, 100);
            }

            // Handle LED
            else if (c == 'z'){
                SDL_JoystickSetLED(current_joystick, 255, 0, 0);
            } else if (c == 'x'){
                SDL_JoystickSetLED(current_joystick, 0, 255, 0);
            } else if (c == 'c'){
                SDL_JoystickSetLED(current_joystick, 0, 0, 255);
            } else if (c == 'n'){
                SDL_JoystickSetLED(current_joystick, 0, 0, 0);
            } else if (c == 'm'){
                SDL_JoystickSetLED(current_joystick, 255, 255, 255);
            }
        }
    }

    //Quit
    printf("\033[?25h"); // Show cursor
    printf("\n\rQuitting...\n\r");
    ResetTerminalInputMode();
    Quit();

    return 0;
}


int main(int argc, char* argv[]){
    CommandLineArgumentRemoveExecutableArgument(argv, argc);

    bool help = false;
    const char* args[argc];

    int n_args = ParseCommandLineArguments(argc, argv, args, ARGS(
        ARG(help, ARG_FLAG, "--help", "-help", "/help", "--h", "-h", "/h", "--?", "-?", "/?", "h", "help"),
    ));
    if (CommandLineArgumentIsError(n_args)){
        int arg = CommandLineArgumentErrorIndex(n_args);
        printf("Error: cannot parse argument %i '%s'", arg, argv[arg]);
        return 1;
    }
    else if (help){
        printf(
            "This application allows testing of controllers from the command line\n\n"
            "Controls:\n"
            "\tq / CTRL+C\n"
            "\tQuit the application\n\n"
            "\t1 ... 9\n"
            "\tSwap the controller index to 1 through to 9\n\n"
            "\tw / s\n"
            "\tScroll vertically\n\n"
            "\ta / tab / d\n"
            "\tScroll horizontally\n\n"
            "\tr\n"
            "\tRumble the current controller\n\n"
            "\tt\n"
            "\tRumble the current controller's triggers\n\n"
            "\tz\n"
            "\tSet the current controller's LED to red\n\n"
            "\tx\n"
            "\tSet the current controller's LED to green\n\n"
            "\tc\n"
            "\tSet the current controller's LED to blue\n\n"
            "\tn\n"
            "\tSet the current controller's LED to black\n\n"
            "\tm\n"
            "\tSet the current controller's LED to white\n\n"
        );
        return 0;
    }
    
    return Main();
}