/*
 * Copyright 2025 Taichi Murakami.
 * このヘッダー ファイルはお絵かきソフトの各機能を公開します。
 */

#include <gtk/gtk.h>
#include "text.h"
#define GTK_CSS_FLAT                            "flat"
#define GTK_ICON_NEW                            "document-new-symbolic"
#define GTK_ICON_OPEN                           "document-open-symbolic"
#define GTK_ICON_PRINT                          "document-print-symbolic"
#define GTK_ICON_SAVE                           "document-save-symbolic"
#define PAINT_ACTION_ABOUT                      "about"
#define PAINT_ACTION_COPY                       "copy"
#define PAINT_ACTION_ENTRY_STATEFUL             ((const char *) -1)
#define PAINT_ACTION_FLIP_HORIZONTAL            "hflip"
#define PAINT_ACTION_FLIP_VERTICAL              "vflip"
#define PAINT_ACTION_MODE                       "mode"
#define PAINT_ACTION_NEW                        "new"
#define PAINT_ACTION_OPEN                       "open"
#define PAINT_ACTION_PRINT                      "print"
#define PAINT_ACTION_QUIT                       "quit"
#define PAINT_ACTION_SAVE                       "save"
#define PAINT_ACTION_SAVE_AS                    "save-as"
#define PAINT_ACTION_UNDO                       "undo"
#define PAINT_BUILDER_MENUBAR                   "menu"
#define PAINT_CCH_VERSION                       32
#define PAINT_COMMAND_ABOUT                     "win.about"
#define PAINT_COMMAND_CLOSE                     "window.close"
#define PAINT_COMMAND_COPY                      "win.copy"
#define PAINT_COMMAND_CUT                       "win.cut"
#define PAINT_COMMAND_FLIP_HORIZONTAL           "win.hflip"
#define PAINT_COMMAND_FLIP_VERTICAL             "win.vflip"
#define PAINT_COMMAND_MODE                      "win.mode"
#define PAINT_COMMAND_NEW                       "app.new"
#define PAINT_COMMAND_OPEN                      "win.open"
#define PAINT_COMMAND_PASTE                     "win.paste"
#define PAINT_COMMAND_PRINT                     "win.print"
#define PAINT_COMMAND_REDO                      "win.redo"
#define PAINT_COMMAND_SAVE                      "win.save"
#define PAINT_COMMAND_SAVE_AS                   "win.save-as"
#define PAINT_COMMAND_SELECT_ALL                "win.select-all"
#define PAINT_COMMAND_UNDO                      "win.undo"
#define PAINT_FORMAT_VERSION                    "GTK %u.%u.%u"
#define PAINT_LOGO_ICON_NAME                    "application-x-executable"
#define PAINT_MODE_BRUSH                        "brush"
#define PAINT_MODE_ERASER                       "eraser"
#define PAINT_MODE_PENCIL                       "pencil"
#define PAINT_PROPERTY_APPLICATION_ID           "application-id"
#define PAINT_PROPERTY_FLAGS                    "flags"
#define PAINT_RESOURCE_MENUBAR                  "menubar.xml"
#define PAINT_RESOURCE_PREFIX                   "/com/github/mi19a009/paint/resource/"
#define PAINT_RESOURCE_TOOLBAR                  "toolbar.ui"
#define PAINT_SIGNAL_ACTIVATE                   "activate"
#define PAINT_SIGNAL_CLICKED                    "clicked"
#define PAINT_SIGNAL_DESTROY                    "destroy"

G_DECLARE_FINAL_TYPE (PaintApplication,       paint_application,        PAINT, APPLICATION,        GtkApplication);
G_DECLARE_FINAL_TYPE (PaintApplicationWindow, paint_application_window, PAINT, APPLICATION_WINDOW, GtkApplicationWindow);
G_DECLARE_FINAL_TYPE (PaintColorWidget,       paint_color_widget,       PAINT, COLOR_WIDGET,       GtkWidget);
G_DECLARE_FINAL_TYPE (PaintLayerWidget,       paint_layer_widget,       PAINT, LAYER_WIDGET,       GtkWidget);
G_DECLARE_FINAL_TYPE (PaintSettings,          paint_settings_window,    PAINT, SETTINGS,           GObject);
G_DECLARE_FINAL_TYPE (PaintToolWindow,        paint_tool_window,        PAINT, TOOL_WINDOW,        GtkWindow);

#define PAINT_TYPE_APPLICATION        paint_application_get_type        ()
#define PAINT_TYPE_APPLICATION_WINDOW paint_application_window_get_type ()
#define PAINT_TYPE_COLOR_WIDGET       paint_color_widget_get_type       ()
#define PAINT_TYPE_LAYER_WIDGET       paint_layer_widget_get_type       ()
#define PAINT_TYPE_SETTINGS           paint_settings_get_type           ()
#define PAINT_TYPE_TOOL_WINDOW        paint_tool_window_get_type        ()

typedef struct _PaintAccelEntry    PaintAccelEntry;
typedef struct _PaintActionEntry   PaintActionEntry;
typedef struct _PaintFileFilter    PaintFileFilter;

GtkWidget      *paint_about_dialog_new                         (GtkWindow *parent);
void            paint_action_map_add_action_entries            (GActionMap *action_map, const PaintActionEntry *entries, gint n_entries, gpointer user_data);
PaintSettings  *paint_application_get_settings                 (PaintApplication *application);
GApplication   *paint_application_new                          (void);
GtkWidget      *paint_application_window_new                   (GApplication *application);
GtkWidget      *paint_application_window_new_from_file         (GApplication *application, const char *filename);
void            paint_application_set_accels_for_action        (GtkApplication *application, const PaintAccelEntry *entries, gint n_entries);
GtkBuilder     *paint_builder_new_from_resource                (const char *resource_name);
void            paint_file_dialog_open                         (GtkFileDialog *self, GtkWindow *parent, GCancellable *cancellable, GAsyncReadyCallback *callback, gpointer user_data);
GListModel     *paint_file_filter_list_new                     (const PaintFileFilter *entries, int n_entries);
GtkFileFilter  *paint_file_filter_new                          (const PaintFileFilter *entry);
GMenuModel     *paint_menu_new_menubar                         (void);
gchar          *paint_resource_get_path                        (const char *resource_name);
void            paint_settings_get_window_position             (PaintSettings *settings, int *x, int *y);
void            paint_settings_get_window_size                 (PaintSettings *settings, int *width, int *height);
PaintSettings  *paint_settings_new                             (void);
void            paint_settings_read_file                       (PaintSettings *settings);
void            paint_settings_set_window_position             (PaintSettings *settings, int x, int y);
void            paint_settings_set_window_size                 (PaintSettings *settings, int width, int height);
void            paint_settings_wirte_file                      (PaintSettings *settings);
//GtkWidget      *paint_statusbar_new                            (void);
GtkWidget      *paint_tool_window_new                          (void);
GtkWidget      *paint_tool_window_new_from_resource            (GtkWindow *parent, const char *resource_name);
GtkWidget      *paint_tool_window_get_content                  (PaintToolWindow *window);
void            paint_tool_window_set_content                  (PaintToolWindow *window, GtkWidget *content);
GtkWidget      *paint_toolbar_new                              (void);

/**
 * メニュー項目とキーボード ショートカットのペア。
 */
struct _PaintAccelEntry
{
	const char *detailed_action_name;
	const char *accel;
};

/**
 * メニュー項目とコールバック関数のペア。
 */
struct _PaintActionEntry
{
	const char *name;
	void (* activate) (GSimpleAction *action, GVariant *parameter, gpointer user_data);
	const char *state;
};

/**
 * フィルターの名前と書式のペア。
 */
struct _PaintFileFilter
{
	const char *name;
	const char *const *patterns;
};

extern const char *paint_application_authors [];
extern const char *paint_application_copyright;
extern const char *paint_application_id;
extern const char *paint_application_name;
extern const char *paint_application_website;
