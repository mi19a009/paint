/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define ACTION_SHOW_MENUBAR "show-menubar"
#define RESOURCE_ABOUT "/com/github/mi19a009/paint/gtk/about.ui"
#define RESOURCE_ABOUT_DIALOG "dialog"
#define RESOURCE_FILTERS "/com/github/mi19a009/paint/gtk/filters.ui"
#define RESOURCE_FILTERS_FILES "file_filters"
#define SIGNAL_DESTROY "destroy"
#define TEXT_TITLE_OPEN "open"

typedef struct _PaintApplicationAccelEntry PaintApplicationAccelEntry;

/* Paint Application クラスのインスタンス */
struct _PaintApplication
{
	GtkApplication parent_instance;
};

/* Paint Application クラスのショートカット */
struct _PaintApplicationAccelEntry
{
	const char *name;
	const char *accel;
};

static void     paint_application_activate               (GApplication *self);
static void     paint_application_activate_about         (GSimpleAction *action, GVariant *parameter, gpointer self);
static void     paint_application_activate_open          (GSimpleAction *action, GVariant *parameter, gpointer self);
static void     paint_application_activate_radio         (GSimpleAction *action, GVariant *parameter, gpointer self);
static void     paint_application_activate_toggle        (GSimpleAction *action, GVariant *parameter, gpointer self);
static void     paint_application_change_menubar         (GSimpleAction *action, GVariant *state, gpointer self);
static void     paint_application_change_tool            (GSimpleAction *action, GVariant *state, gpointer self);
static void     paint_application_change_tool_width      (GSimpleAction *action, GVariant *state, gpointer self);
static void     paint_application_class_init             (PaintApplicationClass *self);
static void     paint_application_class_init_application (GApplicationClass *self);
static void     paint_application_choose_file_open       (GtkApplication *self);
static gboolean paint_application_get_show_menubar       (GActionGroup *self);
static void     paint_application_init                   (PaintApplication *self);
static void     paint_application_init_accels            (GtkApplication *self);
static void     paint_application_open                   (GApplication *self, GFile **files, gint n_files, const gchar *hint);
static void     paint_application_respond_open           (GObject *other, GAsyncResult *result, gpointer self);
static void     paint_application_show_about             (GtkApplication *self);
static void     paint_application_show_window            (GApplication *self, GFile *file);
static void     paint_application_startup                (GApplication *self);

/* キーボード ショートカット */
static PaintApplicationAccelEntry paint_application_accel_entries [] =
{
	{ "app.new", "<Ctrl>N" },
	{ "app.open", "<Ctrl>O" },
};

/* メニュー アクション */
static GActionEntry paint_application_action_entries [] =
{
	{ "about", paint_application_activate_about, NULL, NULL, NULL },
	{ "open", paint_application_activate_open, NULL, NULL, NULL },
	{ ACTION_SHOW_MENUBAR, paint_application_activate_toggle, NULL, "false", paint_application_change_menubar },
	{ "tool", paint_application_activate_radio, "s", "'pencil'", paint_application_change_tool },
	{ "tool-width", paint_application_activate_radio, "i", "1", paint_application_change_tool_width },
};

/*******************************************************************************
Paint Application クラス:
アプリケーションを表します。
アプリケーションは各ドキュメント ウィンドウが共有する情報を格納します。
*/ G_DEFINE_FINAL_TYPE (PaintApplication, paint_application, GTK_TYPE_APPLICATION);
#define PAINT_APPLICATION_N_ACCEL_ENTRIES (G_N_ELEMENTS (paint_application_accel_entries))
#define PAINT_APPLICATION_N_ACTION_ENTRIES (G_N_ELEMENTS (paint_application_action_entries))
#define PAINT_APPLICATION_PROPERTY_APPLICATION_ID_NAME "application-id"
#define PAINT_APPLICATION_PROPERTY_FLAGS_NAME          "flags"

/*******************************************************************************
新しいウィンドウを表示します。
*/ static void
paint_application_activate (GApplication *self)
{
	G_APPLICATION_CLASS (paint_application_parent_class)->activate (self);
	paint_application_show_window (self, NULL);
}

/*******************************************************************************
バージョン情報ダイアログ ボックスを表示します。
*/ static void
paint_application_activate_about (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	paint_application_show_about (GTK_APPLICATION (self));
}

/*******************************************************************************
ファイルを開くダイアログ ボックスを表示します。
*/ static void
paint_application_activate_open (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	paint_application_choose_file_open (GTK_APPLICATION (self));
}

/*******************************************************************************
メニュー項目の選択を変更します。
*/ static void
paint_application_activate_radio (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	g_action_change_state (G_ACTION (action), parameter);
}

/*******************************************************************************
メニュー項目の選択を切り替えます。
*/ static void
paint_application_activate_toggle (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	GAction *target;
	GVariant *state;
	target = G_ACTION (action);
	state = g_action_get_state (target);
	g_action_change_state (target, g_variant_new_boolean (!g_variant_get_boolean (state)));
	g_variant_unref (state);
}

/*******************************************************************************
メニュー バーの表示を切り替えます。
*/ static void
paint_application_change_menubar (GSimpleAction *action, GVariant *state, gpointer self)
{
	GList *windows;
	GtkApplicationWindow *window;
	gboolean menubar;
	g_simple_action_set_state (action, state);
	windows = gtk_application_get_windows (self);
	menubar = g_variant_get_boolean (state);

	for (windows = gtk_application_get_windows (self); windows; windows = windows->next)
	{
		window = GTK_APPLICATION_WINDOW (windows->data);
		gtk_application_window_set_show_menubar (window, menubar);
	}
}

/*******************************************************************************
ペイント ツールを変更します。
*/ static void
paint_application_change_tool (GSimpleAction *action, GVariant *state, gpointer self)
{
	GList *windows;
	GtkWindow *window;
	GValue value;
	g_simple_action_set_state (action, state);
	windows = gtk_application_get_windows (self);

	for (windows = gtk_application_get_windows (self); windows; windows = windows->next)
	{
		window = windows->data;
		g_value_init (&value, G_TYPE_STRING);
		g_value_set_string (&value, g_variant_get_string (state, NULL));
		g_object_set_property (G_OBJECT (window), "tool-label", &value);
		g_value_unset (&value);
	}
}

/*******************************************************************************
ペイント ツールの幅を変更します。
*/ static void
paint_application_change_tool_width (GSimpleAction *action, GVariant *state, gpointer self)
{
	GList *windows;
	GtkWindow *window;
	GValue value;
	g_simple_action_set_state (action, state);
	windows = gtk_application_get_windows (self);

	for (windows = gtk_application_get_windows (self); windows; windows = windows->next)
	{
		window = windows->data;
		g_value_init (&value, G_TYPE_INT);
		g_value_set_int (&value, g_variant_get_int32 (state));
		g_object_set_property (G_OBJECT (window), "tool-width", &value);
		g_value_unset (&value);
	}
}

/*******************************************************************************
クラスを初期化します。
*/ static void
paint_application_class_init (PaintApplicationClass *self)
{
	paint_application_class_init_application (G_APPLICATION_CLASS (self));
}

/*******************************************************************************
GApplication クラスを初期化します。
*/ static void
paint_application_class_init_application (GApplicationClass *self)
{
	self->activate = paint_application_activate;
	self->open = paint_application_open;
	self->startup = paint_application_startup;
}

/*******************************************************************************
ファイルを開くダイアログ ボックスを表示します。
*/ static void
paint_application_choose_file_open (GtkApplication *self)
{
	GtkFileDialog *dialog;
	GtkBuilder *builder;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		gtk_file_dialog_set_title (dialog, TEXT_TITLE_OPEN);
		builder = gtk_builder_new_from_resource (RESOURCE_FILTERS);

		if (builder)
		{
			gtk_file_dialog_set_filters (dialog, G_LIST_MODEL (gtk_builder_get_object (builder, RESOURCE_FILTERS_FILES)));
			g_object_unref (builder);
		}

		gtk_file_dialog_open (dialog, gtk_application_get_active_window (self), NULL, paint_application_respond_open, self);
		g_object_unref (dialog);
	}
}

/*******************************************************************************
メニュー バーを表示する場合は TRUE を返します。
*/ static gboolean
paint_application_get_show_menubar (GActionGroup *self)
{
	GVariant *state;
	gboolean result;
	state = g_action_group_get_action_state (self, ACTION_SHOW_MENUBAR);

	if (state)
	{
		result = g_variant_get_boolean (state);
		g_variant_unref (state);
	}
	else
	{
		result = FALSE;
	}

	return result;
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/ static void
paint_application_init (PaintApplication *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), paint_application_action_entries, PAINT_APPLICATION_N_ACTION_ENTRIES, self);
}

/*******************************************************************************
キーボード ショートカットを追加します。
*/ static void
paint_application_init_accels (GtkApplication *self)
{
	const PaintApplicationAccelEntry *entries;
	const char *accels [2];
	int index;
	entries = paint_application_accel_entries;
	accels [1] = NULL;

	for (index = 0; index < PAINT_APPLICATION_N_ACCEL_ENTRIES; index++)
	{
		*accels = entries->accel;
		gtk_application_set_accels_for_action (self, entries->name, accels);
		entries++;
	}
}

/*******************************************************************************
クラスのインスタンスを作成します。
*/ GApplication *
paint_application_new (const char *application_id, GApplicationFlags flags)
{
	return g_object_new (PAINT_TYPE_APPLICATION,
		PAINT_APPLICATION_PROPERTY_APPLICATION_ID_NAME, application_id,
		PAINT_APPLICATION_PROPERTY_FLAGS_NAME, flags,
		NULL);
}

/*******************************************************************************
ドキュメント ファイルを指定して新しいウィンドウを作成します。
*/ static void
paint_application_open (GApplication *self, GFile **files, gint n_files, const gchar *hint)
{
	gint index;

	for (index = 0; index < n_files; index++)
	{
		paint_application_show_window (G_APPLICATION (self), files [index]);
	}
}

/*******************************************************************************
ダイアログからファイルを指定して新しいウィンドウを作成します。
*/ static void
paint_application_respond_open (GObject *other, GAsyncResult *result, gpointer self)
{
	GFile *file;
	file = gtk_file_dialog_open_finish (GTK_FILE_DIALOG (other), result, NULL);

	if (file)
	{
		paint_application_show_window (self, file);
		g_object_unref (file);
	}
}

/*******************************************************************************
バージョン情報ダイアログ ボックスを表示します。
*/ static void
paint_application_show_about (GtkApplication *self)
{
	GtkBuilder *builder;
	GtkWindow *dialog;
	builder = gtk_builder_new_from_resource (RESOURCE_ABOUT);

	if (builder)
	{
		dialog = GTK_WINDOW (gtk_builder_get_object (builder, RESOURCE_ABOUT_DIALOG));

		if (dialog)
		{
			g_signal_connect_swapped (dialog, SIGNAL_DESTROY, G_CALLBACK (gtk_window_destroy), dialog);
			gtk_window_set_destroy_with_parent (dialog, TRUE);
			gtk_window_set_modal (dialog, TRUE);
			gtk_window_set_transient_for (dialog, gtk_application_get_active_window (self));
			gtk_window_present (dialog);
		}

		g_object_unref (builder);
	}
}

/*******************************************************************************
新しいウィンドウを表示します。
ウィンドウが開くファイルを指定します。
*/ static void
paint_application_show_window (GApplication *self, GFile *file)
{
	GtkWidget *window;
	gboolean menubar;
	menubar = paint_application_get_show_menubar (G_ACTION_GROUP (self));
	window = paint_window_new (self, menubar, file);

	if (window)
	{
		gtk_window_present (GTK_WINDOW (window));
	}
}

/*******************************************************************************
アプリケーションを開始します。
*/ static void
paint_application_startup (GApplication *self)
{
	G_APPLICATION_CLASS (paint_application_parent_class)->startup (self);
	paint_application_init_accels (GTK_APPLICATION (self));
}
