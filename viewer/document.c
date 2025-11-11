/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define PROPERTY_APPLICATION  "application"
#define PROPERTY_SHOW_MENUBAR "show-menubar"
#define TITLE                 _("Picture Viewer")

/* クラスのインスタンス */
struct _ViewerDocumentWindow
{
	GtkApplicationWindow parent_instance;
};

static void viewer_document_window_class_init (ViewerDocumentWindowClass *this_class);
static void viewer_document_window_init       (ViewerDocumentWindow *self);

/*******************************************************************************
* Viewer Document Window クラス。
* ドキュメントを格納するウィンドウを表します。
* ウィンドウ作成時はドキュメントを作成します。
* ウィンドウ破棄時はドキュメントを破棄します。
*/
G_DEFINE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, GTK_TYPE_APPLICATION_WINDOW);

/*******************************************************************************
* @brief クラスを初期化します。
*/
static void
viewer_document_window_class_init (ViewerDocumentWindowClass *this_class)
{
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
viewer_document_window_init (ViewerDocumentWindow *self)
{
	gtk_window_set_title (GTK_WINDOW (self), TITLE);
}

/*******************************************************************************
* @brief クラスのインスタンスを作成します。
*/
GtkWidget *
viewer_document_window_new (GApplication *application)
{
	return g_object_new (VIEWER_TYPE_DOCUMENT_WINDOW,
		PROPERTY_APPLICATION, application,
		PROPERTY_SHOW_MENUBAR, TRUE,
		NULL);
}
