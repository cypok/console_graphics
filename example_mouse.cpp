#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "console.h"

inline bool IsPointInRect(rect_t rect, point_t pt)
{
	return (
		pt.x >= rect.left &&
		pt.x < rect.right &&
		pt.y >= rect.top  &&
		pt.y < rect.bottom
		);
}

class Button : protected rect_t
{
	bool m_bHover;
	short m_Colors[2];
	const char * m_pText;
	bool m_bRedraw;

public:
	Button(short l, short t, short w, short h, short colorOn, short colorOff, const char * pText)
	{
		left = l;
		top  = t;
		right  = l + w;
		bottom = t + h;
		m_bHover = false;
		m_Colors[0] = colorOff;
		m_Colors[1] = colorOn;
		m_pText = pText;
		m_bRedraw = true;
	}
	
	void onMouseMove(point_t pt)
	{
		if ( IsPointInRect(*this, pt) != m_bHover )
		{
			m_bHover = !m_bHover;
			m_bRedraw = true;
		}
	}

	bool getHover(void) const { return m_bHover; }

	void setText(const char * pText)
	{
		if ( m_pText != pText )
		{
			m_pText = pText;
			m_bRedraw = true;
		}
	}

	void draw(void)
	{
		if ( !m_bRedraw )
			return;

		point_t old, c;

		con_getPoint(&old);
		con_setColor(m_Colors[m_bHover]);

		for (c.y = top; c.y < bottom; c.y++)
		{
			for (c.x = left; c.x < right; c.x++)
			{
				con_gotoPoint(c);
				con_outTxt(" ");
			}
		}

		if ( m_pText )
		{
			const size_t len = strlen(m_pText);
			c.y = (top + bottom) / 2;
			c.x = (left + right - len) / 2;
			con_gotoPoint(c);
			con_outTxt("%s", m_pText);
		}

		con_setColor(0);
		con_gotoPoint(old);

		m_bRedraw = false;
	}
};


void main(void)
{
	int count = 0;
	point_t ptMouse;

	int state = 1;
	const char * name[2] = {"Push me", "Touch me"};

	Button b1(15, 5, 15, 3, 1, 2, state ? name[0] : NULL);
	Button b2(45, 5, 15, 3, 3, 4, state ? NULL : name[1]);

	con_init();
	con_hideCursor();
	con_initPair(0, CON_COLOR_WHITE, CON_COLOR_BLACK);
	con_initPair(1, CON_COLOR_WHITE, CON_COLOR_RED);
	con_initPair(2, CON_COLOR_WHITE, CON_COLOR_GREEN);
	con_initPair(3, CON_COLOR_WHITE, CON_COLOR_YELLOW);
	con_initPair(4, CON_COLOR_WHITE, CON_COLOR_BLUE);
	con_setColor(0);

	while (1)
	{
		if ( !con_keyPressed() )
			continue;

		int code = con_getKey(&ptMouse);

		if ( con_isMouse(code) )
		{
			printf("- [%8.8d] --- code:%8.8X -- (%02d, %02d)\r", count, code, ptMouse.x, ptMouse.y);

			b1.onMouseMove(ptMouse);

			if ( code & CON_MOUSE_LBTN_DOWN )
			{
				if ( b1.getHover() && state )
				{
					state = !state;
					b1.setText(state ? name[0] : NULL);
					b2.setText(state ? NULL : name[1]);
				}
			}

			b2.onMouseMove(ptMouse);

			if ( b2.getHover() && !state )
			{
				state = !state;
				b1.setText(state ? name[0] : NULL);
				b2.setText(state ? NULL : name[1]);
			}

			b1.draw();
			b2.draw();
		}
		else
		{
			printf("- [%8.8d] --- code:%8.8X --                    \r", count, code);

			if ( code == CON_KEY_ESCAPE )
				break;
		}



		count++;
	}

	con_deinit();
}

