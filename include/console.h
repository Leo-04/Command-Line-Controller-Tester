#ifndef INCLUDE_CONSOLE_H
#define INCLUDE_CONSOLE_H


// See: https://stackoverflow.com/questions/448944/c-non-blocking-keyboard-input/448982#448982
// Added windows suport

// See: https://stackoverflow.com/questions/23369503/get-size-of-terminal-window-rows-columns#23370070
// For GetTerminalSize

void Clear(int x, int y, int w, int h);
struct Size{int w, h;} GetTerminalSize(void);

void ResetTerminalInputMode(void);
void SetTerminalInputMode(void);
int kbhit(void);
int getch(void);


#endif // INCLUDE_CONSOLE_H
#ifdef CONSOLE_IMPLEMENTATION

/*
* Clears a section of the terminal
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
void Clear(int x, int y, int w, int h){
    // Set cursor to x,y
    printf("\033[%i;%iH", x, y);

    // Clear the screen
    for (int i = 0; i < h; i++){
        printf("%*s", w, "");
        if (i != h - 1) printf("\n\r");
    }

    // Set cursor to x,y again
    printf("\033[%i;%iH", x, y);
}

#ifdef __LINUX__

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>

static struct termios orig_termios;

struct Size GetTerminalSize(void){
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return (struct Size){size.ws_col, size.ws_row};
}

void ResetTerminalInputMode(void){
    tcsetattr(0, TCSANOW, &orig_termios);
}

void SetTerminalInputMode(void){
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(ResetTerminalInputMode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit(void){
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

int getch(void){
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

#else // __LINIX__
// #elseif _WIN32

#include <conio.h>

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

struct Size GetTerminalSize(void){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return (struct Size){
        (int)(csbi.srWindow.Right-csbi.srWindow.Left+1),
        (int)(csbi.srWindow.Bottom-csbi.srWindow.Top+1)
    };
}

void ResetTerminalInputMode(void){}
void SetTerminalInputMode(void){}

int kbhit(){
    return _kbhit();
}

int getch(){
    return _getch();
}

#endif // __LINIX__

#endif // CONSOLE_IMPLEMENTATION