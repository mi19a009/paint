/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#define VIEWER_RESOURCE_PATH_CCH    64
#define VIEWER_TYPE_APPLICATION     (viewer_application_get_type     ())
#define VIEWER_TYPE_DOCUMENT_WINDOW (viewer_document_window_get_type ())

G_DECLARE_FINAL_TYPE (ViewerApplication,    viewer_application,     VIEWER, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, VIEWER, DOCUMENT_WINDOW, GtkApplicationWindow);

/* Viewer */
GResource    *viewer_get_resource        (void);
int           viewer_get_resource_path   (char *buffer, size_t maxlen, const char *name);
GSettings    *viewer_get_settings        (void);
/* Viewer Application */
GApplication *viewer_application_new     (const char *application_id, GApplicationFlags flags);
/* Viewer Document Window */
GtkWidget    *viewer_document_window_new (GApplication *application);
