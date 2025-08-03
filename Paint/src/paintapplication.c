/**
	@file paintapplication.c
	@author Taichi Murakami
	@date 2025
	@brief Paint Application クラスを定義します。
*/
#include "paint.h"
#define PAINT_APPLICATION_PROPERTY_FLAGS_NAME   "flags"
#define PAINT_APPLICATION_PROPERTY_ID_NAME      "application-id"
#define PAINT_APPLICATION_RESOURCE_ABOUT_DIALOG "about"
#define SIGNAL_ACTIVATE                         "activate"

typedef enum _PaintApplicationProperty PaintApplicationProperty;

/** オブジェクト プロパティ */
enum _PaintApplicationProperty
{
	PAINT_APPLICATION_PROPERTY_0,
};

static void       paint_application_activate                (GApplication *);
static void       paint_application_activate_about          (GSimpleAction *, GVariant *, gpointer);
static void       paint_application_class_init              (PaintApplicationClass *);
static void       paint_application_class_init_application  (GApplicationClass *);
static void       paint_application_init                    (PaintApplication *);
static void       paint_application_open                    (GApplication *, GFile **, int, const char *);
static void       paint_application_present_modal_window    (GtkApplication *, GtkWindow *);
static void       paint_application_show_about_dialog       (GtkApplication *);
static GtkWidget *paint_application_show_application_window (GApplication *);
static void       paint_application_startup                 (GApplication *);

/** メニュー アクション */
static const GActionEntry paint_application_action_entries [] =
{
	{ "about", paint_application_activate_about, NULL, NULL, NULL },
};

/** Paint Application クラス */
G_DEFINE_TYPE (PaintApplication, paint_application, GTK_TYPE_APPLICATION);
#define PAINT_APPLICATION_N_ACTION_ENTRIES G_N_ELEMENTS (paint_application_action_entries)

/**
	アプリケーションを活性化します。
	@memberof PaintApplication
*/
void paint_application_activate (GApplication *application)
{
	G_APPLICATION_CLASS (paint_application_parent_class)->activate (application);
	paint_application_show_application_window (application);
}

/**
	メニュー アクション: バージョン情報。
	@memberof PaintApplication
*/
void paint_application_activate_about (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	paint_application_show_about_dialog (GTK_APPLICATION (self));
}

/**
	クラスを初期化します。
	@memberof PaintApplication
*/
void paint_application_class_init (PaintApplicationClass *application)
{
	paint_application_class_init_application (G_APPLICATION_CLASS (application));
}

/**
	Application クラスを初期化します。
	@memberof PaintApplication
*/
void paint_application_class_init_application (GApplicationClass *application)
{
	application->activate = paint_application_activate;
	application->open = paint_application_open;
	application->startup = paint_application_startup;
}

/**
	クラスのインスタンスを初期化します。
	@memberof PaintApplication
*/
void paint_application_init (PaintApplication *application)
{
	g_action_map_add_action_entries (G_ACTION_MAP (application), paint_application_action_entries, G_N_ELEMENTS (paint_application_action_entries), application);
}

/**
	クラスのインスタンスを作成します。
	@return 作成したインスタンスを返す。
	@memberof PaintApplication
*/
GApplication *paint_application_new (void)
{
	return g_object_new (PAINT_TYPE_APPLICATION,
		PAINT_APPLICATION_PROPERTY_ID_NAME, PAINT_APPLICATION_ID,
		PAINT_APPLICATION_PROPERTY_FLAGS_NAME, G_APPLICATION_HANDLES_OPEN,
		NULL);
}

/**
	指定したファイルを開きます。
	@param files 開くべきファイルの配列。
	@param n_files 開くべきファイルの配列の要素数。
	@param hint 非 NULL であるヒントまたは空文字列。
	@memberof PaintApplication
*/
void paint_application_open (GApplication *application, GFile **files, int n_files, const char *hint)
{
	G_APPLICATION_CLASS (paint_application_parent_class)->open (application, files, n_files, hint);
}

/**
	モーダル ウィンドウを表示します。
	@param window モーダル ウィンドウ。
	@memberof PaintApplication
*/
void paint_application_present_modal_window (GtkApplication *application, GtkWindow *window)
{
	GtkWindow *parent;
	parent = gtk_application_get_active_window (application);

	if (parent)
	{
		gtk_window_set_destroy_with_parent (window, TRUE);
		gtk_window_set_modal (window, TRUE);
		gtk_window_set_transient_for (window, parent);
	}

	gtk_window_present (window);
}

/**
	ダイアログ ウィンドウを表示します。
	@memberof PaintApplication
*/
void paint_application_show_about_dialog (GtkApplication *application)
{
	GtkBuilder *builder;
	GtkWindow *window;
	builder = gtk_builder_new_from_resource (PAINT_RESOURCE_PATH_DIALOG);

	if (builder)
	{
		window = GTK_WINDOW (gtk_builder_get_object (builder, PAINT_APPLICATION_RESOURCE_ABOUT_DIALOG));

		if (window)
		{
			paint_application_present_modal_window (application, window);
		}

		g_object_unref (builder);
	}
}

/**
	アプリケーション ウィンドウを表示します。
	@memberof PaintApplication
*/
GtkWidget *paint_application_show_application_window (GApplication *application)
{
	GtkWidget *window;
	window = paint_application_window_new (application);

	if (window)
	{
		gtk_window_present (GTK_WINDOW (window));
	}

	return window;
}

/**
	アプリケーションを開始します。
	@memberof PaintApplication
*/
void paint_application_startup (GApplication *application)
{
	G_APPLICATION_CLASS (paint_application_parent_class)->startup (application);
}
