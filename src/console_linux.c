#include "../console.h"

#include <stdarg.h>

int colors = 1;

void con_init()
{
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    attron(A_BOLD);
}

void con_deinit()
{
    endwin();
}

int con_gotoXY(int x, int y)
{
    return move(y, x);
}

int con_getXY(int *px, int *py)
{
    return getyx(stdscr, *py, *px);
}

int con_getMaxXY(int *px, int *py)
{
    return getmaxyx(stdscr, *py, *px);
}

int con_clearScr()
{
    clear();
    return refresh();
}

int con_outTxt(const char *format, ...)
{
    va_list arglist;
    char buffer[1024];
    int len;

    va_start(arglist, format);

    len = vsnprintf(buffer, sizeof(buffer)-1, format, arglist);
    len = (printw(buffer) != ERR) ? len : -1;
    refresh();
    return len;
}

int con_keyPressed()
{
    int ch = getch();
    if (ch != ERR)
    {
        ungetch(ch);
        return 1;
    }
    return 0;
}

int con_getKey()
{
    return getch();
}

int con_initPair(short n, short fg, short bg)
{
    return init_pair(n, fg, bg);
}

int con_setColor(short n)
{
    return attron(COLOR_PAIR(n));
}

