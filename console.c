#include "console.h"

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include <wtypes.h>
#include <wincon.h>
#include <locale.h>

#include <..\crt\src\mtdll.h> // ?how to lock console input? -- _mlock / _munlock

typedef
struct
{
	int regular;
	int shift;
	int ctrl;
	int alt;
}
KEY_CODES;

typedef
struct
{
	KEY_CODES    codes;
	unsigned int enh;
}
KEY_CODES_EX;

// Tables of key values for normal & enhanced keys. Inspired by Visual Studio <..\crt\src\getch.c>.

#define KeyCodeNil  0      /* lead char: 0,    next char: 0  => extended code:  0  */
#define NoKeys      KeyCodeNil, KeyCodeNil, KeyCodeNil, KeyCodeNil
#define KeyCode(c)  (c)    /* lead char: c,    next char: 0  => extended code:  c  */
#define KeyCode2(c) (-(c)) /* lead char: 0,    next char: c  => extended code: -c */
#define KeyCodeE(c) (-(c)) /* lead char: 0xE0, next char: c  => extended code: -c */

const static KEY_CODES_EX KeyCodes[] = 
{
	  /* scan code   regular         shift           ctrl            alt                enh */
	{ /* 0x00 */	{ NoKeys                                                       },	0	}, 
	{ /* 0x01 */	{KeyCode(0x1B),  KeyCode(0x1B),  KeyCode(0x1B),  KeyCode2(0x01)},	0	}, 
	{ /* 0x02 */	{KeyCode('1'),   KeyCode('!'),   KeyCodeNil,     KeyCode2(0x78)},	0	}, 
	{ /* 0x03 */	{KeyCode('2'),   KeyCode('@'),   KeyCode2(0x03), KeyCode2(0x79)},	0	}, 
	{ /* 0x04 */	{KeyCode('3'),   KeyCode('#'),   KeyCodeNil,     KeyCode2(0x7A)},	0	}, 
	{ /* 0x05 */	{KeyCode('4'),   KeyCode('$'),   KeyCodeNil,     KeyCode2(0x7B)},	0	}, 
	{ /* 0x06 */	{KeyCode('5'),   KeyCode('%'),   KeyCodeNil,     KeyCode2(0x7C)},	0	}, 
	{ /* 0x07 */	{KeyCode('6'),   KeyCode('^'),   KeyCode(0x1E),  KeyCode2(0x7D)},	0	}, 
	{ /* 0x08 */	{KeyCode('7'),   KeyCode('&'),   KeyCodeNil,     KeyCode2(0x7E)},	0	}, 
	{ /* 0x09 */	{KeyCode('8'),   KeyCode('*'),   KeyCodeNil,     KeyCode2(0x7F)},	0	}, 
	{ /* 0x0A */	{KeyCode('9'),   KeyCode('('),   KeyCodeNil,     KeyCode2(0x80)},	0	}, 
	{ /* 0x0B */	{KeyCode('0'),   KeyCode(')'),   KeyCodeNil,     KeyCode2(0x81)},	0	}, 
	{ /* 0x0C */	{KeyCode('-'),   KeyCode('_'),   KeyCode(0x1F),  KeyCode2(0x82)},	0	}, 
	{ /* 0x0D */	{KeyCode('='),   KeyCode('+'),   KeyCodeNil,     KeyCode2(0x83)},	0	}, 
	{ /* 0x0E */	{KeyCode('\b'),  KeyCode('\b'),  KeyCode(0x7F),  KeyCode2(0x0E)},	0	}, 
	{ /* 0x0F */	{KeyCode('\t'),  KeyCode2(0x0F), KeyCode2(0x94), KeyCode2(0x0F)},	0	}, 
	{ /* 0x10 */	{KeyCode('q'),   KeyCode('Q'),   KeyCode(0x11),  KeyCode2(0x10)},	0	}, 
	{ /* 0x11 */	{KeyCode('w'),   KeyCode('W'),   KeyCode(0x17),  KeyCode2(0x11)},	0	}, 
	{ /* 0x12 */	{KeyCode('e'),   KeyCode('E'),   KeyCode(0x05),  KeyCode2(0x12)},	0	}, 
	{ /* 0x13 */	{KeyCode('r'),   KeyCode('R'),   KeyCode(0x12),  KeyCode2(0x13)},	0	}, 
	{ /* 0x14 */	{KeyCode('t'),   KeyCode('T'),   KeyCode(0x14),  KeyCode2(0x14)},	0	}, 
	{ /* 0x15 */	{KeyCode('y'),   KeyCode('Y'),   KeyCode(0x19),  KeyCode2(0x15)},	0	}, 
	{ /* 0x16 */	{KeyCode('u'),   KeyCode('U'),   KeyCode(0x15),  KeyCode2(0x16)},	0	}, 
	{ /* 0x17 */	{KeyCode('i'),   KeyCode('I'),   KeyCode(0x09),  KeyCode2(0x17)},	0	}, 
	{ /* 0x18 */	{KeyCode('o'),   KeyCode('O'),   KeyCode(0x0F),  KeyCode2(0x18)},	0	}, 
	{ /* 0x19 */	{KeyCode('p'),   KeyCode('P'),   KeyCode(0x10),  KeyCode2(0x19)},	0	}, 
	{ /* 0x1A */	{KeyCode('['),   KeyCode('{'),   KeyCode(0x1B),  KeyCode2(0x1A)},	0	}, 
	{ /* 0x1B */	{KeyCode(']'),   KeyCode('}'),   KeyCode(0x1D),  KeyCode2(0x1B)},	0	}, 
	{ /* 0x1C */	{KeyCode('\r'),  KeyCode('\r'),  KeyCode('\n'),  KeyCode2(0x1C)},	1	}, 
	{ /* 0x1D */	{ NoKeys                                                       },	0	}, 
	{ /* 0x1E */	{KeyCode('a'),   KeyCode('A'),   KeyCode(0x01),  KeyCode2(0x1E)},	0	}, 
	{ /* 0x1F */	{KeyCode('s'),   KeyCode('S'),   KeyCode(0x13),  KeyCode2(0x1F)},	0	}, 
	{ /* 0x20 */	{KeyCode('d'),   KeyCode('D'),   KeyCode(0x04),  KeyCode2(0x20)},	0	}, 
	{ /* 0x21 */	{KeyCode('f'),   KeyCode('F'),   KeyCode(0x06),  KeyCode2(0x21)},	0	}, 
	{ /* 0x22 */	{KeyCode('g'),   KeyCode('G'),   KeyCode(0x07),  KeyCode2(0x22)},	0	}, 
	{ /* 0x23 */	{KeyCode('h'),   KeyCode('H'),   KeyCode(0x08),  KeyCode2(0x23)},	0	}, 
	{ /* 0x24 */	{KeyCode('j'),   KeyCode('J'),   KeyCode(0x0A),  KeyCode2(0x24)},	0	}, 
	{ /* 0x25 */	{KeyCode('k'),   KeyCode('K'),   KeyCode(0x0B),  KeyCode2(0x25)},	0	}, 
	{ /* 0x26 */	{KeyCode('l'),   KeyCode('L'),   KeyCode(0x0C),  KeyCode2(0x26)},	0	}, 
	{ /* 0x27 */	{KeyCode(';'),   KeyCode(':'),   KeyCodeNil,     KeyCode2(0x27)},	0	}, 
	{ /* 0x28 */	{KeyCode('\''),  KeyCode('\"'),  KeyCodeNil,     KeyCode2(0x28)},	0	}, 
	{ /* 0x29 */	{KeyCode('`'),   KeyCode('~'),   KeyCodeNil,     KeyCode2(0x29)},	0	}, 
	{ /* 0x2A */	{ NoKeys                                                       },	0	}, 
	{ /* 0x2B */	{KeyCode('\\'),  KeyCode('|'),   KeyCode(0x1C),  KeyCodeNil    },	0	}, 
	{ /* 0x2C */	{KeyCode('z'),   KeyCode('Z'),   KeyCode(0x1A),  KeyCode2(0x2C)},	0	}, 
	{ /* 0x2D */	{KeyCode('x'),   KeyCode('X'),   KeyCode(0x18),  KeyCode2(0x2D)},	0	}, 
	{ /* 0x2E */	{KeyCode('c'),   KeyCode('C'),   KeyCode(0x03),  KeyCode2(0x2E)},	0	}, 
	{ /* 0x2F */	{KeyCode('v'),   KeyCode('V'),   KeyCode(0x16),  KeyCode2(0x2F)},	0	}, 
	{ /* 0x30 */	{KeyCode('b'),   KeyCode('B'),   KeyCode(0x02),  KeyCode2(0x30)},	0	}, 
	{ /* 0x31 */	{KeyCode('n'),   KeyCode('N'),   KeyCode(0x0E),  KeyCode2(0x31)},	0	}, 
	{ /* 0x32 */	{KeyCode('m'),   KeyCode('M'),   KeyCode(0x0D),  KeyCode2(0x32)},	0	}, 
	{ /* 0x33 */	{KeyCode(','),   KeyCode('<'),   KeyCodeNil,     KeyCode2(0x33)},	0	}, 
	{ /* 0x34 */	{KeyCode('.'),   KeyCode('>'),   KeyCodeNil,     KeyCode2(0x34)},	0	}, 
	{ /* 0x35 */	{KeyCode('/'),   KeyCode('?'),   KeyCodeNil,     KeyCode2(0x35)},	2	}, 
	{ /* 0x36 */	{ NoKeys                                                       },	0	}, 
	{ /* 0x37 */	{KeyCode('*'),   KeyCodeNil,     KeyCode(0x72),  KeyCodeNil    },	0	}, 
	{ /* 0x38 */	{ NoKeys                                                       },	0	}, 
	{ /* 0x39 */	{KeyCode(' '),   KeyCode(' '),   KeyCode(' '),   KeyCode(' ')  },	0	}, 
	{ /* 0x3A */	{ NoKeys                                                       },	0	}, 
	{ /* 0x3B */	{KeyCode2(0x3B), KeyCode2(0x54), KeyCode2(0x5E), KeyCode2(0x68)},	0	}, 
	{ /* 0x3C */	{KeyCode2(0x3C), KeyCode2(0x55), KeyCode2(0x5F), KeyCode2(0x69)},	0	}, 
	{ /* 0x3D */	{KeyCode2(0x3D), KeyCode2(0x56), KeyCode2(0x60), KeyCode2(0x6A)},	0	}, 
	{ /* 0x3E */	{KeyCode2(0x3E), KeyCode2(0x57), KeyCode2(0x61), KeyCode2(0x6B)},	0	}, 
	{ /* 0x3F */	{KeyCode2(0x3F), KeyCode2(0x58), KeyCode2(0x62), KeyCode2(0x6C)},	0	}, 
	{ /* 0x40 */	{KeyCode2(0x40), KeyCode2(0x59), KeyCode2(0x63), KeyCode2(0x6D)},	0	}, 
	{ /* 0x41 */	{KeyCode2(0x41), KeyCode2(0x5A), KeyCode2(0x64), KeyCode2(0x6E)},	0	}, 
	{ /* 0x42 */	{KeyCode2(0x42), KeyCode2(0x5B), KeyCode2(0x65), KeyCode2(0x6F)},	0	}, 
	{ /* 0x43 */	{KeyCode2(0x43), KeyCode2(0x5C), KeyCode2(0x66), KeyCode2(0x70)},	0	}, 
	{ /* 0x44 */	{KeyCode2(0x44), KeyCode2(0x5D), KeyCode2(0x67), KeyCode2(0x71)},	0	}, 
	{ /* 0x45 */	{ NoKeys                                                       },	0	}, 
	{ /* 0x46 */	{ NoKeys                                                       },	0	}, 
	{ /* 0x47 */	{KeyCode2(0x47), KeyCode('7'),   KeyCode2(0x77), KeyCodeNil    },	3	}, 
	{ /* 0x48 */	{KeyCode2(0x48), KeyCode('8'),   KeyCode2(0x8D), KeyCodeNil    },	4	}, 
	{ /* 0x49 */	{KeyCode2(0x49), KeyCode('9'),   KeyCode2(0x84), KeyCodeNil    },	5	}, 
	{ /* 0x4A */	{KeyCodeNil,     KeyCode('-'),   KeyCodeNil,     KeyCodeNil    },	0	}, 
	{ /* 0x4B */	{KeyCode2(0x4B), KeyCode('4'),   KeyCode2(0x73), KeyCodeNil    },	6	}, 
	{ /* 0x4C */	{KeyCodeNil,     KeyCode('5'),   KeyCodeNil,     KeyCodeNil    },	0	}, 
	{ /* 0x4D */	{KeyCode2(0x4D), KeyCode('6'),   KeyCode2(0x74), KeyCodeNil    },	7	}, 
	{ /* 0x4E */	{KeyCodeNil,     KeyCode('+'),   KeyCodeNil,     KeyCodeNil    },	0	}, 
	{ /* 0x4F */	{KeyCode2(0x4F), KeyCode('1'),   KeyCode2(0x75), KeyCodeNil    },	8	}, 
	{ /* 0x50 */	{KeyCode2(0x50), KeyCode('2'),   KeyCode2(0x91), KeyCodeNil    },	9	}, 
	{ /* 0x51 */	{KeyCode2(0x51), KeyCode('3'),   KeyCode2(0x76), KeyCodeNil    },	10	}, 
	{ /* 0x52 */	{KeyCode2(0x52), KeyCode('0'),   KeyCode2(0x92), KeyCodeNil    },	11	}, 
	{ /* 0x53 */	{KeyCode2(0x53), KeyCode('.'),   KeyCode2(0x93), KeyCodeNil    },	12	}, 
	{ /* 0x54 */	{ NoKeys                                                       },	0	}, 
	{ /* 0x55 */	{ NoKeys                                                       },	0	}, 
	{ /* 0x56 */	{ NoKeys                                                       },	0	}, 
	{ /* 0x57 */	{KeyCodeE(0x85), KeyCodeE(0x87), KeyCodeE(0x89), KeyCodeE(0x8B)},	0	}, 
	{ /* 0x58 */	{KeyCodeE(0x86), KeyCodeE(0x88), KeyCodeE(0x8A), KeyCodeE(0x8C)},	0	}
};

#define KeyCodesEntries ( sizeof(KeyCodes) / sizeof(KeyCodes[0]) )

const static KEY_CODES EnhancedKeyCodes[] = 
{
	/* scan code   regular         shift           ctrl            alt                enh */
	/* 0x00 - 0  */	{ NoKeys                                                       }, 
	/* 0x1C - 1  */	{KeyCode('\r'),  KeyCode('\r'),  KeyCode('\n'),  KeyCode2(0xA6)},
	/* 0x35 - 2  */	{KeyCode(0x2F),  KeyCode(0x3F),  KeyCode2(0x95), KeyCode2(0xA4)},
	/* 0x47 - 3  */	{KeyCodeE(0x47), KeyCodeE(0x47), KeyCodeE(0x77), KeyCode2(0x97)},
	/* 0x48 - 4  */	{KeyCodeE(0x48), KeyCodeE(0x48), KeyCodeE(0x8D), KeyCode2(0x98)},
	/* 0x49 - 5  */	{KeyCodeE(0x49), KeyCodeE(0x49), KeyCodeE(0x86), KeyCode2(0x99)},
	/* 0x4B - 6  */	{KeyCodeE(0x4B), KeyCodeE(0x4B), KeyCodeE(0x73), KeyCode2(0x9B)},
	/* 0x4D - 7  */	{KeyCodeE(0x4D), KeyCodeE(0x4D), KeyCodeE(0x74), KeyCode2(0x9D)},
	/* 0x4F - 8  */	{KeyCodeE(0x4F), KeyCodeE(0x4F), KeyCodeE(0x75), KeyCode2(0x9F)},
	/* 0x50 - 9  */	{KeyCodeE(0x50), KeyCodeE(0x50), KeyCodeE(0x91), KeyCode2(0xA0)},
	/* 0x51 - 10 */	{KeyCodeE(0x51), KeyCodeE(0x51), KeyCodeE(0x76), KeyCode2(0xA1)},
	/* 0x52 - 11 */	{KeyCodeE(0x52), KeyCodeE(0x52), KeyCodeE(0x92), KeyCode2(0xA2)},
	/* 0x53 - 12 */	{KeyCodeE(0x53), KeyCodeE(0x53), KeyCodeE(0x93), KeyCode2(0xA3)}
};

static short color_attrs[CON_MAX_COLORS]; // - like ncurses color pairs
static HANDLE hStdIn = INVALID_HANDLE_VALUE, hStdOut = INVALID_HANDLE_VALUE;

static COORD MousePosition = {-1, -1};
static DWORD MouseButtonState = 0;

#define ALT_PRESSED  (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)
#define CTRL_PRESSED (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)

static int GetExtendedKeyCode(const KEY_EVENT_RECORD * pKeyRecord)
{
    const DWORD dwState = pKeyRecord->dwControlKeyState;
	const unsigned short nScanCode = pKeyRecord->wVirtualScanCode;
	const KEY_CODES_EX * pCodesEx = &KeyCodes[nScanCode < KeyCodesEntries ? nScanCode : 0];
	const KEY_CODES * pCodes = ( dwState & ENHANCED_KEY ) ? 
		&EnhancedKeyCodes[pCodesEx->enh] : &pCodesEx->codes;

	if ( dwState & ALT_PRESSED )
		return pCodes->alt;

	else if ( dwState & CTRL_PRESSED )
		return pCodes->ctrl;

	else if ( dwState & SHIFT_PRESSED )
		return pCodes->shift;

	else
		return pCodes->regular;
}

#define MOUSE_FLAG  0x8000
#define MOUSE_MASK  ( MOUSE_FLAG | \
	CON_MOUSE_LBTN_DOWN | CON_MOUSE_LBTN_UP | \
	CON_MOUSE_RBTN_DOWN | CON_MOUSE_RBTN_UP | \
	CON_MOUSE_MBTN_DOWN | CON_MOUSE_MBTN_UP | \
	CON_MOUSE_MOVE | CON_MOUSE_DCLICK )

static int ParseInputEvent(const INPUT_RECORD * pRec, COORD * pMousePos)
{
	int nCode = 0;
	const KEY_EVENT_RECORD *   pKRec = &pRec->Event.KeyEvent;
	const MOUSE_EVENT_RECORD * pMRec = &pRec->Event.MouseEvent;

	switch ( pRec->EventType )
	{
	case KEY_EVENT:

		if ( !pKRec->bKeyDown )
			return 0;

		if ( (nCode = (unsigned char)pKRec->uChar.AsciiChar) )
			return nCode;

		if ( (nCode = GetExtendedKeyCode(pKRec)) )
			return nCode;

		return 0;

	case MOUSE_EVENT:
		{
			const DWORD dwButtonChange = pMRec->dwButtonState ^ MouseButtonState;
			const DWORD dwPositionChange = (MousePosition.X != pMRec->dwMousePosition.X) || (MousePosition.Y != pMRec->dwMousePosition.Y);
			DWORD dwEventFlags = pMRec->dwEventFlags;
			DWORD mask = 0;
			CONSOLE_SCREEN_BUFFER_INFO csbi;

			dwEventFlags &= ~(MOUSE_WHEELED | MOUSE_HWHEELED);

			if ( !dwPositionChange )
				dwEventFlags &= ~MOUSE_MOVED;

			if ( !dwButtonChange && !dwEventFlags )
				return 0;

			nCode = MOUSE_FLAG;

			mask = FROM_LEFT_1ST_BUTTON_PRESSED;
			if ( dwButtonChange & mask )
				nCode |= ( pMRec->dwButtonState & mask ) ? CON_MOUSE_LBTN_DOWN : CON_MOUSE_LBTN_UP;

			mask = RIGHTMOST_BUTTON_PRESSED;
			if ( dwButtonChange & mask )
				nCode |= ( pMRec->dwButtonState & mask ) ? CON_MOUSE_RBTN_DOWN : CON_MOUSE_RBTN_UP;

			mask = FROM_LEFT_2ND_BUTTON_PRESSED;
			if ( dwButtonChange & mask )
				nCode |= ( pMRec->dwButtonState & mask ) ? CON_MOUSE_MBTN_DOWN : CON_MOUSE_MBTN_UP;

			nCode |= ((dwEventFlags & 0x03) << 8);

			if ( pMousePos && GetConsoleScreenBufferInfo(hStdOut, &csbi) )
			{
				pMousePos->X = pMRec->dwMousePosition.X - csbi.srWindow.Left;
				pMousePos->Y = pMRec->dwMousePosition.Y - csbi.srWindow.Top;
			}

		}
		return nCode;

	default:
		return 0;
	}
}

static void TraceConsoleInput(const INPUT_RECORD * pRecord)
{
	const MOUSE_EVENT_RECORD * pMRec = &pRecord->Event.MouseEvent;

	switch ( pRecord->EventType )
	{
	case MOUSE_EVENT:
		MousePosition = pMRec->dwMousePosition;
		MouseButtonState = pMRec->dwButtonState;
		return;
	}
}

static int SkipInputEvents(void)
{
	DWORD nCount = 0;
	INPUT_RECORD record;

	while ( GetNumberOfConsoleInputEvents(hStdIn, &nCount) && nCount > 0 )
	{
		if ( !PeekConsoleInput(hStdIn, &record, 1, &nCount) || !nCount )
			break;

		if ( ParseInputEvent(&record, NULL) )
			return 1;

		if ( !ReadConsoleInput(hStdIn, &record, 1, &nCount) || !nCount )
			break;

		TraceConsoleInput(&record);
	}

	return 0;
}

static int ProcessInputEvent(COORD * pMousePos)
{
	INPUT_RECORD record;
	DWORD nCount;
	int nCode;

	while ( ReadConsoleInput(hStdIn, &record, 1, &nCount) && nCount )
	{
		nCode = ParseInputEvent(&record, pMousePos);
		TraceConsoleInput(&record);
		if ( nCode )
			return nCode;
	}

	return EOF;
}

void con_init()
{
    setlocale( LC_ALL, ".OCP" );

	hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    con_showCursor();
}

void con_deinit()
{
    hStdOut = hStdIn = INVALID_HANDLE_VALUE;
}

#define CONSOLE_CURSOR_SIZE 25
static void con_setCursor(int show)
{
    CONSOLE_CURSOR_INFO ci = {CONSOLE_CURSOR_SIZE, show};
    SetConsoleCursorInfo(hStdOut, &ci);
}

void con_showCursor()
{
    con_setCursor(TRUE);
}

void con_hideCursor()
{
    con_setCursor(FALSE);
}

int con_gotoXY(int x, int y)
{
    COORD pos = { x, y };

    return SetConsoleCursorPosition(hStdOut, pos);
}

int con_getXY(int *px, int *py)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

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

	if ( GetConsoleScreenBufferInfo(hStdOut, &csbi) )
	{
		*px = csbi.srWindow.Right - csbi.srWindow.Left;
		*py = csbi.srWindow.Bottom - csbi.srWindow.Top;
		return 1;
	}
	return 0; // error
}

int con_gotoPoint(point_t cursor)
{
	COORD pos = { cursor.x, cursor.y };
	return SetConsoleCursorPosition(hStdOut, pos);
}

int con_getPoint(point_t * pcursor)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if ( GetConsoleScreenBufferInfo(hStdOut, &csbi) )
	{
		*(PCOORD)pcursor = csbi.dwCursorPosition;
		return 1;
	}
	return 0; // error
}

int con_getMaxPoint(point_t * pmaxpos)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if ( GetConsoleScreenBufferInfo(hStdOut, &csbi) )
	{
		pmaxpos->x = csbi.srWindow.Right - csbi.srWindow.Left;
		pmaxpos->y = csbi.srWindow.Bottom - csbi.srWindow.Top;
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

int con_keyPressed(void)
{
	DWORD dwState;
	int bResult = 0;

	if ( hStdIn == INVALID_HANDLE_VALUE )
		return 0;

	_mlock(_CONIO_LOCK);
	__try
	{
		GetConsoleMode(hStdIn, &dwState);
		SetConsoleMode(hStdIn, 0);
		bResult = SkipInputEvents();
		SetConsoleMode(hStdIn, dwState);
	}
	__finally
	{
		_munlock(_CONIO_LOCK);
	}

	return bResult;
}

int con_getKey(point_t * pPoint)
{
	DWORD dwState;
    int nCode = EOF;

	if ( hStdIn == INVALID_HANDLE_VALUE )
		return EOF;

	_mlock(_CONIO_LOCK);
	__try
	{
		GetConsoleMode(hStdIn, &dwState);
		SetConsoleMode(hStdIn, 0);
		nCode = ProcessInputEvent((PCOORD)pPoint);
		SetConsoleMode(hStdIn, dwState);
	}
	__finally
	{
		_munlock(_CONIO_LOCK);
	}

    return nCode;
}

int con_isMouse(int code)
{
	return ( code & MOUSE_FLAG ) && ( (code & MOUSE_MASK) == code );
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
    if (n < 0 || n > CON_MAX_COLORS)
        return 0;
    return SetConsoleTextAttribute(hStdOut, color_attrs[n]);
}

