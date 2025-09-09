/* Copyright (C) 2025 Taichi Murakami. */
#pragma once
#include <gtk/gtk.h>
#define PAINT_RESOURCE_PATH_CCH 64
#define PAINT_TYPE_APPLICATION (paint_application_get_type ())
#define PAINT_TYPE_SETTINGS    (paint_settings_get_type    ())
#define PAINT_TYPE_TOOL        (paint_tool_get_type        ())
#define PAINT_TYPE_TOOL_ERASER (paint_tool_eraser_get_type ())
#define PAINT_TYPE_TOOL_PENCIL (paint_tool_pencil_get_type ())
#define PAINT_TYPE_WINDOW      (paint_window_get_type      ())
#define PAINT_OBJECT_CLASS_INSTALL_PROPERTY(object_class, PROPERTY, PSPEC) (g_object_class_install_property ((object_class), (PROPERTY), PSPEC (PROPERTY)))
#define PAINT_PARAM_SPEC_BOOLEAN(PROPERTY) g_param_spec_boolean (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                             PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_BOXED(PROPERTY)   g_param_spec_boxed   (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _BOXED_TYPE,                                                PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_DOUBLE(PROPERTY)  g_param_spec_double  (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_ENUM(PROPERTY)    g_param_spec_enum    (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _ENUM_TYPE,                     PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_FLAGS(PROPERTY)   g_param_spec_flags   (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _FLAGS_TYPE,                    PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_FLOAT(PROPERTY)   g_param_spec_float   (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_INT(PROPERTY)     g_param_spec_int     (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_OBJECT(PROPERTY)  g_param_spec_object  (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _OBJECT_TYPE,                                               PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_POINTER(PROPERTY) g_param_spec_pointer (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                                                         PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_STRING(PROPERTY)  g_param_spec_string  (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                             PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_UINT(PROPERTY)    g_param_spec_uint    (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)

/* ペイント ツール */
struct _PaintToolClass
{
	GObjectClass parent_class;
	const char  *icon_name;
	const char  *name;
	const char  *nick;
};

G_DECLARE_FINAL_TYPE     (PaintApplication, paint_application, PAINT, APPLICATION, GtkApplication);
G_DECLARE_DERIVABLE_TYPE (PaintTool,        paint_tool,        PAINT, TOOL,        GObject);
G_DECLARE_FINAL_TYPE     (PaintToolEraser,  paint_tool_eraser, PAINT, TOOL_ERASER, PaintTool);
G_DECLARE_FINAL_TYPE     (PaintToolPencil,  paint_tool_pencil, PAINT, TOOL_PENCIL, PaintTool);
G_DECLARE_FINAL_TYPE     (PaintWindow,      paint_window,      PAINT, WINDOW,      GtkApplicationWindow);

/* Paint モジュール */
GResource       *paint_get_resource             (void);
cairo_surface_t *paint_surface_create_from_file (cairo_surface_t *other, GFile *file);

/* Paint Tool クラス */
GdkRGBA    *paint_tool_get_color     (PaintTool *self);
const char *paint_tool_get_icon_name (PaintTool *self);
const char *paint_tool_get_name      (PaintTool *self);
const char *paint_tool_get_nick      (PaintTool *self);
float       paint_tool_get_width     (PaintTool *self);
void        paint_tool_load          (PaintTool *self, PaintTool *other);
void        paint_tool_set_color     (PaintTool *self, const GdkRGBA *color);
void        paint_tool_set_width     (PaintTool *self, float width);

/* Paint Tool Eraser クラス */
PaintTool *paint_tool_eraser_new (void);

/* Paint Tool Pencil クラス */
PaintTool *paint_tool_pencil_new (void);

/* Paint Window クラス */
GFile      *paint_window_get_file (PaintWindow *self);
PaintTool  *paint_window_get_tool (PaintWindow *self);
GtkWidget  *paint_window_new      (GApplication *application, gboolean show_menubar, GFile *file);
void        paint_window_set_file (PaintWindow *self, GFile *file);
void        paint_window_set_tool (PaintWindow *self, PaintTool *tool);
