/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "paint.h"
#define PROPERTY_APPLICATION  "application"
#define PROPERTY_SHOW_MENUBAR "show-menubar"
#define SETTINGS_HEIGHT       "window-height"
#define SETTINGS_MAXIMIZED    "window-maximized"
#define SETTINGS_WIDTH        "window-width"
#define TITLE                 _("Paint")

/* クラスのインスタンス */
struct _PaintDocumentWindow
{
	GtkApplicationWindow parent_instance;
	int                  width;
	int                  height;
	int                  maximized;
};

static void paint_document_window_class_init        (PaintDocumentWindowClass *this_class);
static void paint_document_window_class_init_object (GObjectClass *this_class);
static void paint_document_window_construct         (GObject *self);
static void paint_document_window_init              (PaintDocumentWindow *self);
static void paint_document_window_init_settings     (PaintDocumentWindow *self);
static void paint_document_window_load_settings     (PaintDocumentWindow *self);

/*******************************************************************************
* Paint Document Window クラス。
* ドキュメントを格納するウィンドウを表します。
* ウィンドウ作成時はドキュメントを作成します。
* ウィンドウ破棄時はドキュメントを破棄します。
*/
G_DEFINE_FINAL_TYPE (PaintDocumentWindow, paint_document_window, GTK_TYPE_APPLICATION_WINDOW);

/*******************************************************************************
* @brief クラスを初期化します。
*/
static void
paint_document_window_class_init (PaintDocumentWindowClass *this_class)
{
	paint_document_window_class_init_object (G_OBJECT_CLASS (this_class));
}

/*******************************************************************************
* @brief Object クラスを初期化します。
*/
static void
paint_document_window_class_init_object (GObjectClass *this_class)
{
	this_class->constructed = paint_document_window_construct;
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
paint_document_window_construct (GObject *self)
{
	paint_document_window_init_settings (PAINT_DOCUMENT_WINDOW (self));
	G_OBJECT_CLASS (paint_document_window_parent_class)->constructed (self);
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
paint_document_window_init (PaintDocumentWindow *self)
{
	gtk_window_set_title (GTK_WINDOW (self), TITLE);
}

/*******************************************************************************
* @brief 環境設定を適用します。
*/
static void
paint_document_window_init_settings (PaintDocumentWindow *self)
{
	GtkWindow *window;
	window = GTK_WINDOW (self);
	paint_document_window_load_settings (self);
	gtk_window_set_default_size (window, self->width, self->height);

	if (self->maximized)
	{
		gtk_window_maximize (window);
	}
}

/*******************************************************************************
* @brief 環境設定を読み込みます。
*/
static void
paint_document_window_load_settings (PaintDocumentWindow *self)
{
	GSettings *settings;
	settings = paint_get_settings ();
	self->width = g_settings_get_int (settings, SETTINGS_WIDTH);
	self->height = g_settings_get_int (settings, SETTINGS_HEIGHT);
	self->maximized = g_settings_get_boolean (settings, SETTINGS_MAXIMIZED);
	g_object_unref (settings);
}

/*******************************************************************************
* @brief クラスのインスタンスを作成します。
*/
GtkWidget *
paint_document_window_new (GApplication *application)
{
	return g_object_new (PAINT_TYPE_DOCUMENT_WINDOW,
		PROPERTY_APPLICATION, application,
		PROPERTY_SHOW_MENUBAR, TRUE,
		NULL);
}
