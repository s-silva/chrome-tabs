#include "../main.h"


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); 
LRESULT CALLBACK WindowProc2(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); 
LRESULT CALLBACK WindowProc_TabButton(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); 
void draw_tabs(HWND hwnd, int ctabid, int ctabx, int isctabvisible);

HINSTANCE application_instance = 0;
HWND hwnd2 = 0, hwnd1 = 0, hwnd_tabbutton = 0;

int cap = 0;
int mwindow = 0;

POINT mdown_pos;

#define POPUP_STYLES   (WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_SYSMENU | WS_CAPTION | WS_THICKFRAME)
#define POPUP_EXSTYLES (WS_EX_TOOLWINDOW | WS_EX_WINDOWEDGE)
#define CHILD_STYLES   (WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS)
#define CHILD_EXSTYLES (0)

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
	tab_set = (struct window_tab*) sys_mem_realloc(tab_set, sizeof(struct window_tab) * tab_count);
	if(!tab_set) return 0;

	tab_set[tc].title = (string)sys_mem_alloc(str_len(title) + 16);
	str_cpy(tab_set[tc].title, title);

	tab_set[tc].index = tc;
	tab_set[tc].isactive = 1;
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

}

int tab_uninit()
{
	int i;

	for(i=0; i<tab_count; i++)
	{
		if(tab_set[i].title)
			sys_mem_free(tab_set[i].title);
	}

	if(tab_set) sys_mem_free(tab_set);

	tab_set = 0;
	tab_count = 0;
	window_count = 0;

	return 0;
}

HWND tab_get_window(int tabid)
{

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


void tab_cleanwindows()
{
	int i;

	for(i=0; i<window_count; i++)
	{
		if(tab_get_tab_count(i) == 0)
		{
			ShowWindow(tab_windows[i], SW_HIDE);
			twindow_remove(tab_windows[i]);
		}
	}

}

int tab_get_id_bypoint(HWND wnd, int x)
{

}

int tab_activate(int tabid)
{
	
}

/* main */



int wWinMain(HINSTANCE hInst,HINSTANCE,LPWSTR,int nCmdShow) 
{ 
	WNDCLASS wc; 
	HWND hwnd; 
	MSG msg; 

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	Status st = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	assert(st == Ok);
	if (st != Ok) return FALSE;

	application_instance = hInst;

	wc.style=CS_HREDRAW | CS_VREDRAW; 
	wc.lpfnWndProc=WindowProc; 
	wc.cbClsExtra=0; 
	wc.cbWndExtra=0; 
	wc.hInstance=hInst; 
	wc.hIcon=LoadIcon(hInst,(LPCTSTR)1); 
	wc.hCursor=LoadCursor(NULL,IDC_ARROW); 
	wc.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(0xd9, 0xe3, 0xec)); 
	wc.lpszMenuName=NULL; 
	wc.lpszClassName=application_title; 

	if (!RegisterClass(&wc)) 
		return 0; 

	wc.style=CS_HREDRAW | CS_VREDRAW; 
	wc.lpfnWndProc=WindowProc2; 
	wc.cbClsExtra=0; 
	wc.cbWndExtra=0; 
	wc.hInstance=hInst; 
	wc.hIcon=LoadIcon(hInst,(LPCTSTR)1); 
	wc.hCursor=LoadCursor(NULL,IDC_ARROW); 
	wc.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(0xbc, 0xc6, 0xcf)); 
	wc.lpszMenuName=NULL; 
	wc.lpszClassName=uni("Resample1CWindow"); 

	if (!RegisterClass(&wc)) 
		return 0; 

	hwnd1 = hwnd = CreateWindowEx(WS_EX_WINDOWEDGE | WS_EX_APPWINDOW, application_title,application_title, 
		WS_THICKFRAME | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_OVERLAPPED, 
		CW_USEDEFAULT,CW_USEDEFAULT,1000,500, 
		NULL,NULL,hInst,NULL); 

	if (!hwnd) 
		return 0; 

	ShowWindow(hwnd,nCmdShow); 
	UpdateWindow(hwnd); 

	int winid = twindow_new(hwnd);
	

	hwnd2 = CreateWindow(uni("Resample1CWindow"),uni("Tab 1"), 
		(WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS), 
		0,84,1000,700, 
		hwnd,NULL,hInst,NULL); 

	HWND hwnd3 = CreateWindow(uni("Resample1CWindow"),uni("Tab 2"), 
		(WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS), 
		0,84,1000,700, 
		hwnd,NULL,hInst,NULL); 

	current_tab_id = tab_new(uni("Tab 1"), winid);
	tab_setpanel(current_tab_id, hwnd2);

	int nid = tab_new(uni("Tab 2"), winid);
	tab_setpanel(nid, hwnd3);

	ShowWindow(hwnd2,nCmdShow); 
	UpdateWindow(hwnd2); 

	draw_tabs(hwnd, -1, 0, 0);




	wc.style=CS_HREDRAW | CS_VREDRAW; 
	wc.lpfnWndProc=WindowProc_TabButton; 
	wc.cbClsExtra=0; 
	wc.cbWndExtra=0; 
	wc.hInstance=hInst; 
	wc.hIcon=LoadIcon(hInst,(LPCTSTR)1); 
	wc.hCursor=LoadCursor(NULL,IDC_ARROW); 
	wc.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(0xe2, 0xeb, 0xf1)); 
	wc.lpszMenuName=NULL; 
	wc.lpszClassName=uni("ResampleTabButton"); 

	if (!RegisterClass(&wc)) 
		return 0; 

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

	ui_shape_draw_rect(gr, 0xffd9e3ec, 0, 0, w, 23);

	for(i=0; i<tab_count; i++)
	{
		if(tab_set[i].wnd == hwnd)
		{
			if(tab_set[i].isactive)
				ui_shape_draw_rect(gr, 0xffe3ebf1, xpos, 5, tab_width - 3, 27);
			else
				ui_shape_draw_rect(gr, 0xffe3ebf1, xpos, 5, tab_width - 3, 29);

			ui_shape_draw_rect(gr, 0xffbcc6cf, xpos + 7, 6 + 5, 16, 16);
			ui_text_draw(gr, tab_set[i].title, 0xff60898c, xpos + 28, 6 + 5, 0);
			xpos += tab_width;
		}
	}
}


void draw_tabs(HWND hwnd, int ctabid, int ctabx, int isctabvisible)
{
	PAINTSTRUCT ps; 
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



LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
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

					current_tab_id = twindow_get_ctab(hwnd);

					if(current_tab_id >= 0)
					{
						ShowWindow(hwnd_tabbutton, SW_SHOW);
						SetWindowPos(hwnd_tabbutton, 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

						RECT rc;
						int w, tab_width = 160;

						GetClientRect(hwnd, &rc);
						w = rc.right;

						if(!tab_count) break;

						tab_width = (w - 90) / tab_count;
						if(tab_width > 160) tab_width = 160;
						else if(tab_width < 30) tab_width = 30;

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


LRESULT CALLBACK WindowProc2(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
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





LRESULT CALLBACK WindowProc_TabButton(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
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
				
				hw = CreateWindowEx(WS_EX_WINDOWEDGE, application_title,application_title, 
								WS_OVERLAPPEDWINDOW, 
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