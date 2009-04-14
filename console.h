#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#ifdef WIN32
#include <conio.h>

#define CON_KEY_UP          -72
#define CON_KEY_LEFT        -75
#define CON_KEY_RIGHT       -77
#define CON_KEY_DOWN        -80
#define CON_KEY_DEL         -83
#define CON_KEY_ESCAPE      27
#define CON_KEY_BACKSPACE   '\b'

#else
#include <ncurses.h>

#define CON_KEY_UP          KEY_UP
#define CON_KEY_DOWN        KEY_DOWN
#define CON_KEY_LEFT        KEY_LEFT
#define CON_KEY_RIGHT       KEY_RIGHT
#define CON_KEY_ESCAPE      27
#define CON_KEY_DEL         KEY_DC
#define CON_KEY_BACKSPACE   KEY_BACKSPACE

#endif

extern void     con_init();
extern void     con_deinit();

extern int      con_gotoXY(int x, int y);
extern int      con_getXY(int *px, int *py);
extern int      con_getMaxXY(int *px, int *py);
extern int      con_clearScr();

extern int      con_outTxt(const char *format, ...);

extern int      con_keyPressed();
extern int      con_getKey();

#endif
