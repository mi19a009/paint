/*
Copyright 2025 Taichi Murakami.
*/

#include <locale.h>
#include <math.h>
#include <gtk/gtk.h>
#define CCH_RESOURCE_NAME       64

int format_resource_name (char *buffer, size_t length, const char *name);

G_DECLARE_FINAL_TYPE (PaintApplication,       paint_application,        PAINT, APPLICATION,        GtkApplication);
G_DECLARE_FINAL_TYPE (PaintApplicationWindow, paint_application_window, PAINT, APPLICATION_WINDOW, GtkApplicationWindow);
#define PAINT_TYPE_APPLICATION paint_application_get_type ()
#define PAINT_TYPE_APPLICATION_WINDOW paint_application_window_get_type ()

GApplication *paint_application_new (void);
GtkWidget *paint_application_window_new (GApplication *application);

extern const char *paint_application_id;
extern const char *paint_application_prefix;
