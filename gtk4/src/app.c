/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define PAINT_APPLICATION_FLAGS G_APPLICATION_HANDLES_OPEN
#define RESOURCE_OBJECTS "gtk/objects.ui"
#define RESOURCE_OBJECTS_ABOUT_DIALOG "about_dialog"
#define SIGNAL_DESTROY "destroy"

static void paint_application_activate               (GApplication *application);
static void paint_application_activate_about         (GSimpleAction *action, GVariant *parameter, gpointer self);
static void paint_application_class_init             (PaintApplicationClass *application);
static void paint_application_class_init_application (GApplicationClass *application);
static void paint_application_init                   (PaintApplication *application);
static void paint_application_open                   (GApplication *application, GFile **files, gint n_files, const gchar *hint);
static void paint_application_show_about             (PaintApplication *application);
static void paint_application_show_document          (PaintApplication *application);
static void paint_application_startup                (GApplication *application);

/* メニュー アクション */
static GActionEntry paint_application_action_entries [] =
{
	{ "about", paint_application_activate_about, NULL, NULL, NULL },
};

/*******************************************************************************
Paint Application クラス:
アプリケーションを表します。
アプリケーションは各ドキュメント ウィンドウが共有する情報を格納します。
*/ G_DEFINE_TYPE (PaintApplication, paint_application, GTK_TYPE_APPLICATION);
#define PAINT_APPLICATION_N_ACTION_ENTRIES (G_N_ELEMENTS (paint_application_action_entries))
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
Activate About:
バージョン情報ダイアログ ボックスを表示します。
*/ static void
paint_application_activate_about (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	paint_application_show_about (PAINT_APPLICATION (self));
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
	g_action_map_add_action_entries (G_ACTION_MAP (application), paint_application_action_entries, PAINT_APPLICATION_N_ACTION_ENTRIES, application);
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
	gint index;

	for (index = 0; index < n_files; index++)
	{
		paint_application_show_document (PAINT_APPLICATION (application));
	}
}

/*******************************************************************************
Show About:
バージョン情報ダイアログ ボックスを表示します。
*/ static void
paint_application_show_about (PaintApplication *application)
{
	GtkBuilder *builder;
	GtkWindow *window;
	char path [PAINT_RESOURCE_PATH_CCH];
	paint_get_resource_path (path, PAINT_RESOURCE_PATH_CCH, RESOURCE_OBJECTS);
	builder = gtk_builder_new_from_resource (path);

	if (builder)
	{
		window = GTK_WINDOW (gtk_builder_get_object (builder, RESOURCE_OBJECTS_ABOUT_DIALOG));

		if (window)
		{
			g_signal_connect_swapped (window, SIGNAL_DESTROY, G_CALLBACK (gtk_window_destroy), window);
			gtk_window_set_destroy_with_parent (window, TRUE);
			gtk_window_set_modal (window, TRUE);
			gtk_window_set_transient_for (window, gtk_application_get_active_window (GTK_APPLICATION (application)));
			gtk_window_present (window);
		}

		g_object_unref (builder);
	}
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
