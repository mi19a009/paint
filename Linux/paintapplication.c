/*
 * Copyright 2025 Taichi Murakami.
 * Paint Application クラス。
 * GTK アプリケーションを実装します。
 * 現在のアプリケーションにメニュー バーを割り当てます。
 * 新しいアプリケーション ウィンドウを表示します。
 */

#include "paint.h"
#define PAINT_ACCEL_COPY                        "<Ctrl>C"
#define PAINT_ACCEL_CUT                         "<Ctrl>X"
#define PAINT_ACCEL_NEW                         "<Ctrl>N"
#define PAINT_ACCEL_OPEN                        "<Ctrl>O"
#define PAINT_ACCEL_PASTE                       "<Ctrl>V"
#define PAINT_ACCEL_PRINT                       "<Ctrl>P"
#define PAINT_ACCEL_QUIT                        "<Ctrl>Q"
#define PAINT_ACCEL_REDO                        "<Shift><Ctrl>Z"
#define PAINT_ACCEL_SAVE                        "<Ctrl>S"
#define PAINT_ACCEL_SAVE_AS                     "<Shift><Ctrl>S"
#define PAINT_ACCEL_SELECT_ALL                  "<Ctrl>A"
#define PAINT_ACCEL_UNDO                        "<Ctrl>Z"

typedef void (* PaintApplicationInitMenu) (GMenu *menu);
static void paint_application_activate                         (GApplication *application);
static void paint_application_activate_about                   (GSimpleAction *action, GVariant *parameter, gpointer application);
static void paint_application_activate_new                     (GSimpleAction *action, GVariant *parameter, gpointer application);
static void paint_application_activate_quit                    (GSimpleAction *action, GVariant *parameter, gpointer application);
static void paint_application_class_init                       (PaintApplicationClass *application);
static void paint_application_class_init_application           (GApplicationClass *application);
static void paint_application_class_init_object                (GObjectClass *object);
static void paint_application_dispose                          (GObject *object);
static void paint_application_dispose_self                     (PaintApplication *application);
static void paint_application_init                             (PaintApplication *application);
static void paint_application_init_menubar                     (GtkApplication *application);
static void paint_application_open                             (GApplication *application, GFile **files, gint n_files, const gchar *hint);
static void paint_application_show_about_dialog                (void);
static void paint_application_show_window                      (GApplication *application);
static void paint_application_show_window_from_file            (GApplication *application, GFile *file);
static void paint_application_startup                          (GApplication *application);
static void paint_application_startup_gtk                      (GtkApplication *application);
static void paint_application_startup_self                     (PaintApplication *application);

/**
 * Paint Application クラス。
 */
struct _PaintApplication
{
	GtkApplication super;
	PaintSettings *settings;
};

G_DEFINE_FINAL_TYPE (PaintApplication, paint_application, GTK_TYPE_APPLICATION);
const char *paint_application_authors [] = { "Taichi Murakami", NULL };
const char *paint_application_copyright = "Copyright © 2025 Taichi Murakami.";
const char *paint_application_id = "com.github.mi19a009.paint";
const char *paint_application_name = "Paint";
const char *paint_application_website = "https://mi19a009.github.io/paint/";

/**
 * メニュー項目とキーボード ショートカット。
 */
static const PaintAccelEntry paint_application_accel_entries [] =
{
	{ PAINT_COMMAND_CLOSE,      PAINT_ACCEL_QUIT       },
	{ PAINT_COMMAND_COPY,       PAINT_ACCEL_COPY       },
	{ PAINT_COMMAND_CUT,        PAINT_ACCEL_CUT        },
	{ PAINT_COMMAND_NEW,        PAINT_ACCEL_NEW        },
	{ PAINT_COMMAND_OPEN,       PAINT_ACCEL_OPEN       },
	{ PAINT_COMMAND_PASTE,      PAINT_ACCEL_PASTE      },
	{ PAINT_COMMAND_PRINT,      PAINT_ACCEL_PRINT      },
	{ PAINT_COMMAND_REDO,       PAINT_ACCEL_REDO       },
	{ PAINT_COMMAND_SAVE,       PAINT_ACCEL_SAVE       },
	{ PAINT_COMMAND_SAVE_AS,    PAINT_ACCEL_SAVE_AS    },
	{ PAINT_COMMAND_SELECT_ALL, PAINT_ACCEL_SELECT_ALL },
	{ PAINT_COMMAND_UNDO,       PAINT_ACCEL_UNDO       },
};

/**
 * メニュー項目とコールバック関数。
 */
static const PaintActionEntry paint_application_action_entries [] =
{
	{ PAINT_ACTION_ABOUT, paint_application_activate_about },
	{ PAINT_ACTION_NEW,   paint_application_activate_new   },
	{ PAINT_ACTION_QUIT,  paint_application_activate_quit  },
};

/**
 * この関数は GTK アプリケーションが活性化した場合に呼び出されます。
 * 新しいウィンドウを作成します。
 */
void paint_application_activate (GApplication *application)
{
	G_APPLICATION_CLASS (paint_application_parent_class)->activate (application);
	paint_application_show_window (application);
}

/**
 * メニュー: バージョン情報。
 */
void paint_application_activate_about (GSimpleAction *action, GVariant *parameter, gpointer application)
{
	paint_application_show_about_dialog ();
}

/**
 * メニュー: 新しいウィンドウ。
 */
void paint_application_activate_new (GSimpleAction *action, GVariant *parameter, gpointer application)
{
	paint_application_show_window (G_APPLICATION (application));
}

/**
 * メニュー: 終了。
 */
void paint_application_activate_quit (GSimpleAction *action, GVariant *parameter, gpointer application)
{
	g_application_quit (G_APPLICATION (application));
}

/**
 * クラスを初期化します。
 */
void paint_application_class_init (PaintApplicationClass *application)
{
	paint_application_class_init_object (G_OBJECT_CLASS (application));
	paint_application_class_init_application (G_APPLICATION_CLASS (application));
}

/**
 * クラスのコールバック関数を登録します。
 */
void paint_application_class_init_application (GApplicationClass *application)
{
	application->startup = paint_application_startup;
	application->activate = paint_application_activate;
	application->open = paint_application_open;
}

/**
 * クラスのコールバック関数を登録します。
 */
void paint_application_class_init_object (GObjectClass *object)
{
	object->dispose = paint_application_dispose;
}

/**
 * この関数は現在のインスタンスが破棄される場合に呼び出されます。
 */
void paint_application_dispose (GObject *object)
{
	paint_application_dispose_self (PAINT_APPLICATION (object));
	G_OBJECT_CLASS (paint_application_parent_class)->dispose (object);
}

/**
 * クラスのメンバーを破棄します。
 */
void paint_application_dispose_self (PaintApplication *application)
{
	if (application->settings)
	{
		paint_settings_wirte_file (application->settings);
		g_object_unref (application->settings);
		application->settings = NULL;
	}
}

/**
 * 現在の設定を返します。
 */
PaintSettings *paint_application_get_settings (PaintApplication *application)
{
	return application->settings;
}

/**
 * クラスの新しいインスタンスを初期化します。
 */
void paint_application_init (PaintApplication *application)
{
	paint_action_map_add_action_entries (G_ACTION_MAP (application), paint_application_action_entries, G_N_ELEMENTS (paint_application_action_entries), application);
}

/**
 * 指定したアプリケーションに新しいメニュー バーを設定します。
 */
void paint_application_init_menubar (GtkApplication *application)
{
	GMenuModel *menu;
	menu = paint_menu_new_menubar ();

	if (menu)
	{
		gtk_application_set_menubar (application, menu);
		g_object_unref (menu);
	}
}

/**
 * クラスの新しいインスタンスを初期化します。
 */
GApplication *paint_application_new (void)
{
	return g_object_new (PAINT_TYPE_APPLICATION, PAINT_PROPERTY_APPLICATION_ID, paint_application_id, PAINT_PROPERTY_FLAGS, G_APPLICATION_HANDLES_OPEN, NULL);
}

/**
 * この関数は GTK アプリケーションがファイルを開く場合に呼び出されます。
 * ファイル名を指定して新しいウィンドウを作成します。
 * @param files ファイルの配列。
 * @param n_files ファイルの配列の要素数。
 * @param hint 呼び出し元はヒントを提供します。
 */
void paint_application_open (GApplication *application, GFile **files, gint n_files, const gchar *hint)
{
	int index;

	for (index = 0; index < n_files; index++, files++)
	{
		paint_application_show_window_from_file (application, *files);
	}
}

/**
 * 各アクションにキーボード ショートカットを割り当てます。
 */
void paint_application_set_accels_for_action (GtkApplication *application, const PaintAccelEntry *entries, gint n_entries)
{
	const char *accels [2];
	int index;
	accels [1] = NULL;

	for (index = 0; index < n_entries; index++, entries++)
	{
		*accels = entries->accel;
		gtk_application_set_accels_for_action (application, entries->detailed_action_name, accels);
	}
}

/**
 * バージョン情報ダイアログ ボックスを表示します。
 */
void paint_application_show_about_dialog (void)
{
	GtkWidget *dialog;
	dialog = paint_about_dialog_new (NULL);

	if (dialog)
	{
		gtk_window_present (GTK_WINDOW (dialog));
	}
}

/**
 * 新しいドキュメント ウィンドウを作成します。
 */
void paint_application_show_window (GApplication *application)
{
	GtkWidget *window;
	window = paint_application_window_new (application);

	if (window)
	{
		gtk_window_present (GTK_WINDOW (window));
	}
}

/**
 * 新しいドキュメント ウィンドウを作成します。
 */
void paint_application_show_window_from_file (GApplication *application, GFile *file)
{
	char *path;
	GtkWidget *window;
	path = g_file_get_path (file);

	if (path)
	{
		window = paint_application_window_new_from_file (application, path);

		if (window)
		{
			gtk_window_present (GTK_WINDOW (window));
		}

		g_free (path);
	}
}

/**
 * この関数は GTK アプリケーションが開始された場合に呼び出されます。
 */
void paint_application_startup (GApplication *application)
{
	G_APPLICATION_CLASS (paint_application_parent_class)->startup (application);
	text_language_init ();
	paint_application_startup_gtk (GTK_APPLICATION (application));
	paint_application_startup_self (PAINT_APPLICATION (application));
}

/**
 * アプリケーション用メニューを初期化します。
 */
void paint_application_startup_gtk (GtkApplication *application)
{
	paint_application_init_menubar (application);
	paint_application_set_accels_for_action (application, paint_application_accel_entries, G_N_ELEMENTS (paint_application_accel_entries));
}

/**
 * 設定ファイルを読み込みます。
 */
void paint_application_startup_self (PaintApplication *application)
{
	application->settings = paint_settings_new ();

	if (application->settings)
	{
		paint_settings_read_file (application->settings);
	}
}
