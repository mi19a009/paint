/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>

void       alert   (GtkWindow *parent, const GError *error);
void       msginit (void);
void       pixcpy  (const guchar *source, guchar *destination, int width, int height, int stride);
GdkPixbuf *pixload (GFile *file, GError **error);
