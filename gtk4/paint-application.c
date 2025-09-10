/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define ACTION_ABOUT        "about"
#define ACTION_OPEN         "open"
#define ACTION_SAVE         "save"
#define ACTION_SAVE_AS      "save-as"
#define ACTION_SHOW_MENUBAR "show-menubar"
#define ACTION_TOOL         "tool"
#define ACTION_TOOL_ERASER  "tool-eraser"
#define ACTION_TOOL_PENCIL  "tool-pencil"
#define ACTION_TOOL_WIDTH   "tool-width"
#define MENU_ACCEL  "accel"
#define MENU_ACTION "action"
#define RESOURCE_ABOUT         "/com/github/mi19a009/paint/gtk/about.ui"
#define RESOURCE_ABOUT_DIALOG  "dialog"
#define RESOURCE_FILTERS       "/com/github/mi19a009/paint/gtk/filters.ui"
#define RESOURCE_FILTERS_FILES "file_filters"
#define SIGNAL_DESTROY "destroy"
#define TEXT_TITLE_OPEN "Open"
#define TEXT_TITLE_SAVE "Spen"

typedef struct _PaintApplicationAccelEntry PaintApplicationAccelEntry;

/* Paint Application クラスのインスタンス */
struct _PaintApplication
{
	GtkApplication parent_instance;
	PaintTool     *tool;
};

/* Paint Application クラスのショートカット */
struct _PaintApplicationAccelEntry
{
	const char *name;
	const char *accel;
};

static void     paint_application_activate               (GApplication *self);
static void     paint_application_activate_about         (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     paint_application_activate_boolean       (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     paint_application_activate_open          (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     paint_application_activate_save          (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     paint_application_activate_save_as       (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     paint_application_activate_tool          (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     paint_application_activate_tool_eraser   (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     paint_application_activate_tool_pencil   (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     paint_application_activate_tool_width    (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     paint_application_change_menubar         (GSimpleAction *action, GVariant *state, gpointer user_data);
static void     paint_application_change_tool            (GSimpleAction *action, GVariant *state, gpointer user_data);
static void     paint_application_change_tool_width      (GSimpleAction *action, GVariant *state, gpointer user_data);
static void     paint_application_class_init             (PaintApplicationClass *this_class);
static void     paint_application_class_init_application (GApplicationClass *this_class);
static void     paint_application_class_init_object      (GObjectClass *this_class);
static void     paint_application_choose_file_open       (GtkApplication *self);
static void     paint_application_choose_file_save       (GtkApplication *self);
static void     paint_application_destroy                (PaintApplication *self);
static void     paint_application_dispose                (GObject *self);
static gboolean paint_application_get_show_menubar       (GActionGroup *self);
static void     paint_application_init                   (PaintApplication *self);
static void     paint_application_init_menu              (GtkApplication *self, GMenuModel *model);
static void     paint_application_init_menu_attributes   (GtkApplication *self, GMenuModel *model, gint item_index);
static void     paint_application_init_menu_links        (GtkApplication *self, GMenuModel *model, gint item_index);
static void     paint_application_open                   (GApplication *self, GFile **files, gint n_files, const gchar *hint);
static void     paint_application_respond_open           (GObject *dialog, GAsyncResult *result, gpointer user_data);
static void     paint_application_respond_save           (GObject *dialog, GAsyncResult *result, gpointer user_data);
static void     paint_application_set_tool               (PaintApplication *self, PaintTool *tool);
static void     paint_application_show_about             (GtkApplication *self);
static void     paint_application_show_window            (GApplication *self, GFile *file);
static void     paint_application_startup                (GApplication *self);
static void     paint_application_update_menubar         (GtkApplication *self);
static void     paint_application_update_tool            (PaintApplication *self);

/* メニュー アクション */
static GActionEntry paint_application_action_entries [] =
{
	{ ACTION_ABOUT, paint_application_activate_about, NULL, NULL, NULL },
	{ ACTION_OPEN, paint_application_activate_open, NULL, NULL, NULL },
	{ ACTION_SAVE, paint_application_activate_save, NULL, NULL, NULL },
	{ ACTION_SAVE_AS, paint_application_activate_save_as, NULL, NULL, NULL },
	{ ACTION_SHOW_MENUBAR, paint_application_activate_boolean, NULL, "false", paint_application_change_menubar },
	{ ACTION_TOOL, paint_application_activate_tool, "s", "'pencil'", paint_application_change_tool },
	{ ACTION_TOOL_ERASER, paint_application_activate_tool_eraser, NULL, NULL, NULL },
	{ ACTION_TOOL_PENCIL, paint_application_activate_tool_pencil, NULL, NULL, NULL },
	{ ACTION_TOOL_WIDTH, paint_application_activate_tool_width, "i", "1", paint_application_change_tool_width },
};

/*******************************************************************************
Paint Application クラス:
アプリケーションを表します。
アプリケーションは各ドキュメント ウィンドウが共有する情報を格納します。
*/ G_DEFINE_FINAL_TYPE (PaintApplication, paint_application, GTK_TYPE_APPLICATION);
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
paint_application_activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	paint_application_show_about (GTK_APPLICATION (user_data));
}

/*******************************************************************************
メニュー項目の選択を切り替えます。
*/ static void
paint_application_activate_boolean (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GAction *target;
	GVariant *state;
	target = G_ACTION (action);
	state = g_action_get_state (target);
	g_action_change_state (target, g_variant_new_boolean (!g_variant_get_boolean (state)));
	g_variant_unref (state);
}

/*******************************************************************************
ファイルを開くダイアログ ボックスを表示します。
*/ static void
paint_application_activate_open (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	paint_application_choose_file_open (GTK_APPLICATION (user_data));
}

/*******************************************************************************
現在のファイルを保存します。
*/ static void
paint_application_activate_save (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
}

/*******************************************************************************
ファイルを保存ダイアログ ボックスを表示します。
*/ static void
paint_application_activate_save_as (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	paint_application_choose_file_save (GTK_APPLICATION (user_data));
}

/*******************************************************************************
指定したツールを選択します。
*/ static void
paint_application_activate_tool (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	char name [16];
	snprintf (name, G_N_ELEMENTS (name), "tool-%s", g_variant_get_string (parameter, NULL));
	g_action_group_activate_action (G_ACTION_GROUP (user_data), name, NULL);
	g_action_change_state (G_ACTION (action), parameter);
}

/*******************************************************************************
消しゴム ツールを変更します。
*/ static void
paint_application_activate_tool_eraser (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	PaintTool *tool;
	PaintApplication *self;
	tool = paint_tool_eraser_new ();

	if (tool)
	{
		self = PAINT_APPLICATION (user_data);
		paint_tool_load (tool, self->tool);
		paint_application_set_tool (self, tool);
	}
}

/*******************************************************************************
えんぴつツールを変更します。
*/ static void
paint_application_activate_tool_pencil (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	PaintTool *tool;
	PaintApplication *self;
	tool = paint_tool_pencil_new ();

	if (tool)
	{
		self = PAINT_APPLICATION (user_data);
		paint_tool_load (tool, self->tool);
		paint_application_set_tool (self, tool);
	}
}

/*******************************************************************************
ツールの幅を変更します。
*/ static void
paint_application_activate_tool_width (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	g_action_change_state (G_ACTION (action), parameter);
}

/*******************************************************************************
メニュー バーの表示を切り替えます。
*/ static void
paint_application_change_menubar (GSimpleAction *action, GVariant *state, gpointer user_data)
{
	g_simple_action_set_state (action, state);
	paint_application_update_menubar (GTK_APPLICATION (user_data));
}

/*******************************************************************************
ペイント ツールを変更します。
*/ static void
paint_application_change_tool (GSimpleAction *action, GVariant *state, gpointer user_data)
{
	g_simple_action_set_state (action, state);
}

/*******************************************************************************
ペイント ツールの幅を変更します。
*/ static void
paint_application_change_tool_width (GSimpleAction *action, GVariant *state, gpointer user_data)
{
	g_simple_action_set_state (action, state);
	paint_tool_set_width (PAINT_APPLICATION (user_data)->tool, g_variant_get_int32 (state));
}

/*******************************************************************************
クラスを初期化します。
*/ static void
paint_application_class_init (PaintApplicationClass *this_class)
{
	paint_application_class_init_object (G_OBJECT_CLASS (this_class));
	paint_application_class_init_application (G_APPLICATION_CLASS (this_class));
}

/*******************************************************************************
Application クラスを初期化します。
*/ static void
paint_application_class_init_application (GApplicationClass *this_class)
{
	this_class->activate = paint_application_activate;
	this_class->open = paint_application_open;
	this_class->startup = paint_application_startup;
}

/*******************************************************************************
Object クラスを初期化します。
*/ static void
paint_application_class_init_object (GObjectClass *this_class)
{
	this_class->dispose = paint_application_dispose;
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
ファイルを保存ダイアログ ボックスを表示します。
*/ static void
paint_application_choose_file_save (GtkApplication *self)
{
	GtkFileDialog *dialog;
	GtkBuilder *builder;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		gtk_file_dialog_set_title (dialog, TEXT_TITLE_SAVE);
		builder = gtk_builder_new_from_resource (RESOURCE_FILTERS);

		if (builder)
		{
			gtk_file_dialog_set_filters (dialog, G_LIST_MODEL (gtk_builder_get_object (builder, RESOURCE_FILTERS_FILES)));
			g_object_unref (builder);
		}

		gtk_file_dialog_save (dialog, gtk_application_get_active_window (self), NULL, paint_application_respond_save, self);
		g_object_unref (dialog);
	}
}

/*******************************************************************************
クラスのプロパティを破棄します。
*/ static void
paint_application_destroy (PaintApplication *self)
{
	g_clear_object (&self->tool);
}

/*******************************************************************************
クラスのインスタンスを破棄します。
*/ static void
paint_application_dispose (GObject *self)
{
	paint_application_destroy (PAINT_APPLICATION (self));
	G_OBJECT_CLASS (paint_application_parent_class)->dispose (self);
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
	self->tool = paint_tool_pencil_new ();
	g_action_map_add_action_entries (G_ACTION_MAP (self), paint_application_action_entries, PAINT_APPLICATION_N_ACTION_ENTRIES, self);
}

/*******************************************************************************
指定したメニューからアクセラレーターを走査します。
この関数は再帰的に呼び出されます。
*/ static void
paint_application_init_menu (GtkApplication *self, GMenuModel *model)
{
	int n_items, index;
	n_items = g_menu_model_get_n_items (model);

	for (index = 0; index < n_items; index++)
	{
		paint_application_init_menu_attributes (self, model, index);
		paint_application_init_menu_links (self, model, index);
	}
}

/*******************************************************************************
指定したメニューからアクセラレーターを取得します。
現在のアプリケーションにアクセラレーターを設定します。
*/ static void
paint_application_init_menu_attributes (GtkApplication *self, GMenuModel *model, gint item_index)
{
	GVariant *accel, *action;
	const char **accels, *buffer [2];
	accel = g_menu_model_get_item_attribute_value (model, item_index, MENU_ACCEL, G_VARIANT_TYPE_STRING);

	if (accel)
	{
		buffer [0] = g_variant_get_string (accel, NULL);
		buffer [1] = NULL;
		accels = buffer;
	}
	else
	{
		accel = g_menu_model_get_item_attribute_value (model, item_index, MENU_ACCEL, G_VARIANT_TYPE_STRING_ARRAY);

		if (accel)
		{
			accels = g_variant_get_strv (accel, NULL);
		}
	}
	if (accel)
	{
		action = g_menu_model_get_item_attribute_value (model, item_index, MENU_ACTION, G_VARIANT_TYPE_STRING);

		if (action)
		{
			gtk_application_set_accels_for_action (self, g_variant_get_string (action, NULL), accels);
			g_variant_unref (action);
		}

		g_variant_unref (accel);
	}
}

/*******************************************************************************
指定したメニューの子を走査します。
*/ static void
paint_application_init_menu_links (GtkApplication *self, GMenuModel *model, gint item_index)
{
	GMenuLinkIter *iterator;
	GMenuModel *child;
	iterator = g_menu_model_iterate_item_links (model, item_index);

	if (iterator)
	{
		while (g_menu_link_iter_next (iterator))
		{
			child = g_menu_link_iter_get_value (iterator);
			paint_application_init_menu (self, child);
			g_object_unref (child);
		}

		g_object_unref (iterator);
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
paint_application_respond_open (GObject *dialog, GAsyncResult *result, gpointer user_data)
{
	GFile *file;
	file = gtk_file_dialog_open_finish (GTK_FILE_DIALOG (dialog), result, NULL);

	if (file)
	{
		paint_application_show_window (G_APPLICATION (user_data), file);
		g_object_unref (file);
	}
}

/*******************************************************************************
ダイアログからファイルを指定してドキュメントを保存します。
*/ static void
paint_application_respond_save (GObject *dialog, GAsyncResult *result, gpointer user_data)
{
	GFile *file;
	file = gtk_file_dialog_save_finish (GTK_FILE_DIALOG (dialog), result, NULL);

	if (file)
	{
		g_object_unref (file);
	}
}

/*******************************************************************************
ツールを設定します。
オブジェクトの参照数を増やします。
*/ static void
paint_application_set_tool (PaintApplication *self, PaintTool *tool)
{
	if (self->tool != tool)
	{
		if (self->tool)
		{
			g_object_unref (self->tool);
		}
		if (tool)
		{
			self->tool = g_object_ref (tool);
		}
		else
		{
			self->tool = NULL;
		}

		paint_application_update_tool (self);
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
	GtkApplication *application;
	G_APPLICATION_CLASS (paint_application_parent_class)->startup (self);
	application = GTK_APPLICATION (self);
	paint_application_init_menu (application, gtk_application_get_menubar (application));
}

/*******************************************************************************
各ウィンドウにメニュー バーを設定します。
*/ static void
paint_application_update_menubar (GtkApplication *self)
{
	GList *windows;
	GtkWindow *window;
	gboolean menubar;
	menubar = paint_application_get_show_menubar (G_ACTION_GROUP (self));

	for (windows = gtk_application_get_windows (self); windows; windows = windows->next)
	{
		window = windows->data;

		if (GTK_IS_APPLICATION_WINDOW (window))
		{
			gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (window), menubar);
		}
	}
}

/*******************************************************************************
各ウィンドウにツールを設定します。
*/ static void
paint_application_update_tool (PaintApplication *self)
{
	GList *windows;
	GtkWindow *window;

	for (windows = gtk_application_get_windows (GTK_APPLICATION (self)); windows; windows = windows->next)
	{
		window = windows->data;

		if (PAINT_IS_WINDOW (window))
		{
			paint_window_set_tool (PAINT_WINDOW (window), self->tool);
		}
	}
}
