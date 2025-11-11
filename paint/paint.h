/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#define PAINT_RESOURCE_PATH_CCH    64
#define PAINT_TYPE_APPLICATION     (paint_application_get_type     ())
#define PAINT_TYPE_DOCUMENT_WINDOW (paint_document_window_get_type ())

G_DECLARE_FINAL_TYPE (PaintApplication,    paint_application,     PAINT, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE (PaintDocumentWindow, paint_document_window, PAINT, DOCUMENT_WINDOW, GtkApplicationWindow);

/* Paint */
GResource    *paint_get_resource        (void);
int           paint_get_resource_path   (char *buffer, size_t maxlen, const char *name);
GSettings    *paint_get_settings        (void);
/* Paint Application */
GApplication *paint_application_new     (const char *application_id, GApplicationFlags flags);
/* Paint Document Window */
GtkWidget    *paint_document_window_new (GApplication *application);
