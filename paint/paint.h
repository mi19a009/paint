/* Copyright (C) 2025 Taichi Murakami. */
#pragma once
#include <gtk/gtk.h>
#define PAINT_RESOURCE_PATH_CCH    64
#define PAINT_TYPE_APPLICATION     (paint_application_get_type     ())
#define PAINT_TYPE_CANVAS          (paint_canvas_get_type          ())
#define PAINT_TYPE_DOCUMENT_WINDOW (paint_document_window_get_type ())

G_DECLARE_FINAL_TYPE (PaintApplication,    paint_application,     PAINT, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE (PaintCanvas,         paint_canvas,          PAINT, CANVAS,          GtkGrid);
G_DECLARE_FINAL_TYPE (PaintDocumentWindow, paint_document_window, PAINT, DOCUMENT_WINDOW, GtkApplicationWindow);

/*******************************************************************************
* Paint モジュール:
*/
GResource *paint_get_resource      (void);
int        paint_get_resource_path (char *buffer, size_t maxlen, const char *name);
GSettings *paint_get_settings      (void);

/*******************************************************************************
* Paint Canvas モジュール:
*/
GtkWidget *paint_canvas_get_area       (PaintCanvas *self);
GtkWidget *paint_canvas_get_hscrollbar (PaintCanvas *self);
GtkWidget *paint_canvas_get_vscrollbar (PaintCanvas *self);
GtkWidget *paint_canvas_new            (void);

/*******************************************************************************
* Paint Document Window モジュール:
*/
GtkWidget *paint_document_window_new (GApplication *application);
