/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"

/* Paint Document Window クラスのインスタンス */
struct _PaintDocumentWindow
{
	GtkApplicationWindow parent_instance;
};

static void paint_document_window_class_init        (PaintDocumentWindowClass *window);
static void paint_document_window_class_init_object (GObjectClass *object);
static void paint_document_window_class_init_widget (GtkWidgetClass *widget);
static void paint_document_window_dispose           (GObject *object);
static void paint_document_window_init              (PaintDocumentWindow *window);

/*******************************************************************************
Paint Document Window クラス:
ドキュメント ウィンドウを表します。
ユーザーがドキュメントを編集する方法を提供します。
*/ G_DEFINE_FINAL_TYPE (PaintDocumentWindow, paint_document_window, GTK_TYPE_APPLICATION_WINDOW);
#define PAINT_DOCUMENT_WINDOW_PROPERTY_APPLICATION_NAME "application"
#define PAINT_DOCUMENT_WINDOW_PROPERTY_SHOW_MENUBAR_NAME "show-menubar"
#define PAINT_DOCUMENT_WINDOW_TEMPLATE_NAME "/com/github/mi19a009/paint/gtk/document.ui"

/*******************************************************************************
Class Init:
クラスを初期化します。
*/ static void
paint_document_window_class_init (PaintDocumentWindowClass *window)
{
	paint_document_window_class_init_object (G_OBJECT_CLASS (window));
	paint_document_window_class_init_widget (GTK_WIDGET_CLASS (window));
}

/*******************************************************************************
Class Init Object:
Objectクラスを初期化します。
*/ static void
paint_document_window_class_init_object (GObjectClass *object)
{
	object->dispose = paint_document_window_dispose;
}

/*******************************************************************************
Class Init Widget:
Widget クラスを初期化します。
*/ static void
paint_document_window_class_init_widget (GtkWidgetClass *widget)
{
	gtk_widget_class_set_template_from_resource (widget, PAINT_DOCUMENT_WINDOW_TEMPLATE_NAME);
}

/*******************************************************************************
Init:
クラスのインスタンスを破棄します。
*/ static void
paint_document_window_dispose (GObject *object)
{
	gtk_widget_dispose_template (GTK_WIDGET (object), PAINT_TYPE_DOCUMENT_WINDOW);
	G_OBJECT_CLASS (paint_document_window_parent_class)->dispose (object);
}

/*******************************************************************************
Init:
クラスのインスタンスを初期化します。
*/ static void
paint_document_window_init (PaintDocumentWindow *window)
{
	gtk_widget_init_template (GTK_WIDGET (window));
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
