/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"
#include "share.h"

/* クラスのインスタンス */
struct _ViewerApplication
{
	GtkApplication parent_instance;
};

static void viewer_application_activate               (GApplication *self);
static void viewer_application_activate_new           (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_application_class_init             (ViewerApplicationClass *this_class);
static void viewer_application_class_init_application (GApplicationClass *this_class);
static void viewer_application_init                   (ViewerApplication *self);
static void viewer_application_open                   (GApplication *self, GFile **files, gint n_files, const gchar *hint);
static void viewer_application_startup                (GApplication *self);

/*******************************************************************************
* Viewer Application クラス:
* メニュー、アクセラレーター、およびウィンドウを作成する方法を提供します。
*/
G_DEFINE_FINAL_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);
static const char *ACCELS_CLOSE        [] = { "<Ctrl>q", NULL };
static const char *ACCELS_FULLSCREEN   [] = { "F11", NULL };
static const char *ACCELS_HELP_OVERLAY [] = { "<Ctrl>question", "<Ctrl>slash", NULL };
static const char *ACCELS_NEW          [] = { "<Ctrl>n", NULL };
static const char *ACCELS_OPEN         [] = { "<Ctrl>o", NULL };
static const char *ACCELS_PRINT        [] = { "<Ctrl>p", NULL };
static const char *ACCELS_RESTORE_ZOOM [] = { "<Ctrl>0", NULL };
static const char *ACCELS_UNFULLSCREEN [] = { "Escape", NULL };
static const char *ACCELS_ZOOM_IN      [] = { "<Ctrl>plus", "<Ctrl>semicolon", NULL };
static const char *ACCELS_ZOOM_OUT     [] = { "<Ctrl>minus", NULL };

/* メニュー アクセラレーター */
static const ShareAccelEntry
ACCEL_ENTRIES [] =
{
	{ "window.close",          ACCELS_CLOSE        },
	{ "win.fullscreen",        ACCELS_FULLSCREEN   },
	{ "win.show-help-overlay", ACCELS_HELP_OVERLAY },
	{ "app.new",               ACCELS_NEW          },
	{ "win.open",              ACCELS_OPEN         },
	{ "win.print",             ACCELS_PRINT        },
	{ "win.restore-zoom",      ACCELS_RESTORE_ZOOM },
	{ "win.unfullscreen",      ACCELS_UNFULLSCREEN },
	{ "win.zoom-in",           ACCELS_ZOOM_IN      },
	{ "win.zoom-out",          ACCELS_ZOOM_OUT     },
};

/* メニュー アクション */
static const GActionEntry
ACTION_ENTRIES [] =
{
	{ "new", viewer_application_activate_new, NULL, NULL, NULL },
};

/*******************************************************************************
* @brief アプリケーションを表示します。
*/
static void
viewer_application_activate (GApplication *self)
{
	GtkWidget *document;
	document = viewer_document_window_new (self);
	gtk_window_present (GTK_WINDOW (document));
}

/*******************************************************************************
* @brief 新しいウィンドウを表示します。
*/
static void
viewer_application_activate_new (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkWidget *document;
	document = viewer_document_window_new (G_APPLICATION (user_data));
	gtk_window_present (GTK_WINDOW (document));
}

/*******************************************************************************
* @brief クラスを初期化します。
*/
static void
viewer_application_class_init (ViewerApplicationClass *this_class)
{
	viewer_application_class_init_application (G_APPLICATION_CLASS (this_class));
}

/*******************************************************************************
* @brief Application クラスを初期化します。
*/
static void
viewer_application_class_init_application (GApplicationClass *this_class)
{
	this_class->activate = viewer_application_activate;
	this_class->open = viewer_application_open;
	this_class->startup = viewer_application_startup;
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
viewer_application_init (ViewerApplication *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
}

/*******************************************************************************
* @brief 指定したファイルを開きます。
*/
static void
viewer_application_open (GApplication *self, GFile **files, gint n_files, const gchar *hint)
{
	GtkWidget *document;
	int n;

	for (n = 0; n < n_files; n++)
	{
		document = viewer_document_window_new (self);
		viewer_document_window_set_file (VIEWER_DOCUMENT_WINDOW (document), files [n]);
		gtk_window_present (GTK_WINDOW (document));
	}
}

/*******************************************************************************
* @brief アプリケーションを開始します。
*/
static void
viewer_application_startup (GApplication *self)
{
	G_APPLICATION_CLASS (viewer_application_parent_class)->startup (self);
	share_application_set_accel_entries (GTK_APPLICATION (self), ACCEL_ENTRIES, G_N_ELEMENTS (ACCEL_ENTRIES));
}
