/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define PROPERTY_APPLICATION_ID "application-id"
#define PROPERTY_FLAGS          "flags"

typedef struct _PaintApplicationAccelEntry AccelEntry;

/* クラスのインスタンス */
struct _PaintApplication
{
	GtkApplication parent_instance;
};

/* メニュー アクセラレーター */
struct _PaintApplicationAccelEntry
{
	const char  *detailed_action_name;
	const char **accels;
};

static void paint_application_activate               (GApplication *self);
static void paint_application_activate_new           (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void paint_application_class_init             (PaintApplicationClass *this_class);
static void paint_application_class_init_application (GApplicationClass *this_class);
static void paint_application_init                   (PaintApplication *self);
static void paint_application_init_accels            (GtkApplication *self);
static void paint_application_open                   (GApplication *self, GFile **files, gint n_files, const gchar *hint);
static void paint_application_startup                (GApplication *self);

/*******************************************************************************
* Paint Application クラス。
* メニュー、アクセラレーター、およびウィンドウを作成する方法を提供します。
*/
G_DEFINE_FINAL_TYPE (PaintApplication, paint_application, GTK_TYPE_APPLICATION);
static const char *ACCELS_CLOSE        [] = { "<Ctrl>q", NULL };
static const char *ACCELS_HELP_OVERLAY [] = { "<Ctrl>question", "<Ctrl>slash", NULL };
static const char *ACCELS_NEW          [] = { "<Ctrl>n", NULL };

/* メニュー アクセラレーター */
static const AccelEntry
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
	{ "new", paint_application_activate_new, NULL, NULL, NULL },
};

/*******************************************************************************
* @brief アプリケーションを表示します。
*/
static void
paint_application_activate (GApplication *self)
{
	GtkWidget *window;
	window = paint_document_window_new (self);
	gtk_window_present (GTK_WINDOW (window));
}

/*******************************************************************************
* @brief 新しいウィンドウを表示します。
*/
static void
paint_application_activate_new (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkWidget *window;
	window = paint_document_window_new (G_APPLICATION (user_data));
	gtk_window_present (GTK_WINDOW (window));
}

/*******************************************************************************
* @brief クラスを初期化します。
*/
static void
paint_application_class_init (PaintApplicationClass *this_class)
{
	paint_application_class_init_application (G_APPLICATION_CLASS (this_class));
}

/*******************************************************************************
* @brief Application クラスを初期化します。
*/
static void
paint_application_class_init_application (GApplicationClass *this_class)
{
	this_class->activate = paint_application_activate;
	this_class->open = paint_application_open;
	this_class->startup = paint_application_startup;
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
paint_application_init (PaintApplication *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
}

/*******************************************************************************
* @brief アクセラレーターを初期化します。
*/
static void
paint_application_init_accels (GtkApplication *self)
{
	const AccelEntry *entries;
	int n;
	entries = ACCEL_ENTRIES;

	for (n = 0; n < G_N_ELEMENTS (ACCEL_ENTRIES); n++)
	{
		gtk_application_set_accels_for_action (self, entries->detailed_action_name, entries->accels);
		entries++;
	}
}

/*******************************************************************************
* @brief クラスのインスタンスを作成します。
*/
GApplication *
paint_application_new (const char *application_id, GApplicationFlags flags)
{
	return g_object_new (PAINT_TYPE_APPLICATION,
		PROPERTY_APPLICATION_ID, application_id,
		PROPERTY_FLAGS, flags,
		NULL);
}

/*******************************************************************************
* @brief 指定したファイルを開きます。
*/
static void
paint_application_open (GApplication *self, GFile **files, gint n_files, const gchar *hint)
{
	GtkWidget *window;
	int n;

	for (n = 0; n < n_files; n++)
	{
		window = paint_document_window_new (self);
		gtk_window_present (GTK_WINDOW (window));
	}
}

/*******************************************************************************
* @brief アプリケーションを開始します。
*/
static void
paint_application_startup (GApplication *self)
{
	G_APPLICATION_CLASS (paint_application_parent_class)->startup (self);
	paint_application_init_accels (GTK_APPLICATION (self));
}
