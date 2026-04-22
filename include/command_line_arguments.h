#pragma once

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

enum CommandLineArgumentType{
    //Flag
    ARG_FLAG = 0x00,

    // Values
    ARG_STRING = 0x1,
    ARG_BOOL,
    ARG_LONG,
    ARG_DOUBLE,

    // Greedy values
    ARG_EQ_STRING = 0xF1,
    ARG_EQ_BOOL,
    ARG_EQ_LONG,
    ARG_EQ_DOUBLE,

    // Masks
    ARG_FLAG_TYPE = 0x0F,
    ARG_FLAG_EQ = 0xF0,
};

typedef struct CommandLineArgument{
    void* value;
    enum CommandLineArgumentType type;
    size_t length;
    const char** names; // Array of values (cannot be NULL)
} CommandLineArgument;

#define CommandLineArgumentRemoveExecutableArgument(argv, argc) argv++; argc--;
#define CommandLineArgumentErrorIndex(arg) (-(arg + 1))
#define CommandLineArgumentIsError(arg) (arg < 0)

/*
* Checks if a argument matches with a CommandLineArgument
*
* Parameters:
*   arg: CommandLineArgument
*       The CommandLineArgument
*
*   argv_arg: const char*
*       The agrument from argv
*
* Returns:
*   A match:  Non zero
*       if arg.type & ARG_FLAG_EQ then the length of the name string
*       else 1
*   No match: 0
*/
static int MatchName(CommandLineArgument arg, const char* argv_arg){
    int match = 0;
    for (size_t name = 0; name < arg.length; name++){
        const char* name_string = arg.names[name];
        if (arg.type & ARG_FLAG_EQ && strncmp(argv_arg, name_string, strlen(name_string))==0){
            match = strlen(name_string);
            break;
        } else if (strcmp(argv_arg, name_string)==0){
            match = 1;
            break;
        }
    }

    return match;
}

/*
* Parses a value from the command line into
*  the value of a CommandLineArgument
*
* Parameters:
*   arg: CommandLineArgument
*       The CommandLineArgument
*
*   value: const char*
*       The value to parse though
*
* Returns:
*   0 on success
*/
static int ParseValue(CommandLineArgument arg, char* value){
    switch (arg.type & ARG_FLAG_TYPE){
        // Copy string
        case ARG_STRING:
            // We know argv is valid for whole session so can just copy ptr
            *((const char**) arg.value) = value;
        break;
        
        // Parse boolean
        case ARG_BOOL:{
            for (char* p = value; *p; ++p) *p = tolower(*p);
            bool is_false = 
                strcmp(value, "false")==0
                || strcmp(value, "no")==0
                || strcmp(value, "off")==0
                || strcmp(value, "n")==0
                || strcmp(value, "f")==0
                || strcmp(value, "0")==0
            ;

            bool is_true = 
                strcmp(value, "true")==0
                || strcmp(value, "yes")==0
                || strcmp(value, "on")==0
                || strcmp(value, "y")==0
                || strcmp(value, "t")==0
                || strcmp(value, "1")==0
            ;

            if (!is_true && !is_false){
                // Invalid boolean
                return 1;
            }

            *((bool*) arg.value) = is_true;
        } break;

        // Parse long
        case ARG_LONG: {
            char *end;
            long number = strtol(value, &end, 10);
            if (end == value || *end != '\0'){
                // Invalid long
                return 1;
            }
            *((long*) arg.value) = number;
        } break;

        // Parse double
        case ARG_DOUBLE:{
            char *end;
            double number = strtod(value, &end);
            if (end == value || *end != '\0'){
                // Invalid double
                return 1;
            }
            *((double*) arg.value) = number;
        } break;
    }

    return 0;
}

/*
* Parses though argv and puts values in the passed CommandLineArgument array
*  Will also fill the rest array with values not put in the array
*
* Parameters:
*   argc: int
*       The number of command line arguments
*
*   argv: const char*[argc]
*       The command line arguments
*
*   rest: const char*[argc]
*       The output buffer for agruments that are not put in the array
*
*   n_args: int
*       The number of elements in CommandLineArgument array
*
*   args: CommandLineArgument[n_args]
*       The CommandLineArgument array
*
* Returns:
*   The number of arguments in rest
*
*   Check `CommandLineArgumentIsError` for any errors
*       Use `CommandLineArgumentErrorIndex` to get the index of argv that the error occured
*/
static inline int ParseCommandLineArguments(int argc, char* argv[argc], const char* rest[argc], int n_args, CommandLineArgument args[n_args]){
    int arg_rest = 0;
    for (int arg = 0; arg < argc; arg++){
        bool not_arg = true;
        for (int i = 0; i < n_args; i++){
            int match = MatchName(args[i], argv[arg]);

            // If we have not found an argument match skip value
            if (!match) continue;

            // Stop seaching we have found the argument
            not_arg = false;

            // Check if a flag (non value agrument)
            if (args[i].type == ARG_FLAG || args[i].type == ARG_FLAG_EQ){
                *((bool*) args[i].value) = true;
                break;
            }

            // Get the string that will contain the value
            char* value;
            if (args[i].type & ARG_FLAG_EQ){
                value = argv[arg] + match;
            } else if (arg + 1 < argc){
                value = argv[arg + 1];
                arg++;
            } else {
                // Invalid
                return CommandLineArgumentErrorIndex(arg);
            }
            
            // Parse value
            if (ParseValue(args[i], value)){
                // Error parsing
                return CommandLineArgumentErrorIndex(arg);
            }

            break;
        }

        // if the argument was not an arg, put it in the rest
        if (not_arg){
            rest[arg_rest] = argv[arg];
            arg_rest++;
        }
    }

    return arg_rest;
}

#define ARGS(...) sizeof((CommandLineArgument[]) {__VA_ARGS__})/sizeof(CommandLineArgument), (CommandLineArgument[]) {__VA_ARGS__}
#define ARG(variable, present, ...) (CommandLineArgument){&variable, present, sizeof((const char*[]){__VA_ARGS__})/sizeof(const char*), (const char*[]){__VA_ARGS__}}
