/* Copyright (C) 2025 Taichi Murakami. */
#pragma once
#include <gtk/gtk.h>
#define VIEWER_RESOURCE_PATH_CCH 64

G_DECLARE_FINAL_TYPE (ViewerApplication,    viewer_application,     VIEWER, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, VIEWER, DOCUMENT_WINDOW, GtkApplicationWindow);

#define VIEWER_TYPE_APPLICATION     (viewer_application_get_type     ())
#define VIEWER_TYPE_DOCUMENT_WINDOW (viewer_document_window_get_type ())

/*******************************************************************************
* Viewer モジュール:
*/
GResource *viewer_get_resource      (void);
int        viewer_get_resource_path (char *buffer, size_t maxlen, const char *name);
GSettings *viewer_get_settings      (void);

/*******************************************************************************
* Viewer Document Window モジュール:
*/
GFile     *viewer_document_window_get_file         (ViewerDocumentWindow *self);
GdkPixbuf *viewer_document_window_get_pixbuf       (ViewerDocumentWindow *self);
int        viewer_document_window_get_zoom         (ViewerDocumentWindow *self);
float      viewer_document_window_get_zoom_percent (ViewerDocumentWindow *self);
GtkWidget *viewer_document_window_new              (GApplication *application);
void       viewer_document_window_set_file         (ViewerDocumentWindow *self, GFile *file);
void       viewer_document_window_set_pixbuf       (ViewerDocumentWindow *self, GdkPixbuf *pixbuf);
void       viewer_document_window_set_zoom         (ViewerDocumentWindow *self, int zoom);
void       viewer_document_window_set_zoom_percent (ViewerDocumentWindow *self, float zoom);
