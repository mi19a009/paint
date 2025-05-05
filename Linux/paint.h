/*
Copyright 2025 Taichi Murakami.
Paint アプリケーション。
*/

#pragma once
#include <gtk/gtk.h>

G_DECLARE_FINAL_TYPE (PaintApplication,       paint_application,        PAINT, APPLICATION,        GtkApplication);
G_DECLARE_FINAL_TYPE (PaintApplicationWindow, paint_application_window, PAINT, APPLICATION_WINDOW, GtkApplicationWindow);
G_DECLARE_FINAL_TYPE (PaintColorWindow,       paint_color_window,       PAINT, COLOR_WINDOW,       GtkWindow);
G_DECLARE_FINAL_TYPE (PaintDocumentWindow,    paint_document_window,    PAINT, DOCUMENT_WINDOW,    GtkApplicationWindow);
G_DECLARE_FINAL_TYPE (PaintLayerWindow,       paint_layer_window,       PAINT, LAYER_WINDOW,       GtkWindow);

#define PAINT_ACTION_ENTRY_STATEFUL     ((const char *) -1)
#define PAINT_TYPE_APPLICATION          paint_application_get_type ()
#define PAINT_TYPE_APPLICATION_WINDOW   paint_application_window_get_type ()
#define PAINT_TYPE_COLOR_WINDOW         paint_color_window_get_type ()
#define PAINT_TYPE_DOCUMENT_WINDOW      paint_document_window_get_type ()
#define PAINT_TYPE_LAYER_WINDOW         paint_layer_window_get_type ()

typedef struct _PaintActionEntry
{
	const char *name;
	void (* activate) (GSimpleAction *action, GVariant *parameter, gpointer user_data);
	const char *state;
} PaintActionEntry;

void paint_about_dialog_show (GtkWindow *parent);
GApplication *paint_application_new (void);
void paint_action_map_add_action_entries (GActionMap *map, const PaintActionEntry *entries);
GtkWidget *paint_application_window_new (GApplication *application);
GtkWidget *paint_color_window_new (GtkWindow *parent);
GtkWidget *paint_document_window_new (GApplication *application);
GtkWidget *paint_document_window_new_from_file (GApplication *application, const char *filename);
GtkWidget *paint_layer_window_new (GtkWindow *parent);

extern const char *paint_application_authors [];
extern const char *paint_application_copyright;
extern const char *paint_application_id;
extern const char *paint_application_name;
extern const char *paint_application_prefix;
extern const char *paint_application_website;

#define CCH_RESOURCE_NAME       64

int format_resource_name (char *buffer, size_t length, const char *name);
