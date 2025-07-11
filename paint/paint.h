/* Copyright (C) 2025 Taichi Murakami.
アプリケーションの GUI 機能を宣言する。
このヘッダー ファイルは GTK をインクルードする。
*/
#pragma once
#include <gtk/gtk.h>
#define PAINT_APPLICATION_ID "com.github.mi19a009.paint"
#define PAINT_ZOOM_DEFAULT_VALUE 100
#define PAINT_RESOURCE_PATH_CCH 64
#define CHECK_PROPERTY(value, PROPERTY) ((PROPERTY ## _MINIMUM <= (value)) && ((value) <= PROPERTY ## _MAXIMUM))
#define CLAMP_PROPERTY(value, PROPERTY) CLAMP ((value), PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM)
#define PAINT_PARAM_SPEC_INT(PROPERTY)                 g_param_spec_int     (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_UINT(PROPERTY)                g_param_spec_uint    (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_BOOLEAN(PROPERTY)             g_param_spec_boolean (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                             PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_STRING(PROPERTY)              g_param_spec_string  (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                             PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_ENUM(PROPERTY, enum_type)     g_param_spec_enum    (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, (enum_type),                                PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_BOXED(PROPERTY, boxed_type)   g_param_spec_boxed   (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, (boxed_type),                                                           PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_OBJECT(PROPERTY, object_type) g_param_spec_object  (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, (object_type),                                                          PROPERTY ## _FLAGS)

typedef struct _PaintDocument       PaintDocument;
typedef struct _PaintEditorWindow   PaintEditorWindow;
typedef struct _PaintFrame          PaintFrame;
typedef struct _PaintLayer          PaintLayer;
typedef struct _PaintMain           PaintMain;
typedef struct _PaintMemento        PaintMemento;
typedef struct _PaintScrolledWindow PaintScrolledWindow;
typedef struct _PaintStatusbar      PaintStatusbar;
typedef struct _PaintTool           PaintTool;
typedef struct _PaintToolbar        PaintToolbar;
typedef struct _PaintToolClass      PaintToolClass;
typedef struct _PaintToolPencil     PaintToolPencil;
typedef struct _PaintViewerWindow   PaintViewerWindow;
typedef enum   _PaintVisibility     PaintVisibility;

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
	cairo_surface_t *surface;
	PaintLayer *layer;
};

struct _PaintToolClass
{
	GObjectClass parent_class;
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
GFile *
paint_document_get_file (PaintDocument *document);
GtkAdjustment *
paint_document_get_hadjustment (PaintDocument *document);
gboolean
paint_document_get_hflip (PaintDocument *document);
gint
paint_document_get_rotation (PaintDocument *document);
GtkAdjustment *
paint_document_get_vadjustment (PaintDocument *document);
gboolean
paint_document_get_vflip (PaintDocument *document);
guint
paint_document_get_zoom (PaintDocument *document);
GtkWidget *
paint_document_new (void);
void
paint_document_reset (PaintDocument *document);
void
paint_document_save (PaintDocument *document, const GFile *file);
void
paint_document_set_file (PaintDocument *document, GFile *file);
void
paint_document_set_hflip (PaintDocument *document, gboolean hflip);
void
paint_document_set_rotation (PaintDocument *document, gint rotation);
void
paint_document_set_vflip (PaintDocument *document, gboolean vflip);
void
paint_document_set_zoom (PaintDocument *document, guint zoom);
void
paint_editor_window_load (PaintEditorWindow *editor, GFile *file);
GtkWidget *
paint_editor_window_new (GApplication *application);
void
paint_error_dialog_show (GtkWindow *parent, GError *error);
GListModel *
paint_file_filter_list_new (void);
int
paint_layer_get_height (PaintLayer *layer);
unsigned
paint_layer_get_n_planes (PaintLayer *layer);
const char *
paint_layer_get_name (PaintLayer *layer);
cairo_surface_t *
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
paint_layer_reset (PaintLayer *layer, cairo_surface_t *surface, unsigned n_planes);
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
GtkWidget *
paint_scrolled_window_get_child (PaintScrolledWindow *scrolled);
GtkWidget *
paint_scrolled_window_new (void);
void
paint_scrolled_window_set_child (PaintScrolledWindow *scrolled, GtkWidget *child);
cairo_surface_t *
paint_surface_new_from_file (GFile *file, cairo_t *context, GError **error);

G_DECLARE_FINAL_TYPE (PaintDocument,       paint_document,        PAINT, DOCUMENT,        GtkDrawingArea);
G_DECLARE_FINAL_TYPE (PaintEditorWindow,   paint_editor_window,   PAINT, EDITOR_WINDOW,   GtkApplicationWindow);
G_DECLARE_FINAL_TYPE (PaintFrame,          paint_frame,           PAINT, FRAME,           GtkApplicationWindow);
G_DECLARE_FINAL_TYPE (PaintLayer,          paint_layer,           PAINT, LAYER,           GObject);
G_DECLARE_FINAL_TYPE (PaintMain,           paint_main,            PAINT, MAIN,            GtkApplication);
G_DECLARE_FINAL_TYPE (PaintScrolledWindow, paint_scrolled_window, PAINT, SCROLLED_WINDOW, GtkGrid);
G_DECLARE_FINAL_TYPE (PaintStatusbar,      paint_statusbar,       PAINT, STATUSBAR,       GtkBox);
G_DECLARE_FINAL_TYPE (PaintToolbar,        paint_toolbar,         PAINT, TOOLBAR,         GtkBox);

GType paint_surface_get_type    (void);
GType paint_visibility_get_type (void);

#define PAINT_TYPE_DOCUMENT        paint_document_get_type        ()
#define PAINT_TYPE_EDITOR_WINDOW   paint_editor_window_get_type   ()
#define PAINT_TYPE_FRAME           paint_frame_get_type           ()
#define PAINT_TYPE_LAYER           paint_layer_get_type           ()
#define PAINT_TYPE_MAIN            paint_main_get_type            ()
#define PAINT_TYPE_SCROLLED_WINDOW paint_scrolled_window_get_type ()
#define PAINT_TYPE_SURFACE         paint_surface_get_type         ()
#define PAINT_TYPE_VISIBILITY      paint_visibility_get_type      ()
