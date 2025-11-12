/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#define VIEWER_RESOURCE_PATH_CCH    64
#define VIEWER_TYPE_APPLICATION     (viewer_application_get_type     ())
#define VIEWER_TYPE_CANVAS          (viewer_canvas_get_type          ())
#define VIEWER_TYPE_DOCUMENT_WINDOW (viewer_document_window_get_type ())
#define OBJECT_CLASS_INSTALL_PROPERTY_BOXED(THIS, PROPERTY)  (g_object_class_install_property ((THIS), (PROPERTY ## _ID), g_param_spec_boxed  ((PROPERTY ## _NAME), (PROPERTY ## _NICK), (PROPERTY ## _BLURB), (PROPERTY ## _BOXED_TYPE),  (PROPERTY ## _FLAGS))))
#define OBJECT_CLASS_INSTALL_PROPERTY_OBJECT(THIS, PROPERTY) (g_object_class_install_property ((THIS), (PROPERTY ## _ID), g_param_spec_object ((PROPERTY ## _NAME), (PROPERTY ## _NICK), (PROPERTY ## _BLURB), (PROPERTY ## _OBJECT_TYPE), (PROPERTY ## _FLAGS))))
#define OBJECT_REF(obj)                                      ((obj) ? (g_object_ref (obj)) : NULL)

G_DECLARE_FINAL_TYPE (ViewerApplication,    viewer_application,     VIEWER, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerCanvas,         viewer_canvas,          VIEWER, CANVAS,          GtkGrid);
G_DECLARE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, VIEWER, DOCUMENT_WINDOW, GtkApplicationWindow);

/* Viewer モジュール */
void       viewer_choose_file       (GtkWindow *parent, GFile *initial_file, GAsyncReadyCallback callback, gpointer user_data);
GResource *viewer_get_resource      (void);
int        viewer_get_resource_path (char *buffer, size_t maxlen, const char *name);
GSettings *viewer_get_settings      (void);
/* Viewer Application モジュール */
GApplication *viewer_application_new (const char *application_id, GApplicationFlags flags);
/* Viewer Canvas モジュール */
GtkWidget *viewer_canvas_get_area       (ViewerCanvas *self);
GtkWidget *viewer_canvas_get_hscrollbar (ViewerCanvas *self);
GdkPixbuf *viewer_canvas_get_pixbuf     (ViewerCanvas *self);
GdkRGBA   *viewer_canvas_get_rgba       (ViewerCanvas *self);
GtkWidget *viewer_canvas_get_vscrollbar (ViewerCanvas *self);
GtkWidget *viewer_canvas_new            (void);
void       viewer_canvas_set_pixbuf     (ViewerCanvas *self, GdkPixbuf *pixbuf);
void       viewer_canvas_set_rgba       (ViewerCanvas *self, const GdkRGBA *rgba);
/* Viewer Document Window モジュール */
GFile     *viewer_document_window_get_file (ViewerDocumentWindow *self);
GtkWidget *viewer_document_window_new      (GApplication *application);
void       viewer_document_window_set_file (ViewerDocumentWindow *self, GFile *file);
