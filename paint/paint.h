/* Copyright (C) 2025 Taichi Murakami. */
#pragma once
#define PAINT_RESOURCE_PATH_CCH 64
#include <gtk/gtk.h>

typedef guint                     PaintColor;
typedef struct _PaintCommand      PaintCommand;
typedef struct _PaintCommandClass PaintCommandClass;
typedef enum   _PaintCommandType  PaintCommandType;
typedef struct _PaintPoint        PaintPoint;
typedef void (*PaintCommandExecuteFunc) (PaintCommand *self, cairo_t *cairo);
typedef void (*PaintCommandUpdateFunc)  (PaintCommand *self, int x, int y);

/* コマンド */
enum _PaintCommandType
{
	PAINT_COMMAND_TYPE_NULL,
	PAINT_COMMAND_TYPE_CLEAR,
	PAINT_COMMAND_TYPE_DRAW,
	PAINT_COMMAND_TYPE_ERASE,
	PAINT_COMMAND_TYPE_FILL,
	PAINT_COMMAND_TYPE_PASTE,
};

/* コマンド */
struct _PaintCommandClass
{
	GObjectClass            parent_class;
	PaintCommandExecuteFunc execute;
	PaintCommandUpdateFunc  update;
	PaintCommandType        type;
};

/* 座標 */
struct _PaintPoint
{
	int x;
	int y;
};

G_DECLARE_FINAL_TYPE     (PaintApplication,    paint_application,     PAINT, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE     (PaintCanvas,         paint_canvas,          PAINT, CANVAS,          GtkGrid);
G_DECLARE_DERIVABLE_TYPE (PaintCommand,        paint_command,         PAINT, COMMAND,         GObject);
G_DECLARE_FINAL_TYPE     (PaintCommandClear,   paint_command_clear,   PAINT, COMMAND_CLEAR,   PaintCommand);
G_DECLARE_FINAL_TYPE     (PaintCommandDraw,    paint_command_draw,    PAINT, COMMAND_DRAW,    PaintCommand);
G_DECLARE_FINAL_TYPE     (PaintCommandErase,   paint_command_erase,   PAINT, COMMAND_ERASE,   PaintCommand);
G_DECLARE_FINAL_TYPE     (PaintCommandFill,    paint_command_fill,    PAINT, COMMAND_FILL,    PaintCommand);
G_DECLARE_FINAL_TYPE     (PaintCommandPaste,   paint_command_paste,   PAINT, COMMAND_PASTE,   PaintCommand);
G_DECLARE_FINAL_TYPE     (PaintDocumentWindow, paint_document_window, PAINT, DOCUMENT_WINDOW, GtkApplicationWindow);

#define PAINT_TYPE_APPLICATION     (paint_application_get_type     ())
#define PAINT_TYPE_CANVAS          (paint_canvas_get_type          ())
#define PAINT_TYPE_COMMAND         (paint_command_get_type         ())
#define PAINT_TYPE_COMMAND_CLEAR   (paint_command_clear_get_type   ())
#define PAINT_TYPE_COMMAND_DRAW    (paint_command_draw_get_type    ())
#define PAINT_TYPE_COMMAND_ERASE   (paint_command_erase_get_type   ())
#define PAINT_TYPE_COMMAND_FILL    (paint_command_fill_get_type    ())
#define PAINT_TYPE_COMMAND_PASTE   (paint_command_paste_get_type   ())
#define PAINT_TYPE_DOCUMENT_WINDOW (paint_document_window_get_type ())

/* Paint モジュール */
GResource *paint_get_resource      (void);
int        paint_get_resource_path (char *buffer, size_t maxlen, const char *name);
GSettings *paint_get_settings      (void);

/* Paint Canvas クラス */
gboolean         paint_canvas_get_antialias      (PaintCanvas *self);
PaintColor       paint_canvas_get_color          (PaintCanvas *self);
PaintCommandType paint_canvas_get_command_type   (PaintCanvas *self);
GListModel      *paint_canvas_get_commands       (PaintCanvas *self);
int              paint_canvas_get_content_height (PaintCanvas *self);
int              paint_canvas_get_content_width  (PaintCanvas *self);
int              paint_canvas_get_line_width     (PaintCanvas *self);
void             paint_canvas_get_point          (PaintCanvas *self, int *x, int *y);
int              paint_canvas_get_surface_height (PaintCanvas *self);
int              paint_canvas_get_surface_width  (PaintCanvas *self);
int              paint_canvas_get_zoom           (PaintCanvas *self);
double           paint_canvas_get_zoom_percent   (PaintCanvas *self);
void             paint_canvas_load               (PaintCanvas *self, GdkPixbuf *source);
GtkWidget       *paint_canvas_new                (void);
void             paint_canvas_resize             (PaintCanvas *self, int width, int height);
void             paint_canvas_set_antialias      (PaintCanvas *self, gboolean antialias);
void             paint_canvas_set_color          (PaintCanvas *self, PaintColor color);
void             paint_canvas_set_command_type   (PaintCanvas *self, PaintCommandType type);
void             paint_canvas_set_content_height (PaintCanvas *self, int height);
void             paint_canvas_set_content_width  (PaintCanvas *self, int width);
void             paint_canvas_set_line_width     (PaintCanvas *self, int width);
void             paint_canvas_set_zoom           (PaintCanvas *self, int zoom);
void             paint_canvas_set_zoom_percent   (PaintCanvas *self, double percent);

/* Paint Command クラス */
gboolean paint_command_get_antialias  (PaintCommand *self);
void     paint_command_execute        (PaintCommand *self, cairo_t *cairo);
void     paint_command_set_antialias  (PaintCommand *self, gboolean antialias);
void     paint_command_update         (PaintCommand *self, int x, int y);

/* Paint Command Clear クラス */
void          paint_command_clear_get_point (PaintCommandClear *self, int *x, int *y);
void          paint_command_clear_get_size  (PaintCommandClear *self, int *width, int *height);
PaintCommand *paint_command_clear_new       (void);
void          paint_command_clear_set_point (PaintCommandClear *self, int x, int y);
void          paint_command_clear_set_size  (PaintCommandClear *self, int width, int height);

/* Paint Command Draw クラス */
PaintColor    paint_command_draw_get_color      (PaintCommandDraw *self);
int           paint_command_draw_get_line_width (PaintCommandDraw *self);
PaintCommand *paint_command_draw_new            (void);
void          paint_command_draw_set_color      (PaintCommandDraw *self, PaintColor color);
void          paint_command_draw_set_line_width (PaintCommandDraw *self, int width);

/* Paint Command Erase クラス */
int           paint_command_erase_get_line_width (PaintCommandErase *self);
PaintCommand *paint_command_erase_new            (void);
void          paint_command_erase_set_line_width (PaintCommandErase *self, int width);

/* Paint Command Fill クラス */
GdkRGBA      *paint_command_fill_get_color (PaintCommandFill *self);
void          paint_command_fill_get_point (PaintCommandFill *self, int *x, int *y);
void          paint_command_fill_get_size  (PaintCommandFill *self, int *width, int *height);
PaintCommand *paint_command_fill_new (void);
void          paint_command_fill_set_color (PaintCommandFill *self, const GdkRGBA *color);
void          paint_command_fill_set_point (PaintCommandFill *self, int x, int y);
void          paint_command_fill_set_size  (PaintCommandFill *self, int width, int height);

/* Paint Command Paste クラス */
void          paint_command_paste_get_point  (PaintCommandPaste *self, int *x, int *y);
void          paint_command_paste_get_scale  (PaintCommandPaste *self, float *x, float *y);
GdkPixbuf    *paint_command_paste_get_source (PaintCommandPaste *self);
PaintCommand *paint_command_paste_new        (void);
void          paint_command_paste_set_point  (PaintCommandPaste *self, int x, int y);
void          paint_command_paste_set_scale  (PaintCommandPaste *self, float x, float y);
void          paint_command_paste_set_source (PaintCommandPaste *self, GdkPixbuf *source);

/* Paint Document Window クラス */
GFile     *paint_document_window_get_file (PaintDocumentWindow *self);
GtkWidget *paint_document_window_new      (GApplication *application);
void       paint_document_window_set_file (PaintDocumentWindow *self, GFile *file);
