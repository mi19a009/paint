/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#define PAINT_RESOURCE_PATH_CCH 64

/* Paint モジュール */
GResource *paint_get_resource      (void);
int        paint_get_resource_path (char *buffer, size_t maxlen, const char *name);

/* Paint Application クラス */
GApplication *paint_application_new (const char *application_id);

/* Paint Document Window クラス */
GtkWidget *paint_document_window_new (GApplication *application);

/* Paint Application クラス */
struct _PaintApplicationClass
{
	GtkApplicationClass parent_class;
};

G_DECLARE_DERIVABLE_TYPE (PaintApplication,    paint_application,     PAINT, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE     (PaintDocumentWindow, paint_document_window, PAINT, DOCUMENT_WINDOW, GtkApplicationWindow);

#define PAINT_TYPE_APPLICATION     (paint_application_get_type     ())
#define PAINT_TYPE_DOCUMENT_WINDOW (paint_document_window_get_type ())
