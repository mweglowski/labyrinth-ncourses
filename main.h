#include <stdio.h>
#include <locale.h>
#include <iostream>
#ifdef _WIN32
#include <ncursesw/curses.h>
#else
#include <curses.h>
#endif

int inputloop(void);
int handlemovement(int y, int x);
int drawmap(char *filename);
int winlvl(void);
int confcolors(void);
int startgame(void);

int x = 1;
int y = 1;
WINDOW *win;
int currentlvl = 0;

char *levels[] = {
    "levels/lvl0.txt"
};
