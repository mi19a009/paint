/* Copyright (C) 2025 Taichi Murakami. */
#pragma once
#ifdef G_DECLARE_FINAL_TYPE
#define PAINT_USING_GLIB
#endif /* GLIB */
#ifdef GTK_MAJOR_VERSION
#define PAINT_USING_GTK
#endif /* GTK */
#define PAINT_APPLICATION_ID "com.github.mi19a009.paint"
#define PAINT_ZOOM_DEFAULT_VALUE 100
#define PAINT_RESOURCE_PATH_CCH 64

typedef struct _GApplication      GApplication;
typedef struct _GMenuModel        GMenuModel;
typedef struct _GtkWidget         GtkWidget;
typedef struct _GtkWindow         GtkWindow;
typedef struct _PaintEditorWindow PaintEditorWindow;
typedef struct _PaintLayer        PaintLayer;
typedef struct _PaintMain         PaintMain;
typedef struct _PaintMemento      PaintMemento;
typedef struct _cairo_surface     PaintSurface;
typedef struct _PaintViewerWindow PaintViewerWindow;
typedef enum   _PaintVisibility   PaintVisibility;

/* ドキュメントを閉じるダイアログ ボックスの結果 */
enum _PaintClosingResponse
{
	PAINT_CLOSING_RESPONSE_CANCEL,
	PAINT_CLOSING_RESPONSE_DESTROY,
	PAINT_CLOSING_RESPONSE_SAVE,
	PAINT_CLOSING_RESPONSE_MAX,
};

/* レイヤーの状態 */
enum _PaintVisibility
{
	PAINT_VISIBILITY_EDIT,
	PAINT_VISIBILITY_VISIBLE,
	PAINT_VISIBILITY_HIDDEN,
};

struct _PaintMemento
{
	PaintSurface *surface;
	PaintLayer *layer;
};

extern const char *
TEXT_AUTHORS [];
extern const char *
TEXT_COPYRIGHT;
extern const char *
TEXT_VERSION;
extern const char *
TEXT_WEBSITE;

int
muldiv (int number, int numerator, int denominator);
void
paint_editor_window_load (PaintEditorWindow *editor, GFile *file);
GtkWidget *
paint_editor_window_new (GApplication *application);
GListModel *
paint_file_filter_list_new (void);
int
paint_layer_get_height (PaintLayer *layer);
unsigned
paint_layer_get_n_planes (PaintLayer *layer);
const char *
paint_layer_get_name (PaintLayer *layer);
PaintSurface *
paint_layer_get_surface (PaintLayer *layer);
PaintVisibility
paint_layer_get_visibility (PaintLayer *layer);
int
paint_layer_get_width (PaintLayer *layer);
PaintLayer *
paint_layer_new (int width, int height, unsigned n_planes);
PaintLayer *
paint_layer_new_from_file (GFile *file, unsigned n_planes);
void
paint_layer_reset (PaintLayer *layer, PaintSurface *surface, unsigned n_planes);
void
paint_layer_set_name (PaintLayer *layer, const char *name);
void
paint_layer_set_visibility (PaintLayer *layer, PaintVisibility visibility);
GApplication *
paint_main_new (void);
GMenuModel *
paint_menu_new (void);
int
paint_resource_format_path (char *resource_path, size_t resource_path_cch, const char *resource_name);
PaintSurface *
paint_surface_new_from_file (GFile *file, cairo_t *context, GError **error);

#ifdef PAINT_USING_GLIB

GType paint_surface_get_type    (void);
GType paint_visibility_get_type (void);
G_DECLARE_FINAL_TYPE (PaintLayer, paint_layer, PAINT, LAYER, GObject);

#define PAINT_TYPE_LAYER      paint_layer_get_type      ()
#define PAINT_TYPE_SURFACE    paint_surface_get_type    ()
#define PAINT_TYPE_VISIBILITY paint_visibility_get_type ()
#define CHECK_PROPERTY(value, PROPERTY) ((PROPERTY ## _MINIMUM <= (value)) && ((value) <= PROPERTY ## _MAXIMUM))
#define CLAMP_PROPERTY(value, PROPERTY) CLAMP ((value), PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM)
#define PAINT_PARAM_SPEC_INT(PROPERTY)                 g_param_spec_int     (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_UINT(PROPERTY)                g_param_spec_uint    (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_BOOLEAN(PROPERTY)             g_param_spec_boolean (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                             PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_STRING(PROPERTY)              g_param_spec_string  (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                             PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_ENUM(PROPERTY, enum_type)     g_param_spec_enum    (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, (enum_type),                                PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_BOXED(PROPERTY, boxed_type)   g_param_spec_boxed   (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, (boxed_type),                                                           PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_OBJECT(PROPERTY, object_type) g_param_spec_object  (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, (object_type),                                                          PROPERTY ## _FLAGS)
#endif /* GLIB */
#ifdef PAINT_USING_GTK

G_DECLARE_FINAL_TYPE (PaintEditorWindow, paint_editor_window, PAINT, EDITOR_WINDOW, GtkApplicationWindow);
G_DECLARE_FINAL_TYPE (PaintMain,         paint_main,          PAINT, MAIN,          GtkApplication);

#define PAINT_TYPE_MAIN          paint_main_get_type          ()
#define PAINT_TYPE_EDITOR_WINDOW paint_editor_window_get_type ()
#endif /* GTK */
