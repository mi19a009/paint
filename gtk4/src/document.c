/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"

/* Paint Document Window クラスのインスタンス */
struct _PaintDocumentWindow
{
	GtkApplicationWindow parent_instance;
};

static void paint_document_window_class_init (PaintDocumentWindowClass *window);
static void paint_document_window_init       (PaintDocumentWindow *window);

/*******************************************************************************
Paint Document Window クラス:
ドキュメント ウィンドウを表します。
ユーザーがドキュメントを編集する方法を提供します。
*/ G_DEFINE_FINAL_TYPE (PaintDocumentWindow, paint_document_window, GTK_TYPE_APPLICATION_WINDOW);
#define PAINT_DOCUMENT_WINDOW_PROPERTY_APPLICATION_NAME "application"
#define PAINT_DOCUMENT_WINDOW_PROPERTY_SHOW_MENUBAR_NAME "show-menubar"

/*******************************************************************************
Class Init:
クラスを初期化します。
*/ static void
paint_document_window_class_init (PaintDocumentWindowClass *window)
{
}

/*******************************************************************************
Init:
クラスのインスタンスを初期化します。
*/ static void
paint_document_window_init (PaintDocumentWindow *window)
{
}

/*******************************************************************************
New:
新しいインスタンスを作成します。
*/ GtkWidget *
paint_document_window_new (GApplication *application)
{
	return g_object_new (PAINT_TYPE_DOCUMENT_WINDOW,
		PAINT_DOCUMENT_WINDOW_PROPERTY_APPLICATION_NAME, application,
		PAINT_DOCUMENT_WINDOW_PROPERTY_SHOW_MENUBAR_NAME, TRUE,
		NULL);
}
