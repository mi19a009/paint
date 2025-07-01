/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "text.h"

typedef struct _PaintMainEntry PaintMainEntry;

/* クラスのインスタンス */
struct _PaintMain
{
	GtkApplication parent_instance;
};

/* ショートカット キー */
struct _PaintMainEntry
{
	const char *detailed_action_name;
	const char *accel;
};

static void
paint_main_activate (GApplication *);
static void
paint_main_activate_new (GSimpleAction *, GVariant *, gpointer);
static void
paint_main_class_init (PaintMainClass *);
static void
paint_main_destroy (PaintMain *);
static void
paint_main_dispose (GObject *);
static void
paint_main_init (PaintMain *);
static void
paint_main_init_accels (GtkApplication *);
static void
paint_main_init_actions (GActionMap *);
static void
paint_main_init_menubar (GtkApplication *);
static void
paint_main_open (GApplication *, GFile **, gint, const gchar *);
static GtkWidget *
paint_main_show (GApplication *);
static void
paint_main_startup (GApplication *);
static void
paint_main_startup_gtk (GtkApplication *);

G_DEFINE_FINAL_TYPE (PaintMain, paint_main, GTK_TYPE_APPLICATION);

/* ショートカット キー */
static const PaintMainEntry
paint_main_entries [] =
{
	{ "app.new",      "<Ctrl>N" },
	{ "window.close", "<Ctrl>Q" },
};

/*******************************************************************************
 * @brief アプリケーションを開始する。
 * @return この操作が成功した場合は 0 を返す。
 ******************************************************************************/
int main (int argc, char *argv [])
{
	GApplication *application;
	int result;
	application = paint_main_new ();

	if (application)
	{
		result = g_application_run (application, argc, argv);
		g_object_unref (application);
	}
	else
	{
		result = EXIT_FAILURE;
	}

	return result;
}

/*******************************************************************************
 * @brief 最初のウィンドウを表示する。
 ******************************************************************************/
void
paint_main_activate (GApplication *application)
{
	G_APPLICATION_CLASS (paint_main_parent_class)->activate (application);
	paint_main_show (application);
}

/*******************************************************************************
 * @brief 新規ウィンドウを表示する。
 ******************************************************************************/
void
paint_main_activate_new (GSimpleAction *, GVariant *, gpointer self)
{
	paint_main_show (G_APPLICATION (self));
}

/*******************************************************************************
 * @brief クラスのコールバック関数を登録する。
 ******************************************************************************/
void
paint_main_class_init (PaintMainClass *main)
{
	GObjectClass *object;
	GApplicationClass *application;
	object = G_OBJECT_CLASS (main);
	application = G_APPLICATION_CLASS (main);
	object->dispose       = paint_main_dispose;
	application->startup  = paint_main_startup;
	application->activate = paint_main_activate;
	application->open     = paint_main_open;
}

/*******************************************************************************
 * @brief メンバー変数を破棄する。
 ******************************************************************************/
void
paint_main_destroy (PaintMain *)
{
}

/*******************************************************************************
 * @brief クラスのインスタンスを破棄する。
 ******************************************************************************/
void
paint_main_dispose (GObject *object)
{
	paint_main_destroy (PAINT_MAIN (object));
	G_OBJECT_CLASS (paint_main_parent_class)->dispose (object);
}

/*******************************************************************************
 * @brief クラスのインスタンスを初期化する。
 ******************************************************************************/
void
paint_main_init (PaintMain *main)
{
	paint_main_init_actions (G_ACTION_MAP (main));
}

/*******************************************************************************
 * @brief ショートカット キーを設定する。
 ******************************************************************************/
void
paint_main_init_accels (GtkApplication *application)
{
	const PaintMainEntry *entries;
	const char *accels [2];
	unsigned index;
	entries = paint_main_entries;
	accels [1] = NULL;

	for (index = 0; index < G_N_ELEMENTS (paint_main_entries); index++)
	{
		accels [0] = entries->accel;
		gtk_application_set_accels_for_action (application, (entries++)->detailed_action_name, accels);
	}
}

/*******************************************************************************
 * @brief メニュー項目のコールバック関数を登録する。
 ******************************************************************************/
void
paint_main_init_actions (GActionMap *actions)
{
	GSimpleAction *action;
	action = g_simple_action_new ("new", NULL);

	if (action)
	{
		g_signal_connect (action, "activate", G_CALLBACK (paint_main_activate_new), actions);
		g_action_map_add_action (actions, G_ACTION (action));
		g_object_unref (action);
	}
}

/*******************************************************************************
 * @brief メニュー バーを登録する。
 ******************************************************************************/
void
paint_main_init_menubar (GtkApplication *application)
{
	GMenuModel *menubar;
	menubar = paint_menu_new ();

	if (menubar)
	{
		gtk_application_set_menubar (application, menubar);
		g_object_unref (menubar);
	}
}

/*******************************************************************************
 * @brief クラスのインスタンスを作成する。
 * @return 作成したインスタンスを返す。
 ******************************************************************************/
GApplication *
paint_main_new (void)
{
	return g_object_new (PAINT_TYPE_MAIN,
		"application-id", PAINT_APPLICATION_ID,
		"flags", G_APPLICATION_HANDLES_OPEN,
		NULL);
}

/*******************************************************************************
 * @brief ファイルを開く。
 ******************************************************************************/
void
paint_main_open (GApplication *application, GFile **files, gint n_files, const gchar *hint)
{
	G_APPLICATION_CLASS (paint_main_parent_class)->open (application, files, n_files, hint);
	paint_main_show (application);
}

/*******************************************************************************
 * @brief 新しいアプリケーション ウィンドウを作成する。
 * @return 作成したウィンドウを返す。
 ******************************************************************************/
GtkWidget *
paint_main_show (GApplication *application)
{
	GtkWidget *window;
	window = paint_editor_window_new (application);

	if (window)
	{
		gtk_window_present (GTK_WINDOW (window));
	}

	return window;
}

/*******************************************************************************
 * @brief アプリケーションを開始する。
 ******************************************************************************/
void
paint_main_startup (GApplication *application)
{
	G_APPLICATION_CLASS (paint_main_parent_class)->startup (application);
	text_language_init ();
	paint_main_startup_gtk (GTK_APPLICATION (application));
}

/*******************************************************************************
 * @brief GTK アプリケーションを開始する。
 ******************************************************************************/
void
paint_main_startup_gtk (GtkApplication *application)
{
	paint_main_init_menubar (application);
	paint_main_init_accels (application);
}
