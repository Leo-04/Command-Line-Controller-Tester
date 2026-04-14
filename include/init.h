#pragma once

#include "SDL2/sdl.h"
#include "logger.h"

#include <signal.h>
#include <stdbool.h>

int Init(void);
void Quit(void);

extern volatile bool quit;

#ifdef INIT_IMPLEMENTATION

volatile bool quit = 0;

/* Handles CTRL-C */
static void SignalHandler(int unused) {
    quit = true;
}

/*
* Initlaises SDL aswell as SIGINT handler
*
* Returns:
*   0 on success
*/
int Init(void){
    if (!SDL_SetHintWithPriority(SDL_HINT_JOYSTICK_THREAD, "1", SDL_HINT_OVERRIDE)){
        LogWarning("Cannot set SDL_HINT_JOYSTICK_THREAD: %s", SDL_GetError());
    }
    if (!SDL_SetHintWithPriority(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1", SDL_HINT_OVERRIDE)){
        LogWarning("Cannot set SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS: %s", SDL_GetError());
    }

    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK)){
        LogError("Cannot init SDL: %s", SDL_GetError());
        return 1;
    }

    int state = SDL_JoystickEventState(SDL_ENABLE);
    if (state < 0){
        LogError("SDL_JoystickEventState error: %s", SDL_GetError());
        return 1;
    } else if (state == 0){
        LogWarning("Cannot set Joystick Event State");
    }

    if (signal(SIGINT, SignalHandler) == SIG_ERR){
        LogWarning("Cannot hook SIGINT");
    }

    return 0;
}

/*
* Quits SDL
*/
void Quit(void){
    SDL_Quit();
}

#endif // INIT_IMPLEMENTATION