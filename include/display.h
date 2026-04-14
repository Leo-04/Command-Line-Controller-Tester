#include <stdio.h>
#include <stdlib.h>

#include "console.h"

#include "SDL2/SDL.h"

int GetMaxScroll(SDL_Joystick*);
int UpdateControllerGraphic(size_t, size_t, SDL_Joystick*, int, int, int);

#ifdef DISPLAY_IMPLEMENTATION

/*
* Gets the maximum amount of lines needed to
*  display all information about a joystick
*
* Parameters:
*   joystick: JoyStickList* 
*       The joystick
*
* Returns:
*   The maximum between:
*       SDL_JoystickNumAxes,
*       SDL_JoystickNumButtons,
*       and SDL_JoystickNumHats
*/
int GetMaxScroll(SDL_Joystick* joystick){
    int value, max = SDL_JoystickNumAxes(joystick);
    if ((value = SDL_JoystickNumButtons(joystick)) > max) max = value;
    if ((value = SDL_JoystickNumHats(joystick)) > max) max = value;

    return max;
}


/*
* Prints a grapic about a given joystick
*
* Parameters:
*   controller_index: size_t
*       The index of the current controller
*
*   num_loaded_controllers: size_t
*       The number of loaded controllers
*
*   joystick: JoyStickList* 
*       The joystick
*
*   scroll_index: int
*       The number of lines scrolled down
*
*   height: int
*       The number of lines allowed to be displayed (must be >= `HEADER_LINES`)
*
* Returns:
*   Height of header
*/
int UpdateControllerGraphic(size_t controller_index, size_t num_loaded_controllers, SDL_Joystick* joystick, int scroll_index, int width, int height){
    //Check for NULL
    LogAssertNull(joystick);
    
    int lines = (104 / width) + 1;
    // Output basic information
    SDL_JoystickPowerLevel level = SDL_JoystickCurrentPowerLevel(joystick);
    printf("Controller %03"fULONG"/%03"fULONG", Player index: %03i, ", controller_index + 1, num_loaded_controllers, SDL_JoystickGetPlayerIndex(joystick));
    printf( 
        "LED: %5s, Rumble: %5s, RumbleTriggers: %5s, Power: %s",
        SDL_JoystickHasLED(joystick)? "TRUE": "FALSE",
        SDL_JoystickHasRumble(joystick)? "TRUE": "FALSE",
        SDL_JoystickHasRumbleTriggers(joystick)? "TRUE": "FALSE",
        (level == SDL_JOYSTICK_POWER_WIRED)? "[Wired]":
            (level == SDL_JOYSTICK_POWER_FULL)? "[=====]":
            (level == SDL_JOYSTICK_POWER_MEDIUM)? "[  ===]":
            (level == SDL_JOYSTICK_POWER_LOW)? "[   ==]":
            (level == SDL_JOYSTICK_POWER_EMPTY)? "[   =]": "UNKNOWN"
    );
    printf("%-*s", width - (104 % width) + 1, "");
    const char* name = SDL_JoystickName(joystick);
    lines += (strlen(name) / width) + 1;
    printf("%-*s", width, name);

    // Output table information
    int scroll = GetMaxScroll(joystick);
    for (int i = 0; i < scroll && i + lines < height; i++){
        int index = i + scroll_index;

        // Print new line
        printf("\n\r");

        // Check if we should print "..."
        if (
            (scroll_index != 0 && i == 0)
            || (i + lines == height - 1 && index+1 < scroll)
        ){
            printf("%-15s|", "      ...");
            printf("%-21s|", "         ...");
            printf("%-17s|", "       ...");
            continue;
        }

        // Print button
        if (SDL_JoystickNumButtons(joystick) > index)
            printf(" Button %03i: %c |", index, SDL_JoystickGetButton(joystick, index)? 'X': '-');
        else printf("%15s|", "");
        
        // Print Axis
        if (SDL_JoystickNumAxes(joystick) > index)
            printf(" Axis %03i: %9lf |", index, SDL_JoystickGetAxis(joystick, index) / 32768.0);
        else printf("%21s|", "");
        
        // Print Hat
        if (SDL_JoystickNumHats(joystick) > index){
            int hat = SDL_JoystickGetHat(joystick, index);
            printf(" Hat %03i: [%c%c%c%c] |",
                index, 
                (hat & SDL_HAT_UP)? '^': '-',
                (hat & SDL_HAT_DOWN)? 'v': '-',
                (hat & SDL_HAT_LEFT)? '<': '-',
                (hat & SDL_HAT_RIGHT)? '>': '-'
            );
        }
        else printf("%17s|", "");

        printf("%*s", width - 56, "");
    }

    fflush(stdout);

    return lines;
}

#endif // DISPLAY_IMPLEMENTATION