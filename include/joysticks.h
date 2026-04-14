#pragma once

#include "SDL2/sdl.h"
#include "logger.h"

#include <stdbool.h>
#include <stdlib.h>

typedef struct JoyStickList{
    SDL_Joystick** joysticks;
    size_t length;
    size_t size;
} JoyStickList;

int AddJoystick(JoyStickList*, SDL_Joystick*);
int RemoveJoystick(JoyStickList*, SDL_Joystick*);
int HandleJoystickEvents(JoyStickList*, SDL_Event);

#ifdef JOYSTICK_IMPLEMENTATION

#define INITAL_SIZE 8

/*
* Adds a joystick to a list
*
* Parameters:
*   list: JoyStickList*
*       The list to add the joystick to
*
*   joystick: JoyStickList* 
*       The joystick
*
* Returns:
*   0 on success
*/
int AddJoystick(JoyStickList* list, SDL_Joystick* joystick){
    // Null check
    LogAssertNull(list);
    LogAssertNull(joystick);
    if (list == NULL || list == NULL){ return 1; }

    // Check for empty list
    if (list->joysticks == NULL){
        list->joysticks = malloc(INITAL_SIZE * sizeof(SDL_Joystick*));
        if (list->joysticks == NULL){
            LogError("Cannot malloc array");
            return 1;
        }
        list->size = INITAL_SIZE;
        list->length = 0;

        LogDebug("Created list: %p(%i)", list->joysticks, list->size);
    }
    
    // Check if list is full
    else if (list->size == list->length){
        list->size *= 2;
        list->joysticks = realloc(list->joysticks, list->size * sizeof(SDL_Joystick*));
        if (list->joysticks == NULL){
            LogError("Cannot realloc array");
            return 1;
        }

        LogDebug("Resized list: %p(%i/%i)", list->joysticks, list->length, list->size);
    }

    // Add element
    list->joysticks[list->length++] = joystick;

    return 0;
}

/*
* Removes a joystick from a list
*
* Parameters:
*   list: JoyStickList*
*       The list to add the joystick to
*
*   joystick: JoyStickList* 
*       The joystick
*
* Returns:
*   0 on success
*/
int RemoveJoystick(JoyStickList* list, SDL_Joystick* joystick){
    // Null check
    LogAssertNull(list);
    LogAssertNull(joystick);
    if (list == NULL || list == NULL){ return 1; }
    
    // Check empty list
    if (list->joysticks == NULL || list->size == 0){
        return 0;
    }

    // Find element
    size_t index = (size_t) -1;
    for (size_t i = 0; i < list->length; i++){
        if (list->joysticks[i] == joystick){
            index = i;
            break;
        }
    }

    // Check if we found anything
    if (index == (size_t) -1){
        return 0;
    }

    // Shift elements back
    list->length -= 1;
    for (size_t i = index; i < list->length; i++){
        list->joysticks[i] = list->joysticks[i + 1];
    }

    return 0;
}


/*
* Handes add and remove events for joysticks
*
* Parameters:
*   list: JoyStickList*
*       The list to add the joystick to
*
*   event: SDL_Event
*       The event
*
* Returns:
*   0 on success
*/
int HandleJoystickEvents(JoyStickList* list, SDL_Event event){
    // Controller was added
    if (event.type == SDL_JOYDEVICEREMOVED){
        SDL_Joystick* joystick;
        if ((joystick = SDL_JoystickFromInstanceID(event.jdevice.which)) == NULL){
            LogError("Cannot get JoystickFromInstanceID: %s", SDL_GetError());
            return 1;
        }

        RemoveJoystick(list, joystick);
        SDL_JoystickClose(joystick);
        LogDebug("Removed joystick: %p", joystick);
    }

    // Controller was removed
    else if (event.type == SDL_JOYDEVICEADDED){
        SDL_Joystick* joystick;
        if ((joystick = SDL_JoystickOpen(event.jdevice.which)) == NULL){
            LogError("Cannot Open JoyStick %i: %s", event.jdevice.which, SDL_GetError());
            return 1;
        }

        AddJoystick(list, joystick);
        LogDebug("Added joystick: %p", joystick);
    }

    return 0;
}


#endif // JOYSTICK_IMPLEMENTATION
