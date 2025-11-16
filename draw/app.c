/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "draw.h"
#include "share.h"

/* クラスのインスタンス */
struct _DrawApplication
{
	GtkApplication parent_instance;
};

static void draw_application_activate               (GApplication *self);
static void draw_application_activate_new           (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void draw_application_class_init             (DrawApplicationClass *this_class);
static void draw_application_class_init_application (GApplicationClass *this_class);
static void draw_application_init                   (DrawApplication *self);
static void draw_application_open                   (GApplication *self, GFile **files, gint n_files, const gchar *hint);
static void draw_application_startup                (GApplication *self);

/*******************************************************************************
* Draw Application クラス:
* メニュー、アクセラレーター、およびウィンドウを作成する方法を提供します。
*/
G_DEFINE_FINAL_TYPE (DrawApplication, draw_application, GTK_TYPE_APPLICATION);
static const char *ACCELS_CLOSE        [] = { "<Ctrl>q", NULL };
static const char *ACCELS_HELP_OVERLAY [] = { "<Ctrl>question", "<Ctrl>slash", NULL };
static const char *ACCELS_NEW          [] = { "<Ctrl>n", NULL };

/* メニュー アクセラレーター */
static const ShareAccelEntry
ACCEL_ENTRIES [] =
{
	{ "window.close",          ACCELS_CLOSE        },
	{ "win.show-help-overlay", ACCELS_HELP_OVERLAY },
	{ "app.new",               ACCELS_NEW          },
};

/* メニュー アクション */
static const GActionEntry
ACTION_ENTRIES [] =
{
	{ "new", draw_application_activate_new, NULL, NULL, NULL },
};

/*******************************************************************************
* @brief アプリケーションを表示します。
*/
static void
draw_application_activate (GApplication *self)
{
	GtkWidget *document;
	document = draw_document_window_new (self);
	gtk_window_present (GTK_WINDOW (document));
}

/*******************************************************************************
* @brief 新しいウィンドウを表示します。
*/
static void
draw_application_activate_new (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkWidget *document;
	document = draw_document_window_new (G_APPLICATION (user_data));
	gtk_window_present (GTK_WINDOW (document));
}

/*******************************************************************************
* @brief クラスを初期化します。
*/
static void
draw_application_class_init (DrawApplicationClass *this_class)
{
	draw_application_class_init_application (G_APPLICATION_CLASS (this_class));
}

/*******************************************************************************
* @brief Application クラスを初期化します。
*/
static void
draw_application_class_init_application (GApplicationClass *this_class)
{
	this_class->activate = draw_application_activate;
	this_class->open = draw_application_open;
	this_class->startup = draw_application_startup;
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
draw_application_init (DrawApplication *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
}

/*******************************************************************************
* @brief 指定したファイルを開きます。
*/
static void
draw_application_open (GApplication *self, GFile **files, gint n_files, const gchar *hint)
{
	GtkWidget *document;
	int n;

	for (n = 0; n < n_files; n++)
	{
		document = draw_document_window_new (self);
		gtk_window_present (GTK_WINDOW (document));
	}
}

/*******************************************************************************
* @brief アプリケーションを開始します。
*/
static void
draw_application_startup (GApplication *self)
{
	G_APPLICATION_CLASS (draw_application_parent_class)->startup (self);
	share_application_set_accel_entries (GTK_APPLICATION (self), ACCEL_ENTRIES, G_N_ELEMENTS (ACCEL_ENTRIES));
}
