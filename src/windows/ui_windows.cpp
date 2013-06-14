#include "wrapper.h"

/* public */

window window_main = 0;

/* local */

static HINSTANCE  application_instance = 0;
static SolidBrush cgdi_brush (Color (122 /*A*/, 255 /*R*/, 9 /*G*/, 200 /*B*/));
static Pen        cgdi_pen (Color (122 /*A*/, 200 /*R*/, 200 /*G*/, 200 /*B*/), 10);;

/* local declares */

LRESULT CALLBACK general_window_callback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);



int ui_windows_init()
{
	{
		WNDCLASS wc;

		/* register window classes */

		wc.style         = CS_HREDRAW | CS_VREDRAW; 
		wc.lpfnWndProc   = general_window_callback; 
		wc.cbClsExtra    = 0; 
		wc.cbWndExtra    = 0; 
		wc.hInstance     = application_instance; 
		wc.hIcon         = LoadIcon(application_instance, (LPCTSTR)1); 
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW); 
		wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0,0)); 
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = uni("resamplestudio_wmain");

		if (!RegisterClass(&wc)) 
	    	return -1; 

		wc.lpfnWndProc   = general_window_callback;
		wc.lpszClassName = uni("resamplestudio_wtabbed");

		if(!RegisterClass(&wc)) 
	    	return -1; 
    }

    /* gdi stuff */

    {
    	//cgdi_pen = new Pen(Color (122 /*A*/, 200 /*R*/, 200 /*G*/, 200 /*B*/), 20);
    	//cgdi_brush = new SolidBrush(Color (122 /*A*/, 200 /*R*/, 200 /*G*/, 200 /*B*/));
    }

    return 0;
}



int ui_window_create(window *wind, window owner, int wtype, string title, box *pos, int maxmin, int monitor, void* callback)
{
	HWND hwnd = 0;
	int  mvalue = 0;

	switch(maxmin)
	{
	case v_ui_window_show_maximized: mvalue = SW_MAXIMIZE; break;
	case v_ui_window_show_minimized: mvalue = SW_MINIMIZE; break;
	case v_ui_window_show_default: mvalue = SW_SHOW; break;
	}

	switch(wtype)
	{
	case v_ui_window_type_main: 
		hwnd = CreateWindow(uni("resamplestudio_wmain"), title, WS_OVERLAPPEDWINDOW, pos->x, pos->y, pos->w, pos->h, NULL,NULL,application_instance,NULL); 
		break;

	case v_ui_window_type_child: 
		hwnd = CreateWindow(uni("resamplestudio_wmain"), title, WS_CHILD, pos->x, pos->y, pos->w, pos->h, owner,NULL,application_instance,NULL); 
		break;

	case v_ui_window_type_tabbed: 
		if(owner)
		{
			hwnd = CreateWindow(uni("resamplestudio_wtabbed"), title, WS_CHILD, pos->x, pos->y, pos->w, pos->h, owner,NULL,application_instance,NULL); 
		}else{
			hwnd = CreateWindow(uni("resamplestudio_wtabbed"), title, WS_OVERLAPPEDWINDOW, pos->x, pos->y, pos->w, pos->h, NULL,NULL,application_instance,NULL); 
		}
		break;

	case v_ui_window_type_popup: 
		hwnd = CreateWindow(uni("resamplestudio_wmain"), title, WS_OVERLAPPEDWINDOW, pos->x, pos->y, pos->w, pos->h, owner,NULL,application_instance,NULL); 
		break;
	}
	
    if(wind)
    	*wind = hwnd;

	if (!hwnd) 
		return 0; 

	if(mvalue)
	{
		ShowWindow(hwnd, mvalue); 
		UpdateWindow(hwnd); 
	}

	return 0;
}


int ui_window_set_title(window wind, string title)
{
	return (int)!SetWindowText(wind, title);
}

int ui_window_set_menu(window hwin, menu hmenu)
{
	return (int)!SetMenu(hwin, hmenu);
}

int ui_window_get_position(window hwin, box *pos)
{
	RECT rct;
	GetWindowRect(hwin, &rct);
	pos->x = rct.left;
	pos->y = rct.top;
	pos->w = rct.right - rct.left;
	pos->h = rct.bottom - rct.top;
}




/* menus -------------------------------------------------------------------------------------- */

menu ui_menu_create()
{
	return CreateMenu();
}

int ui_menu_delete(menu hmenu)
{
	BOOL rs = FALSE;

	rs = DestroyMenu(hmenu);

	if(!rs) return -1;
	return 0;
}

int ui_menu_add_item()
{

}

int ui_menu_remove_item()
{

}

int ui_menu_enable_item()
{

}

int ui_menu_check_item()
{

}







/* shapes ------------------------------------------------------------------------------------- */

int ui_set_drawing_mode(graphic_context *gc, color fillcolor, color bordercolor, int bordersize)
{
	return 0;
}

/*
 * - a '/' and a file name will load a local file as a font.
 */
int ui_set_text_mode(graphic_context *gc, string fontname, int weight, int itallic, int underline)
{
	return 0;
}

int ui_set_text_color(graphic_context *gc, color fillcolor)
{
	return 0;
}


int ui_shape_draw_circle(graphic_context *gc, box *position)
{
	return 0;
}


int ui_shape_draw_arc(graphic_context *gc, box *position, int sang, int eang)
{
	return 0;
}


int ui_shape_draw_polygon(graphic_context &gc, color c, point *points, int count)
{
	int i;

	SolidBrush brush (Color (color_get_a(c), color_get_r(c), color_get_g(c), color_get_b(c)));

	PointF *poly = new PointF[count];

	for(i=0; i<count; i++)
	{
		poly[i] = PointF(points[i].x, points[i].y);
	}

    gc.FillPolygon(&brush, poly, count);

    delete poly;
	return 0;
}


int ui_shape_draw_rect(graphic_context *gc, box *pos)
{
    gc->FillRectangle (&cgdi_brush, pos->x, pos->y, pos->w, pos->h);
	return 0;
}


int ui_shape_draw_rect(graphic_context &gc, color c, int x, int y, int w, int h)
{
	SolidBrush brush2 (Color (color_get_a(c), color_get_r(c), color_get_g(c), color_get_b(c)));
	gc.FillRectangle (&brush2, x, y, w, h);
	//gc.FillRectangle (&cgdi_brush, x, y, w, h);
	return 0;
}



int ui_shape_draw_line(graphic_context &gc, int size, color c, int x1, int y1, int x2, int y2)
{
	Pen blackPen(Color (color_get_a(c), color_get_r(c), color_get_g(c), color_get_b(c)), size);
	gc.DrawLine(&blackPen, x1, y1, x2, y2);
	return 0;
}


int ui_shape_draw_point(graphic_context *gc, point *pt)
{
	return 0;
}


int ui_shape_draw_bezier(graphic_context *gc, pointex *pt)
{
	return 0;
}


int ui_shape_draw_cardinal(graphic_context *gc, pointex *pt)
{
	return 0;
}



int ui_text_draw(graphic_context &gc, string text, color c, int x, int y, int mode)
{
	const FontFamily &cgdi_fm = FontFamily(L"Roboto");
	static Font       cgdi_font(&cgdi_fm, 8, FontStyleBold);

	SolidBrush brush (Color (color_get_a(c), color_get_r(c), color_get_g(c), color_get_b(c)));
	gc.DrawString(text, -1, &cgdi_font, PointF(x, y), &brush);
	return 0;
}

int ui_text_draw(graphic_context *gc, string text, color c, box *pos, int mode)
{
	
	return 0;
}

int ui_text_getwidth(graphic_context &gc, string text)
{
	const  FontFamily &cgdi_fm = FontFamily(L"Roboto");
	static Font       cgdi_font(&cgdi_fm, 8, FontStyleBold);

	RectF brect;

	gc.MeasureString(text, -1, &cgdi_font, PointF(0, 0), &brect);
	return brect.Width;
}




int ui_wrapper_set_callback()
{
	return 0;
}





/*
	following messages will be sent to system independant 
	callback functions:

		1. init
		2. resize
		3. mouse (down, up, move)
		4. key (down, up, move)
		5. [controller]
		6. paint
		7. drop_file
		8. drop_file_over (hover)
		9. mouse wheel
		A. [touch] (slide, spin, pinch)
		B. close

*/


LRESULT CALLBACK general_window_callback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{


}
