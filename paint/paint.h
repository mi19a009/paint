/* Copyright (C) 2025 Taichi Murakami. */
#pragma once
#define PAINT_APPLICATION_ID "com.github.mi19a009.paint"

typedef struct _GApplication      GApplication;
typedef struct _GMenuModel        GMenuModel;
typedef struct _GtkWidget         GtkWidget;
typedef struct _GtkWindow         GtkWindow;
typedef struct _PaintEditorWindow PaintEditorWindow;
typedef struct _PaintMain         PaintMain;
typedef struct _PaintViewerWindow PaintViewerWindow;

GtkWidget *
paint_about_dialog_new (void);
void
paint_activate_about (GSimpleAction *action, GVariant *parameter, gpointer self);
GtkWidget *
paint_editor_window_new (GApplication *application);
GApplication *
paint_main_new (void);
GMenuModel *
paint_menu_new (void);
void
paint_present_modal (GtkWindow *modal, GtkWindow *parent);

#ifdef GTK_MAJOR_VERSION

G_DECLARE_FINAL_TYPE (PaintEditorWindow, paint_editor_window, PAINT, EDITOR_WINDOW, GtkApplicationWindow);
G_DECLARE_FINAL_TYPE (PaintMain,         paint_main,          PAINT, MAIN,          GtkApplication);

#define PAINT_TYPE_MAIN          paint_main_get_type          ()
#define PAINT_TYPE_EDITOR_WINDOW paint_editor_window_get_type ()
#endif /* GTK */
