/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define PAINT_APPLICATION_FLAGS G_APPLICATION_HANDLES_OPEN

static void paint_application_activate               (GApplication *application);
static void paint_application_class_init             (PaintApplicationClass *application);
static void paint_application_class_init_application (GApplicationClass *application);
static void paint_application_init                   (PaintApplication *application);
static void paint_application_open                   (GApplication *application, GFile **files, gint n_files, const gchar *hint);
static void paint_application_show_document          (PaintApplication *application);
static void paint_application_startup                (GApplication *application);

/*******************************************************************************
Paint Application クラス:
アプリケーションを表します。
アプリケーションは各ドキュメント ウィンドウが共有する情報を格納します。
*/ G_DEFINE_TYPE (PaintApplication, paint_application, GTK_TYPE_APPLICATION);
#define PAINT_APPLICATION_PROPERTY_APPLICATION_ID_NAME "application-id"
#define PAINT_APPLICATION_PROPERTY_FLAGS_NAME          "flags"

/*******************************************************************************
Activate:
アプリケーション ウィンドウを表示します。
*/ static void
paint_application_activate (GApplication *application)
{
	G_APPLICATION_CLASS (paint_application_parent_class)->activate (application);
	paint_application_show_document (PAINT_APPLICATION (application));
}

/*******************************************************************************
Class Init:
クラスを初期化します。
*/ static void
paint_application_class_init (PaintApplicationClass *application)
{
	paint_application_class_init_application (G_APPLICATION_CLASS (application));
}

/*******************************************************************************
Class Init Application:
Application クラスを初期化します。
*/ static void
paint_application_class_init_application (GApplicationClass *application)
{
	application->activate = paint_application_activate;
	application->open = paint_application_open;
	application->startup = paint_application_startup;
}

/*******************************************************************************
Init:
クラスのインスタンスを初期化します。
*/ static void
paint_application_init (PaintApplication *application)
{
}

/*******************************************************************************
New:
新しいインスタンスを作成します。
*/ GApplication *
paint_application_new (const char *application_id)
{
	return g_object_new (PAINT_TYPE_APPLICATION,
		PAINT_APPLICATION_PROPERTY_APPLICATION_ID_NAME, application_id,
		PAINT_APPLICATION_PROPERTY_FLAGS_NAME, PAINT_APPLICATION_FLAGS,
		NULL);
}

/*******************************************************************************
Open:
ドキュメント ファイルを開きます。
*/ static void
paint_application_open (GApplication *application, GFile **files, gint n_files, const gchar *hint)
{
}

/*******************************************************************************
Show Document:
ドキュメント ウィンドウを表示します。
*/ static void
paint_application_show_document (PaintApplication *application)
{
	GtkWidget *widget;
	widget = paint_document_window_new (G_APPLICATION (application));

	if (widget)
	{
		gtk_window_present (GTK_WINDOW (widget));
	}
}

/*******************************************************************************
Startup:
アプリケーションを開始します。
*/ static void
paint_application_startup (GApplication *application)
{
	G_APPLICATION_CLASS (paint_application_parent_class)->startup (application);
}
