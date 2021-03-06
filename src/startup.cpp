/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2013, Sandaruwan Silva <c-h [-a-t] users [-dot-] sf [-dot-] net>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
**/

#include <windows.h>
#include <gdiplus.h>
#include <assert.h>

using namespace Gdiplus;

typedef wchar_t *string;
typedef wchar_t letter;
typedef Graphics graphic_context;
typedef DWORD color;

typedef struct _box
{
	int x, y, w, h;
} box;

typedef struct _point
{
	int x, y;
} point;

typedef struct _pointex
{
	int x, y, d1, d2;
} pointex;


#define uni(x) L ## x
#define str_len(s)         wcslen(s)
#define str_cpy(d, s)      wcscpy(d, s)
#define color_get_r(rgb)   ((BYTE) ((rgb) >> 16)) 
#define color_get_g(rgb)   ((BYTE) (((WORD) (rgb)) >> 8)) 
#define color_get_b(rgb)   ((BYTE) (rgb))
#define color_get_a(rgb)   ((BYTE) ((rgb) >> 24)) 
#define application_title  uni("Axel Browser")


LRESULT CALLBACK callback_window_main(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); 
LRESULT CALLBACK callback_window_panel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); 
LRESULT CALLBACK callback_window_host(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK callback_window_tab(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); 
void draw_tabs(HWND hwnd, int ctabid, int ctabx, int isctabvisible);

HINSTANCE application_instance = 0;
HWND hwnd2 = 0, hwnd1 = 0, hwnd_tabbutton = 0;

int cap = 0;
int mwindow = 0;

POINT mdown_pos;

int window_count = 0;

struct window_tab
{
	string  title;
	void   *data;
	int     index;
	int     isactive;
	int     window_index;
	HWND    wnd, panel;
} *tab_set = 0;

int   tab_count = 0;
HWND  tab_windows[32];
int   current_tab_id = 0;

HWND hwnd_main = 0;


/* basic drawing */

int ui_shape_draw_rect(graphic_context &gc, color c, int x, int y, int w, int h)
{
	SolidBrush brush (Color (color_get_a(c), color_get_r(c), color_get_g(c), color_get_b(c)));
	gc.FillRectangle (&brush, x, y, w, h);
	return 0;
}

int ui_text_draw(graphic_context &gc, string text, color c, int x, int y, int mode)
{
	const FontFamily &cgdi_fm = FontFamily(L"Cabin");
	static Font       cgdi_font(&cgdi_fm, 9);

	SolidBrush brush (Color (color_get_a(c), color_get_r(c), color_get_g(c), color_get_b(c)));
	gc.DrawString(text, -1, &cgdi_font, PointF(x, y), &brush);
	return 0;
}


/* tab management */

int twindow_getid(HWND wnd)
{
	int i;
	for(i=0; i<window_count; i++)
	{
		if(tab_windows[i] == wnd)
			return i;
	}
	return -1;
}

int twindow_new(HWND wnd)
{
	if(window_count >= 31) return -1;
	tab_windows[window_count++] = wnd;	
	return window_count - 1;
}

int twindow_remove(HWND wnd)
{
	int i;
	for(i=0; i<window_count; i++)
	{
		if(tab_windows[i] == wnd)
		{
			tab_windows[i] = tab_windows[window_count - 1];
			window_count--;
		}
	}
	return 0;
}

int twindow_get_ctab(HWND wnd)
{
	int i;
	for(i=0; i<tab_count; i++)
	{
		if(tab_set[i].wnd == wnd)
		{
			return i;
		}
	}
	return -1;
}

int tab_new(const string title, int windowid)
{
	int tc = tab_count;

	tab_count++;
	tab_set = (struct window_tab*) realloc(tab_set, sizeof(struct window_tab) * tab_count);
	if(!tab_set) return 0;

	tab_set[tc].title = (string)malloc(str_len(title) + 16);
	str_cpy(tab_set[tc].title, title);

	tab_set[tc].index = tc;
	tab_set[tc].isactive = 0;
	tab_set[tc].window_index = windowid;
	tab_set[tc].data = 0;
	tab_set[tc].wnd = tab_windows[windowid];
	tab_set[tc].panel = 0;
	return tc;
}

void tab_setpanel(int tc, HWND wnd)
{
	tab_set[tc].panel = wnd;
}

HWND tab_getpanel(int tc)
{
	return tab_set[tc].panel;
}

int tab_remove(int windowid, int tabid)
{
	return 0;
}

int tab_uninit()
{
	int i;

	for(i=0; i<tab_count; i++)
	{
		if(tab_set[i].title)
			free(tab_set[i].title);
	}

	if(tab_set) free(tab_set);

	tab_set = 0;
	tab_count = 0;
	window_count = 0;

	return 0;
}

HWND tab_get_window(int tabid)
{
	return (HWND)0;
}

int tab_get_tab_count(int winid)
{
	int i, c = 0;

	for(i=0; i<tab_count; i++)
	{
		if(tab_set[i].wnd == tab_windows[winid]) c++;
	}

	return c;
}

int tab_move(int tabid, int winid)
{
	if(tabid < 0) return -1;
	if(tabid >= tab_count) return -1;

	tab_set[tabid].window_index = winid;
	tab_set[tabid].wnd = tab_windows[winid];

	return 0;
}

HWND tab_is_window(int x, int y)
{
	int i;
	RECT r;
	HWND hwnd = 0;
	for(i=0; i<window_count; i++)
	{
		hwnd = tab_windows[i];
		GetWindowRect(hwnd, &r);
		if(x > r.left && x < r.right && y > r.top && y < r.bottom)
		{
			return hwnd;
		}
	}

	return 0;
}

int tab_getwindowid(HWND wnd)
{
	int i;

	for(i=0; i<window_count; i++)
	{
		if(wnd == tab_windows[i])
			return i;
	}

	return 0;
}

HWND tab_add(HWND hwnd, const string title)
{
	HWND newwnd = CreateWindow(uni("Resample1CWindow"),title, 
		(WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS), 0,84,1000,700, hwnd,NULL,application_instance,NULL); 

	int current_tab_id = tab_new((const string)title, tab_getwindowid(hwnd));
	tab_setpanel(current_tab_id, newwnd);

	return newwnd;
}

void tab_cleanwindows()
{
	int i;

	for(i=0; i<window_count; i++)
	{
		if(tab_get_tab_count(i) == 0)
		{
			//ShowWindow(tab_windows[i], SW_HIDE);
			if(tab_windows[i] && tab_windows[i] != hwnd_main)
			{
				DestroyWindow(tab_windows[i]);
				tab_windows[i] = 0;
				twindow_remove(tab_windows[i]);
			}
			
		}
	}

}

int tab_get_id_bypoint(HWND wnd, int x)
{
	return 0;
}

int tab_activate(HWND wnd, int tabposid)
{
	int i, c = 0, cid = -1;

	for(i=0; i<tab_count; i++)
	{
		if(tab_set[i].wnd == wnd)
		{
			if(c == tabposid)
			{
				tab_set[i].isactive = 1;
				ShowWindow(tab_set[i].panel, SW_SHOW);
				cid = i;
			}else{
				tab_set[i].isactive = 0;
				ShowWindow(tab_set[i].panel, SW_HIDE);
			}
			c++;
		}
	}

	return cid;
}

int tab_getid(HWND wnd, int x, int w)
{
	/* -1  = add new */
	int tabposid;
	int i, c = 0, cid = -1;
	int tab_width = 160;

	if(!tab_count) return 0;

	tab_width = (w - 90) / tab_count;
	if(tab_width > 160) tab_width = 160;
	else if(tab_width < 30) tab_width = 30;

	tabposid = (x - 10) / tab_width;

	for(i=0; i<tab_count; i++)
	{
		if(tab_set[i].wnd == wnd)
		{
			if(c == tabposid)
			{
				return i;
			}
			c++;
		}
	}

	if(x - ((c * tab_width) + 10) < 30) return -1;
	return 0;
}

/* main */


int wWinMain(HINSTANCE hInst,HINSTANCE,LPWSTR,int nCmdShow) 
{ 
	WNDCLASS            wc; 
	HWND                hwnd; 
	MSG                 msg; 
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;


	Status st = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	assert(st == Ok);
	if(st != Ok) return FALSE;

	application_instance = hInst;

	wc.style         = CS_HREDRAW | CS_VREDRAW; 
	wc.lpfnWndProc   = callback_window_main; 
	wc.cbClsExtra    = 0; 
	wc.cbWndExtra    = 0; 
	wc.hInstance     = hInst; 
	wc.hIcon         = LoadIcon(hInst,(LPCTSTR)1); 
	wc.hCursor       = LoadCursor(NULL,IDC_ARROW); 
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0xd9, 0xe3, 0xec)); 
	wc.lpszMenuName  = NULL; 
	wc.lpszClassName = application_title; 

	if(!RegisterClass(&wc)) return 0; 

	wc.lpfnWndProc   = callback_window_panel;
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0xbc, 0xc6, 0xcf)); 
	wc.lpszClassName = uni("Resample1CWindow"); 

	if(!RegisterClass(&wc)) return 0; 

	wc.lpfnWndProc   = callback_window_host; 
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0xbc, 0xc6, 0xcf)); 
	wc.lpszClassName = uni("Resample1SubTWindow"); 

	if(!RegisterClass(&wc)) return 0; 

	wc.lpfnWndProc   = callback_window_tab; 
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0xe2, 0xeb, 0xf1)); 
	wc.lpszClassName = uni("ResampleTabButton"); 

	if(!RegisterClass(&wc)) return 0; 

	hwnd1 = hwnd = CreateWindowEx(WS_EX_WINDOWEDGE | WS_EX_APPWINDOW, application_title, application_title, 
		WS_THICKFRAME | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_OVERLAPPED, 
		CW_USEDEFAULT, CW_USEDEFAULT, 1000, 500, NULL, NULL, hInst, NULL); 

	if (!hwnd) return 0; 
	hwnd_main = hwnd;

	ShowWindow(hwnd,nCmdShow); 
	UpdateWindow(hwnd); 

	int winid = twindow_new(hwnd);
	

	hwnd2 = CreateWindow(uni("Resample1CWindow"),uni("Tab 1"), 
		(WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS), 0,84,1000,700, hwnd,NULL,hInst,NULL); 

	HWND hwnd3 = CreateWindow(uni("Resample1CWindow"),uni("Tab 2"), 
		(WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS), 0,84,1000,700, hwnd,NULL,hInst,NULL); 

	HWND hwnd4 = CreateWindow(uni("Resample1CWindow"),uni("Tab 3"), 
		(WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS), 0,84,1000,700, hwnd,NULL,hInst,NULL); 

	current_tab_id = tab_new((const string)uni("Tab 1"), winid);
	tab_setpanel(current_tab_id, hwnd2);

	int nid = tab_new((const string)uni("Tab 2"), winid);
	tab_setpanel(nid, hwnd3);

	int nid2 = tab_new((const string)uni("Tab 3"), winid);
	tab_setpanel(nid2, hwnd4);

	ShowWindow(hwnd2,nCmdShow); 
	UpdateWindow(hwnd2); 

	draw_tabs(hwnd, -1, 0, 0);



	hwnd_tabbutton = CreateWindow(uni("ResampleTabButton"),application_title, 
		(WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS), 
		0,0,160, 30,
		hwnd,NULL,hInst,NULL); 


 
	while (GetMessage(&msg,NULL,0,0) > 0) 
	{ 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	} 

	//GdiplusShutdown(gdiplusToken);
	return 0;
} 

void grdraw(graphic_context &gr)
{
	SolidBrush brush2 (Color (122 /*A*/, 200 /*R*/, 200 /*G*/, 200 /*B*/));
	gr.FillRectangle (&brush2, 0, 60, 2000, 2000);
}


void draw_tabs_gr(Graphics &gr, HWND hwnd, int w, int ctabid, int ctabx, int isctabvisible)
{

	int i, xpos = 10;
	int tab_width = 160;

	if(!tab_count) return;

	tab_width = (w - 90) / tab_count;
	if(tab_width > 160) tab_width = 160;
	else if(tab_width < 30) tab_width = 30;

	ui_shape_draw_rect(gr, 0xffd9e3ec, 0, 0, w, 34);

	for(i=0; i<tab_count; i++)
	{
		if(tab_set[i].wnd == hwnd)
		{
			if(!tab_set[i].isactive)
				ui_shape_draw_rect(gr, 0xffe3ebf1, xpos, 5, tab_width - 3, 27);
			else
				ui_shape_draw_rect(gr, 0xffe3ebf1, xpos, 5, tab_width - 3, 29);

			ui_shape_draw_rect(gr, 0xffbcc6cf, xpos + 7, 6 + 5, 16, 16);
			ui_text_draw(gr, tab_set[i].title, 0xff60898c, xpos + 28, 6 + 5, 0);
			xpos += tab_width;
		}
	}

	ui_shape_draw_rect(gr, 0xffe3ebf1, xpos, 5, 30, 27);
	ui_text_draw(gr, uni("+"), 0xff60898c, xpos + 10, 6 + 5, 0);
	xpos += tab_width;
}


void draw_tabs(HWND hwnd, int ctabid, int ctabx, int isctabvisible)
{
	HDC hdc; 
	RECT r; 
	
	GetClientRect(hwnd,&r); 
	hdc = GetDC(hwnd);
	Graphics gr(hdc);


	gr.SetSmoothingMode(SmoothingModeAntiAlias);
	gr.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	draw_tabs_gr(gr, hwnd, r.right, ctabid, ctabx, isctabvisible);

	ReleaseDC(hwnd, hdc);
	//EndPaint(hwnd, &ps);
}


LRESULT CALLBACK callback_window_main(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{ 
	switch (msg) 
	{ 
		case WM_PAINT: 
		{ 
			PAINTSTRUCT ps; 
			HDC hdc; 
			RECT r; 
			
			GetClientRect(hwnd,&r); 
			hdc = BeginPaint(hwnd, &ps);
			Graphics gr(hdc);
			const FontFamily &fm = FontFamily(L"Roboto");
			Font font(&fm, 8);
			gr.SetSmoothingMode(SmoothingModeAntiAlias);
			gr.SetInterpolationMode(InterpolationModeHighQualityBicubic);
			Color color( 122, 0, 0, 0 );
			SolidBrush brush(color);
			Color color2( 122, 200, 200, 200 );
			SolidBrush brush2 (Color (122 /*A*/, 200 /*R*/, 200 /*G*/, 200 /*B*/));

			ui_shape_draw_rect(gr, 0xffd9e3ec, 0, 0, r.right, 34);
			ui_shape_draw_rect(gr, 0xffbcc6cf, 0, 84, r.right, r.bottom);
			ui_shape_draw_rect(gr, 0xffe3ebf1, 0, 34, r.right, 50);

			ui_shape_draw_rect(gr, 0xffeff4f7, 8, 34 + 7, 42, 36);
			ui_shape_draw_rect(gr, 0xffeff4f7, 8 + 42 + 4, 34 + 7, 42, 36);
			ui_shape_draw_rect(gr, 0xffeff4f7, 8 + (42 * 2) + 8, 34 + 7, 42, 36);

			ui_shape_draw_rect(gr, 0xfff4f7fa, 147, 34 + 7, r.right - 147 - 53, 36);

			ui_shape_draw_rect(gr, 0xffeff4f7, r.right - 47, 34 + 7, 42, 36);

			
			EndPaint(hwnd, &ps);

			draw_tabs(hwnd, -1, 0, 0);

			break; 
		} 

		case WM_DESTROY: 
			tab_uninit();
			PostQuitMessage(0); 
			break; 

		case WM_LBUTTONDOWN:
			mwindow = 0;
			SetCapture(hwnd);
			if(HIWORD(lparam) < 23)
			{
				if(LOWORD(lparam) < 10)
				{
					SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0); 
				}else{
					POINT pt;
					GetCursorPos(&pt);
					
					RECT rc;
					int w, tab_width = 160;

					GetClientRect(hwnd, &rc);
					w = rc.right;

					if(!tab_count) break;

					tab_width = (w - 90) / tab_count;
					if(tab_width > 160) tab_width = 160;
					else if(tab_width < 30) tab_width = 30;

					if(tab_getid(hwnd, LOWORD(lparam), w) == -1)
					{
						tab_add(hwnd, uni("New Tab"));
						break;
					}

					if((LOWORD(lparam) - 10) / tab_width)

					current_tab_id = tab_activate(hwnd, (LOWORD(lparam) - 10) / tab_width);

					if(current_tab_id >= 0)
					{
						ShowWindow(hwnd_tabbutton, SW_SHOW);
						SetWindowPos(hwnd_tabbutton, 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);


						mdown_pos.x = (LOWORD(lparam) - 10) % tab_width;
						mdown_pos.y = HIWORD(lparam) - 5;

						//SendMessage(hwnd_tabbutton, WM_NCLBUTTONDOWN, HTCAPTION, 0); 
						cap = 1;
					}
				}
			}
			break;

		case WM_LBUTTONUP:
			if(cap) SendMessage(hwnd_tabbutton, WM_LBUTTONUP, 0, 0);
			cap = 0;
			draw_tabs(hwnd, -1, 0, 0);
			ReleaseCapture();
			break;

		case WM_MOUSEMOVE:
			if(cap)
			{
				POINT p, clp;
				RECT r;

				GetCursorPos(&p);
				GetClientRect(hwnd, &r);

				clp.x = 0;
				clp.y = 5;

				ClientToScreen(hwnd, &clp);


				if(LOWORD(lparam) < -20 || LOWORD(lparam) > r.right || HIWORD(lparam) < 0 || HIWORD(lparam) > 45)
				{
					SetWindowPos(hwnd_tabbutton, NULL, p.x - mdown_pos.x, p.y - mdown_pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); 
				}else{
					SetWindowPos(hwnd_tabbutton, NULL, p.x - mdown_pos.x, clp.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); 
					draw_tabs(hwnd, current_tab_id, p.x, 0);
				}
				return 0;
			}
			break;


		default: 
			return DefWindowProc(hwnd, msg, wparam, lparam); 
	} 
	return 0; 
} 


LRESULT CALLBACK callback_window_panel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{ 
	switch (msg) 
	{ 
	case WM_PAINT: 
		{ 
			PAINTSTRUCT ps; 
			HDC hdc; 
			RECT r;
			letter buf[128];

			GetClientRect(hwnd,&r); 
			hdc = BeginPaint(hwnd, &ps);
			Graphics gr(hdc);

			gr.SetSmoothingMode(SmoothingModeAntiAlias);
			gr.SetInterpolationMode(InterpolationModeHighQualityBicubic);

			GetWindowText(hwnd, buf, 128);
			ui_text_draw(gr, buf, 0xff60898c, (r.right / 2) - 10, ((r.bottom - r.top) / 2) - 5, 0);

			EndPaint(hwnd, &ps);
			break; 
		}
		break;

	default: 
		return DefWindowProc(hwnd, msg, wparam, lparam); 
	} 
	return 0; 
} 





LRESULT CALLBACK callback_window_tab(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{ 
	switch (msg) 
	{ 
	case WM_NCLBUTTONUP:
	case WM_LBUTTONUP:
		{
			POINT pt;
			GetCursorPos(&pt);
			HWND hw = 0;
			HWND hover_tab_window = tab_is_window(pt.x, pt.y);


			ShowWindow(hwnd_tabbutton, SW_HIDE);
			if(!hover_tab_window)
			{
				
				hw = CreateWindowEx( WS_EX_TOOLWINDOW, uni("Resample1SubTWindow"),application_title, 
								WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 
								pt.x,pt.y,1000,500, 
								NULL,NULL,application_instance,NULL); 

				int winid = twindow_new(hw);
				if(winid < 0) break;

				tab_move(current_tab_id, winid);
			}else{

				SetForegroundWindow(hover_tab_window);
				tab_move(current_tab_id, twindow_getid(hover_tab_window));
				draw_tabs(hover_tab_window, -1, 0, 0);
				hw = hover_tab_window;
			}

			tab_cleanwindows();

			if(hw)
			{
				///SetParent(hwnd2, hw);
				RECT rc;
				GetClientRect(hw, &rc);
				HWND panel = tab_getpanel(current_tab_id);
				SetParent(panel, hw);
				
				ShowWindow(panel, SW_SHOW);
				UpdateWindow(panel);

				if(pt.y < 50)
					ShowWindow(hw,SW_SHOWMAXIMIZED);
				else
					ShowWindow(hw,SW_SHOW);

				SetWindowPos(panel, NULL, 0, 84, rc.right, rc.bottom - 84, SWP_NOZORDER); 

				UpdateWindow(hw); 
			}
		
		}
		break;

	default: 
		return DefWindowProc(hwnd, msg, wparam, lparam); 
	} 
	return 0; 
} 

LRESULT CALLBACK callback_window_host(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{ 
	switch (msg) 
	{ 
	case WM_DESTROY: 
		break; 

	default: 
		return callback_window_main(hwnd, msg, wparam, lparam);
	} 
	return 0; 
} 


/*
 *---------------------------------------------------------------------------------------
 * eof.
 *
 */