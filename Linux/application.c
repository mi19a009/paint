/*
Copyright 2025 Taichi Murakami.
GTK アプリケーションを実装します。
*/

#include <gtk/gtk.h>
#include "paint.h"

#define ACTION_CLOSE_NAME       "window.close"
#define ACTION_CLOSE_ACCEL      "<Ctrl>Q"
#define ACTION_COPY_NAME        "win.copy"
#define ACTION_COPY_ACCEL       "<Ctrl>C"
#define ACTION_CUT_NAME         "win.cut"
#define ACTION_CUT_ACCEL        "<Ctrl>X"
#define ACTION_NEW              "new"
#define ACTION_NEW_NAME         "app.new"
#define ACTION_NEW_ACCEL        "<Ctrl>N"
#define ACTION_OPEN             "open"
#define ACTION_OPEN_NAME        "win.open"
#define ACTION_OPEN_ACCEL       "<Ctrl>O"
#define ACTION_PASTE_NAME       "win.paste"
#define ACTION_PASTE_ACCEL      "<Ctrl>V"
#define ACTION_PRINT_NAME       "win.print"
#define ACTION_PRINT_ACCEL      "<Ctrl>P"
#define ACTION_QUIT             "quit"
#define ACTION_REDO_NAME        "win.redo"
#define ACTION_REDO_ACCEL       "<Shift><Ctrl>Z"
#define ACTION_SAVE_AS_NAME     "win.save-as"
#define ACTION_SAVE_AS_ACCEL    "<Shift><Ctrl>S"
#define ACTION_SAVE_NAME        "win.save"
#define ACTION_SAVE_ACCEL       "<Ctrl>S"
#define ACTION_SELECT_ALL_NAME  "win.select-all"
#define ACTION_SELECT_ALL_ACCEL "<Ctrl>A"
#define ACTION_UNDO_NAME        "win.undo"
#define ACTION_UNDO_ACCEL       "<Ctrl>Z"
#define PROPERTY_APPLICATION_ID "application-id"
#define PROPERTY_FLAGS          "flags"
#define UI_MENU                 "menu"
#define UI_TEMPLATE             "application.ui"

typedef struct _PaintApplication
{
	GtkApplication super;
	GtkWidget *window;
	GSettings *settings;
} Self;

static void paint_application_class_init (PaintApplicationClass *self);
static void paint_application_init (PaintApplication *self);

G_DEFINE_FINAL_TYPE (PaintApplication, paint_application, GTK_TYPE_APPLICATION);
#define SUPER_CLASS             paint_application_parent_class
#define THIS_TYPE               paint_application_get_type ()

static void activate (GApplication *application);
static void activate_new (GSimpleAction *action, GVariant *parameter, gpointer self);
static void activate_quit (GSimpleAction *action, GVariant *parameter, gpointer self);
static void dispose (GObject *object);
static void dispose_self (Self *self);
static void init_accel (GtkApplication *application);
static void init_menu (GtkApplication *application);
static void init_object_class (GObjectClass *object);
static void init_application_class (GApplicationClass *application);
static void open (GApplication *application, GFile **files, gint n_files, const gchar *hint);
static void show_document_window (GApplication *application);
static void show_document_window_from_file (GApplication *application, GFile *file);
static void startup (GApplication *application);

const char *paint_application_authors[] = { "Taichi Murakami", NULL };
const char *paint_application_copyright = "Copyright © 2025 Taichi Murakami.";
const char *paint_application_id = "com.github.mi19a009.paint";
const char *paint_application_name = "Paint";
const char *paint_application_prefix = "/com/github/mi19a009/paint/";
const char *paint_application_website = "https://github.com/mi19a009/paint";

/* メニュー項目とキーボード ショートカット。 */
static const char *accel_entries[] =
{
	ACTION_CLOSE_NAME, ACTION_CLOSE_ACCEL,
	ACTION_NEW_NAME, ACTION_NEW_ACCEL,
	ACTION_OPEN_NAME, ACTION_OPEN_ACCEL,
	ACTION_SAVE_NAME, ACTION_SAVE_ACCEL,
	ACTION_SAVE_AS_NAME, ACTION_SAVE_AS_ACCEL,
	NULL,
};

/* メニュー項目とコールバック関数。 */
static const GActionEntry action_entries[] =
{
	{ ACTION_NEW, activate_new },
	{ ACTION_QUIT, activate_quit },
};

/* クラスの新しいインスタンスを初期化します。 */
GApplication *paint_application_new (void)
{
	return g_object_new (THIS_TYPE, PROPERTY_APPLICATION_ID, paint_application_id, PROPERTY_FLAGS, G_APPLICATION_HANDLES_OPEN, NULL);
}

/* クラスを初期化します。 */
static void paint_application_class_init (PaintApplicationClass *self)
{
	init_object_class (G_OBJECT_CLASS (self));
	init_application_class (G_APPLICATION_CLASS (self));
}

/* クラスの新しいインスタンスを初期化します。 */
static void paint_application_init (PaintApplication *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), action_entries, G_N_ELEMENTS (action_entries), self);
}

/* 新しいウィンドウを作成します。 */
static void activate (GApplication *application)
{
	G_APPLICATION_CLASS (SUPER_CLASS)->activate (application);
	show_document_window (application);
}

/* メニュー項目: 新しいウィンドウ。 */
static void activate_new (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	show_document_window (G_APPLICATION (self));
}

/* メニュー項目: 終了。 */
static void activate_quit (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	g_application_quit (G_APPLICATION (self));
}

/* 現在のインスタンスを終了します。 */
static void dispose (GObject *object)
{
	dispose_self (PAINT_APPLICATION (object));
	G_OBJECT_CLASS (SUPER_CLASS)->dispose (object);
}

/* 現在のクラス変数を開放します。 */
static void dispose_self (Self *self)
{
	g_clear_object (&self->settings);
}

/* クラスのコールバック関数を登録します。 */
static void init_object_class (GObjectClass *object)
{
	object->dispose = dispose;
}

/* 各アクションにキーボード ショートカットを割り当てます。 */
static void init_accel (GtkApplication *application)
{
	const char **entries;
	const char *action_name;
	const char *accels [2];
	entries = accel_entries;
	accels [1] = NULL;

	while (*entries)
	{
		action_name = *(entries++);
		*accels = *(entries++);
		gtk_application_set_accels_for_action (application, action_name, accels);
	}
}

/* 現在のアプリケーションに新しいメニューを設定します。 */
static void init_menu (GtkApplication *application)
{
	GtkBuilder *builder;
	GMenuModel *menubar;
	char name [CCH_RESOURCE_NAME];
	format_resource_name (name, CCH_RESOURCE_NAME, UI_TEMPLATE);
	builder = gtk_builder_new_from_resource (name);

	if (builder)
	{
		menubar = G_MENU_MODEL (gtk_builder_get_object (builder, UI_MENU));

		if (menubar)
		{
			gtk_application_set_menubar (application, menubar);
		}

		g_object_unref (builder);
	}
}

/* クラスのコールバック関数を登録します。 */
static void init_application_class (GApplicationClass *application)
{
	application->startup = startup;
	application->activate = activate;
	application->open = open;
}

/* ファイル名を指定して新しいウィンドウを作成します。 */
static void open (GApplication *application, GFile **files, gint n_files, const gchar *hint)
{
	int n;

	for (n = 0; n < n_files; n++)
	{
		show_document_window_from_file (application, *(files++));
	}
}

/* 新しいドキュメント ウィンドウを作成します。 */
static void show_document_window (GApplication *application)
{
	GtkWidget *window;
	window = paint_document_window_new (application);

	if (window)
	{
		gtk_window_present (GTK_WINDOW (window));
	}
}

/* 新しいドキュメント ウィンドウを作成します。 */
static void show_document_window_from_file (GApplication *application, GFile *file)
{
	char *path;
	GtkWidget *window;
	path = g_file_get_path (file);

	if (path)
	{
		window = paint_document_window_new_from_file (application, path);
	
		if (window)
		{
			gtk_window_present (GTK_WINDOW (window));
		}

		g_free (path);
	}
}

/* 現在のアプリケーションを開始します。 */
static void startup (GApplication *application)
{
	G_APPLICATION_CLASS (SUPER_CLASS)->startup (application);
	init_menu (GTK_APPLICATION (application));
	init_accel (GTK_APPLICATION (application));
}
