#include <windows.h>
#include <gdiplus.h>
#include <assert.h>

using namespace Gdiplus;





typedef wchar_t *string;
typedef wchar_t letter;

#define uni(x) L ## x
#define str_len(s)          wcslen(s)
#define str_cpy(d, s)       wcscpy(d, s)
#define str_cat(d, s)       wcscat(d, s)
#define str_ncpy(d, s, n)   wcsncpy(d, s, n)
#define str_ncat(d, s, n)   wcsncat(d, s, n)
#define str_cmp(a, b)       wcscmp(a, b)
#define str_ncmp(a, b, n)   wcsncmp(a, b, n)
#define str_icmp(a, b)      wcsicmp(a, b)
#define str_lower(s)        wcslwr(s)
#define str_upper(s)        wcsupr(s)
#define str_inc(s)          wcsinc(s)
#define str_dec(s)          wcsdec(s)
#define str_mcpy(d, s, n)   wcsncpy(d, s, n)
#define str_mmov(d, s, n)   memmove((char*)(d), (const char*)(s), (n) * sizeof(letter))
#define str_size(s)         (wcslen(s) * sizeof(letter))
#define str_incmp(a, b, n)  _memicmp(a, b, n * sizeof(letter))
#define str_itos(i, b, n)   _itow(i, b, n)
#define str_stoi(s)         _wtoi(s)
#define str_chr(s, c)       wcschr(s, c)
#define str_rchr(s, c)      wcsrchr(s, c)
#define str_clower(c)       towlower(c)
#define str_cupper(c)       towupper(c)
#define str_ispunct(c)      iswpunct(c)


#define color_get_r(rgb)   ((BYTE) ((rgb) >> 16)) 
#define color_get_g(rgb)   ((BYTE) (((WORD) (rgb)) >> 8)) 
#define color_get_b(rgb)   ((BYTE) (rgb))
#define color_get_a(rgb)   ((BYTE) ((rgb) >> 24)) 

/* < general values > */

#define v_sys_maxpath 260

/* </ general values > */




/* < general functions > */

#define sys_pass()  (Sleep(0))
#define sys_sleep(n)(Sleep(n))
#define sys_beep()  (Beep(1000,100))
#define sys_feep()  (Beep(2000,100))

/* </ general functions > */




/* < disc i/o functions > */

/* types */

typedef HANDLE t_sys_file_handle;

/* functions */

#define sys_file_open(a, s)              (CreateFile(a, s, 0, 0, (s == v_sys_file_forread ? OPEN_EXISTING : CREATE_NEW), 0, 0))
#define sys_file_openshare(a, s)         (CreateFile(a, s, FILE_SHARE_READ, 0, (s == v_sys_file_forread ? OPEN_EXISTING : CREATE_NEW), 0, 0))
#define sys_file_openforce(a, s)         (CreateFile(a, s, 0, 0, (s == v_sys_file_forread ? OPEN_ALWAYS : CREATE_ALWAYS), 0, 0))
#define sys_file_openforceshare(a, s)    (CreateFile(a, s, FILE_SHARE_READ, 0, (s == v_sys_file_forread ? OPEN_ALWAYS : CREATE_ALWAYS), 0, 0))
#define sys_file_openbuffering(a, s)     (CreateFile(a, s, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, (s == v_sys_file_forread ? OPEN_EXISTING : CREATE_NEW), FILE_FLAG_SEQUENTIAL_SCAN, 0))
#define sys_file_openstream(a, s)        (CreateFile(a, s, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, (s == v_sys_file_forread ? OPEN_EXISTING : CREATE_NEW), 0, 0))
#define sys_file_openforcestream(a, s)   (CreateFile(a, s, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, (s == v_sys_file_forread ? OPEN_ALWAYS : CREATE_NEW), 0, 0))

#define sys_file_create(a, s)            (CreateFile(a, s, 0, 0, (s == v_sys_file_forread ? OPEN_EXISTING : CREATE_NEW), 0, 0))
#define sys_file_createshare(a, s)       (CreateFile(a, s, FILE_SHARE_READ, 0, (s == v_sys_file_forread ? OPEN_EXISTING : CREATE_NEW), 0, 0))
#define sys_file_createforce(a, s)       (CreateFile(a, s, 0, 0, (s == v_sys_file_forread ? OPEN_ALWAYS : CREATE_ALWAYS), 0, 0))
#define sys_file_createforceshare(a, s)  (CreateFile(a, s, FILE_SHARE_READ, 0, (s == v_sys_file_forread ? OPEN_ALWAYS : CREATE_ALWAYS), 0, 0))
#define sys_file_createbuffering(a, s)   (CreateFile(a, s, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, (s == v_sys_file_forread ? OPEN_EXISTING : CREATE_NEW), FILE_FLAG_SEQUENTIAL_SCAN, 0))
#define sys_file_createstream(a, s)      (CreateFile(a, s, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, (s == v_sys_file_forread ? OPEN_EXISTING : CREATE_NEW), 0, 0))
#define sys_file_createforcestream(a, s) (CreateFile(a, s, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, (s == v_sys_file_forread ? OPEN_ALWAYS : CREATE_ALWAYS), 0, 0))

#define sys_file_tell(h)                 (SetFilePointer(h, 0, 0, FILE_CURRENT))
#define sys_file_seek(h, p)              (SetFilePointer(h, p, 0, FILE_BEGIN))
#define sys_file_getsize(h)              (GetFileSize(h, 0))
#define sys_file_seteof(h)               (SetEndOfFile(h))

#define sys_file_close(h)                (CloseHandle(h) ? 1 : v_error_sys_file_close);

unsigned long sys_file_read (t_sys_file_handle h, void* a, unsigned long z);
unsigned long sys_file_write(t_sys_file_handle h, void* a, unsigned long z);

/* error codes */

#define v_error_sys_file_open   INVALID_HANDLE_VALUE
#define v_error_sys_file_create INVALID_HANDLE_VALUE
#define v_error_sys_file_read   0
#define v_error_sys_file_write  0
#define v_error_sys_file_close  0

/* flags */

#define v_sys_file_forwrite     GENERIC_WRITE
#define v_sys_file_forread      GENERIC_READ

/* </ disc i/o functions > */


/* < memory (de/re)allocation > */

#define sys_mem_alloc(a)     (malloc(a))
#define sys_mem_realloc(a, z)(realloc(a,z))
#define sys_mem_free(a)      (free(a))

/* error codes */

#define v_error_sys_mem_alloc 0

/* </ memory (de/re)allocation > */


/* user interface stuff */

typedef HWND window;
typedef HMENU menu;
typedef Graphics graphic_context;


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

typedef DWORD color;


#define v_ui_window_show_maximized 1
#define v_ui_window_show_minimized 2
#define v_ui_window_show_default   3
#define v_ui_window_show_hidden    4

#define v_ui_window_type_main      1
#define v_ui_window_type_child     2
#define v_ui_window_type_tabbed    3
#define v_ui_window_type_popup     4










/* dec -------------------------------------------------------------- */


int ui_windows_init();
int ui_window_create(window *wind, window owner, int wtype, string title, box *pos, int maxmin, int monitor, void* callback);
int ui_window_set_title(window wind, string title);
int ui_window_set_menu(window hwin, menu hmenu);
int ui_window_get_position(window hwin, box *pos);

/* menus -------------------------------------------------------------------------------------- */

menu ui_menu_create();
int ui_menu_delete(menu hmenu);
int ui_menu_add_item();
int ui_menu_remove_item();
int ui_menu_enable_item();
int ui_menu_check_item();

/* shapes ------------------------------------------------------------------------------------- */

int ui_set_drawing_mode(graphic_context *gc, color fillcolor, color bordercolor, int bordersize);
int ui_set_text_mode(graphic_context *gc, string fontname, int weight, int itallic, int underline);
int ui_set_text_color(graphic_context *gc, color fillcolor);
int ui_shape_draw_circle(graphic_context *gc, box *position);
int ui_shape_draw_arc(graphic_context *gc, box *position, int sang, int eang);
int ui_shape_draw_polygon(graphic_context &gc, color c, point *points, int count);
int ui_shape_draw_rect(graphic_context *gc, box *pos);
int ui_shape_draw_rect(graphic_context &gc, color c, int x, int y, int w, int h);
int ui_shape_draw_line(graphic_context &gc, int size, color c, int x1, int y1, int x2, int y2);
int ui_shape_draw_point(graphic_context *gc, point *pt);
int ui_shape_draw_bezier(graphic_context *gc, pointex *pt);
int ui_shape_draw_cardinal(graphic_context *gc, pointex *pt);
int ui_text_draw(graphic_context &gc, string text, color c, int x, int y, int mode);
int ui_wrapper_set_callback();
int ui_text_getwidth(graphic_context &gc, string text);