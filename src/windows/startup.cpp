#include "../main.h"


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); 
LRESULT CALLBACK WindowProc2(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); 
LRESULT CALLBACK WindowProc_TabButton(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); 

HINSTANCE application_instance = 0;
HWND hwnd2 = 0, hwnd1 = 0, hwnd_tabbutton = 0;

int cap = 0;
int mwindow = 0;

#define POPUP_STYLES   (WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_SYSMENU | WS_CAPTION | WS_THICKFRAME)
#define POPUP_EXSTYLES (WS_EX_TOOLWINDOW | WS_EX_WINDOWEDGE)
#define CHILD_STYLES   (WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS)
#define CHILD_EXSTYLES (0)


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
	wc.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(0, 0,0)); 
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



		hwnd2 = CreateWindow(uni("Resample1CWindow"),application_title, 
		(WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS), 
		0,58,1000,700, 
		hwnd,NULL,hInst,NULL); 




	ShowWindow(hwnd2,nCmdShow); 
	UpdateWindow(hwnd2); 




	wc.style=CS_HREDRAW | CS_VREDRAW; 
	wc.lpfnWndProc=WindowProc_TabButton; 
	wc.cbClsExtra=0; 
	wc.cbWndExtra=0; 
	wc.hInstance=hInst; 
	wc.hIcon=LoadIcon(hInst,(LPCTSTR)1); 
	wc.hCursor=LoadCursor(NULL,IDC_ARROW); 
	wc.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(128, 128,128)); 
	wc.lpszMenuName=NULL; 
	wc.lpszClassName=uni("ResampleTabButton"); 

	if (!RegisterClass(&wc)) 
		return 0; 

	hwnd_tabbutton = CreateWindow(uni("ResampleTabButton"),application_title, 
		(WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS), 
		0,0,160, 23,
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

			ui_shape_draw_rect(gr, 0xff878787, 0, 58, r.right, r.bottom);
			ui_shape_draw_rect(gr, 0xffe2ebf1, 0, 23, r.right, 35);

			EndPaint(hwnd, &ps);
			break; 
		} 

		case WM_DESTROY: 
			PostQuitMessage(0); 
			break; 

		case WM_LBUTTONDOWN:
			mwindow = 0;
			if(HIWORD(lparam) < 23)
			{
				if(LOWORD(lparam) < 200)
				{
					SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0); 
				}else{
					POINT pt;
					GetCursorPos(&pt);

					
					ShowWindow(hwnd_tabbutton, SW_SHOW);
					SetWindowPos(hwnd_tabbutton, 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

					SendMessage(hwnd_tabbutton, WM_NCLBUTTONDOWN, HTCAPTION, 0); 
					cap = 1;
				}
			}
			break;

		case WM_LBUTTONUP:
			cap = 0;
			ReleaseCapture();
			SendMessage(hwnd_tabbutton, WM_LBUTTONUP, HTCAPTION, 0); 
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
		case WM_MOVING:
		{
			/*RECT *rect;
			rect = (RECT*)lparam;
			 POINT pt;
			GetCursorPos(&pt);
			if(pt.x < 500 && pt.y < 500)
			{
					RECT rtp;
					GetWindowRect(hwnd1, &rtp);
					cap = 1;
					//SetPopupStyle(hwnd2, hwnd1);
					{

						 rect->top = rtp.top + 58;
						 rect->left = rtp.left;
						 rect->right = rtp.right;
						 rect->bottom = rtp.bottom - 58;
						 PostMessage(hwnd2, WM_NCLBUTTONUP, HTCAPTION, 0);
					}
			}else{
					//SetPopupStyle(hwnd2, 0);
				if(!mwindow)
				{
					mwindow = 1;
						HWND hw = CreateWindowEx(WS_EX_WINDOWEDGE, application_title,application_title, 
							WS_SIZEBOX | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_POPUP, 
							CW_USEDEFAULT,CW_USEDEFAULT,1000,500, 
							NULL,NULL,application_instance,NULL); 

							ShowWindow(hw,SW_SHOW);
													UpdateWindow(hw); 
												}
					cap = 0;
			}*/
		}
			break;

	case WM_LBUTTONDOWN:
		if(LOWORD(lparam) < 200)
		{
			SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0); 
		}else{
			SendMessage(hwnd2, WM_NCLBUTTONDOWN, HTCAPTION, 0); 
			cap = 1;
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
			HWND hw = CreateWindowEx(WS_EX_WINDOWEDGE, application_title,application_title, 
							WS_SIZEBOX | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_POPUP, 
							CW_USEDEFAULT,CW_USEDEFAULT,1000,500, 
							NULL,NULL,application_instance,NULL); 

			ShowWindow(hw,SW_SHOW);
			UpdateWindow(hw); 
		
		}
		break;

	default: 
		return DefWindowProc(hwnd, msg, wparam, lparam); 
	} 
	return 0; 
} 