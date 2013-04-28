#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include <wtypes.h>
#include <wincon.h>
#include <locale.h>

#include "console.h"

#ifdef WIN32

short color_attrs[CON_MAX_COLORS]; // - like ncurses color pairs

#endif //WIN32

void con_init()
{
//    CONSOLE_CURSOR_INFO ci = {1, FALSE};
//    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
//    int i =SetConsoleCursorInfo(hStdOut, &ci);
    setlocale( LC_ALL, ".OCP" );
}

void con_deinit()
{
    ;
}

int con_gotoXY(int x, int y)
{
    COORD pos = { x, y };

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    return SetConsoleCursorPosition(hStdOut, pos);
}

int con_getXY(int *px, int *py)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
    {
        *px = csbi.dwCursorPosition.X;
        *py = csbi.dwCursorPosition.Y;
        return 1;
    }
    return 0; // error
}

int con_getMaxXY(int *px, int *py)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
    {
        *px = csbi.srWindow.Right - csbi.srWindow.Left;
        *py = csbi.srWindow.Bottom - csbi.srWindow.Top;
        return 1;
    }
    return 0; // error
}

int con_clearScr()
{
    int mx, my;

    if (con_getMaxXY(&mx, &my))
    {
        COORD szBuf = {mx + 1, my + 1};
        COORD BufferCoord= {0, 0};
        SMALL_RECT srWriteRegion = {0, 0, mx, my};
        int nResult;

        CHAR_INFO *lpBuffer = (CHAR_INFO *)calloc(szBuf.X * szBuf.Y, sizeof(CHAR_INFO));

        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        nResult = WriteConsoleOutput(hStdOut, lpBuffer, szBuf, BufferCoord, &srWriteRegion);
        free(lpBuffer);

        return nResult;
    }
    return 0; // error
}

int con_outTxt(const char * format , ...)
{
    va_list arglist;
    char buffer[1024];
    int len;

    va_start(arglist, format);

    assert(format);
    len = _vsnprintf(buffer, sizeof(buffer)-1, format, arglist);
    assert(len >= 0);

    return _cputs(buffer) == 0 ? len : -1;
}

int con_keyPressed()
{
    return _kbhit();
}

int con_getKey()
{
    int ch = _getch();
    if (ch == 0x00 || ch == 0xE0) // extended code
        return - _getch();
    else
        return ch;
}

int con_initPair(short n, short fg, short bg)
{
    if (n < 0 || n > CON_MAX_COLORS)
        return 0;
    color_attrs[n] = fg | FOREGROUND_INTENSITY | (bg << 4);
    return 1;
}

int con_setColor(short n)
{
    HANDLE hStdOut;
    if (n < 0 || n > CON_MAX_COLORS)
        return 0;
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    return SetConsoleTextAttribute(hStdOut, color_attrs[n]);
}

