/* Copyright (C) 2025 Taichi Murakami. */
#pragma once
#include <gtk/gtk.h>
#define PAINT_RESOURCE_PATH_CCH 64
#define PAINT_TYPE_APPLICATION     (paint_application_get_type     ())
#define PAINT_TYPE_SETTINGS        (paint_settings_get_type        ())
#define PAINT_TYPE_WINDOW          (paint_window_get_type          ())
#define PAINT_OBJECT_CLASS_INSTALL_PROPERTY(object, PROPERTY, PSPEC) (g_object_class_install_property ((object), (PROPERTY), PSPEC (PROPERTY)))
#define PAINT_PARAM_SPEC_BOOLEAN(PROPERTY) g_param_spec_boolean (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                             PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_BOXED(PROPERTY)   g_param_spec_boxed   (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _BOXED_TYPE,                                                PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_ENUM(PROPERTY)    g_param_spec_enum    (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _ENUM_TYPE,                     PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_FLAGS(PROPERTY)   g_param_spec_flags   (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _FLAGS_TYPE,                    PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_INT(PROPERTY)     g_param_spec_int     (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_OBJECT(PROPERTY)  g_param_spec_object  (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _OBJECT_TYPE,                                               PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_POINTER(PROPERTY) g_param_spec_pointer (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                                                         PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_STRING(PROPERTY)  g_param_spec_string  (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                             PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_UINT(PROPERTY)    g_param_spec_uint    (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)

typedef enum _PaintLanguage PaintLanguage;
typedef enum _PaintToolType PaintToolType;

/* 使用言語 */
enum _PaintLanguage
{
	PAINT_LANGUAGE_DEFAULT,
	PAINT_LANGUAGE_JAPANESE,
	PAINT_N_LANGUAGES,
};

enum _PaintToolType
{
	PAINT_TOOL_TYPE_PENCIL,
	PAINT_TOOL_TYPE_ERASER,
	PAINT_TOOL_N_TYPES,
};

G_DECLARE_FINAL_TYPE     (PaintApplication,    paint_application,      PAINT, APPLICATION,      GtkApplication);
G_DECLARE_FINAL_TYPE     (PaintTool,           paint_tool,             PAINT, TOOL,             GObject);
G_DECLARE_FINAL_TYPE     (PaintWindow,         paint_window,           PAINT, WINDOW,           GtkApplicationWindow);

/* Paint モジュール */
GResource       *paint_get_resource             (void);
int              paint_get_resource_path        (char *buffer, size_t maxlen, const char *name);
cairo_surface_t *paint_surface_create_from_file (cairo_surface_t *other, GFile *file);

/* Paint Window クラス */
GFile      *paint_window_get_file       (PaintWindow *self);
const char *paint_window_get_tool_label (PaintWindow *self);
int         paint_window_get_tool_width (PaintWindow *self);
GtkWidget  *paint_window_new            (GApplication *application, gboolean show_menubar, GFile *file);
void        paint_window_set_file       (PaintWindow *self, GFile *file);
void        paint_window_set_tool_label (PaintWindow *self, const char *value);
void        paint_window_set_tool_width (PaintWindow *self, int value);
