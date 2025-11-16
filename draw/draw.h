/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#define DRAW_RESOURCE_PATH_CCH    64
#define DRAW_TYPE_APPLICATION     (draw_application_get_type     ())
#define DRAW_TYPE_DOCUMENT_WINDOW (draw_document_window_get_type ())

G_DECLARE_FINAL_TYPE (DrawApplication,    draw_application,     DRAW, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE (DrawDocumentWindow, draw_document_window, DRAW, DOCUMENT_WINDOW, GtkApplicationWindow);

/*******************************************************************************
* Draw モジュール:
*/
GResource *draw_get_resource      (void);
int        draw_get_resource_path (char *buffer, size_t maxlen, const char *name);
GSettings *draw_get_settings      (void);

/*******************************************************************************
* Draw Document Window モジュール:
*/
GtkWidget *draw_document_window_new (GApplication *application);
