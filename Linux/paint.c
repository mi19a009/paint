/*
 * Copyright 2025 Taichi Murakami.
 * GTK を利用するプログラムについて、特定のクラスに属さない関数を実装します。
 */

#include "paint.h"
#define PAINT_FORMAT_LIBRARY_GLIB               "GLIB %u.%u.%u"
#define PAINT_FORMAT_LIBRARY_GTK                "GTK %u.%u.%u"
#define PAINT_TOOLBAR_SPACING                   1

typedef enum _PaintCreditSectionLibrary
{
	PAINT_CREDIT_SECTION_LIBRARY_GLIB,
	PAINT_CREDIT_SECTION_LIBRARY_GTK,
	PAINT_CREDIT_SECTION_LIBRARY_MAX,
	PAINT_CREDIT_SECTION_LIBRARY_PEOPLE,
} PaintCreditSectionLibrary;

static void             paint_about_dialog_init                (GtkAboutDialog *dialog);
static void             paint_about_dialog_init_version        (GtkAboutDialog *dialog);
static void             paint_menu_append_item                 (GMenu *menu, const char *label, const char *action, const char *target);
static GMenuModel      *paint_menu_new_edit                    (void);
static GMenuModel      *paint_menu_new_file                    (void);
static GMenuModel      *paint_menu_new_help                    (void);
static GMenuModel      *paint_menu_new_tool                    (void);
static GMenuModel      *paint_menu_new_view                    (void);
static void             paint_toolbar_click                    (GtkButton *button, gpointer action);
static void             paint_toolbar_init                     (GtkBox *toolbar);
static void             paint_window_set_modal                 (GtkWindow *dialog, GtkWindow *parent);

static const char *paint_toolbar_classes [] = { GTK_CSS_FLAT, NULL };

/**
 * 新しいアプリケーションを開始します。
 */
int main(int argc, char **argv)
{
	GApplication *application;
	int result;
	application = paint_application_new ();

	if (application)
	{
		result = g_application_run (application, argc, argv);
		g_object_unref (application);
	}
	else
	{
		result = -1;
	}

	return result;
}

/**
 * 指定したダイアログ ボックスに値を設定します。
 */
void paint_about_dialog_init (GtkAboutDialog *dialog)
{
	gtk_about_dialog_set_authors (dialog, paint_application_authors);
	gtk_about_dialog_set_copyright (dialog, paint_application_copyright);
	gtk_about_dialog_set_license_type (dialog, GTK_LICENSE_APACHE_2_0);
	gtk_about_dialog_set_logo_icon_name (dialog, PAINT_LOGO_ICON_NAME);
	gtk_about_dialog_set_program_name (dialog, TEXT (text_title));
	gtk_about_dialog_set_version (dialog, paint_application_version);
	gtk_about_dialog_set_website (dialog, paint_application_website);
	paint_about_dialog_init_version (dialog);
}

/**
 * 現在のバージョンを説明する文字列を設定します。
 */
void paint_about_dialog_init_version (GtkAboutDialog *dialog)
{
	char *people [PAINT_CREDIT_SECTION_LIBRARY_PEOPLE];
	guint major, minor, micro;
	char version [PAINT_CCH_VERSION * PAINT_CREDIT_SECTION_LIBRARY_MAX];
	people [PAINT_CREDIT_SECTION_LIBRARY_MAX] = NULL;

	for (major = 0; major < PAINT_CREDIT_SECTION_LIBRARY_MAX; major++)
	{
		people [major] = version + major * PAINT_CCH_VERSION;
	}

	major = glib_major_version;
	minor = glib_minor_version;
	micro = glib_micro_version;
	g_snprintf (people [PAINT_CREDIT_SECTION_LIBRARY_GLIB], PAINT_CCH_VERSION, PAINT_FORMAT_LIBRARY_GLIB, major, minor, micro);
	major = gtk_get_major_version ();
	minor = gtk_get_minor_version ();
	micro = gtk_get_micro_version ();
	g_snprintf (people [PAINT_CREDIT_SECTION_LIBRARY_GTK], PAINT_CCH_VERSION, PAINT_FORMAT_LIBRARY_GTK, major, minor, micro);
	gtk_about_dialog_add_credit_section (dialog, TEXT (text_title_library), (const char **) people);
}

/**
 * 新しいバージョン情報ダイアログ ボックスを作成します。
 * @param parent ダイアログ ボックスの親。
 * @return ダイアログ ボックス。
 */
GtkWidget *paint_about_dialog_new (GtkWindow *parent)
{
	GtkWidget *dialog;
	dialog = gtk_about_dialog_new ();

	if (dialog)
	{
		paint_window_set_modal (GTK_WINDOW (dialog), parent);
		paint_about_dialog_init (GTK_ABOUT_DIALOG (dialog));
		g_signal_connect_swapped(dialog, PAINT_SIGNAL_DESTROY, G_CALLBACK (gtk_window_destroy), dialog);
	}

	return dialog;
}

/**
 * メニュー項目が選択された際の行動を決定します。
 * メニュー項目が選択された際に呼び出される関数を登録します。
 * @param entries コールバック関数の配列。
 * @param n_entries コールバック関数の配列の要素数。
 */
void paint_action_map_add_action_entries (GActionMap *action_map, const PaintActionEntry *entries, gint n_entries, gpointer user_data)
{
	GSimpleAction *action;
	GVariant *state;
	int index;

	for (index = 0; index < n_entries; index++, entries++)
	{
		switch ((ptrdiff_t) entries->state)
		{
		case 0: /* Push Button */
			action = g_simple_action_new (entries->name, NULL);
			break;
		case (ptrdiff_t) PAINT_ACTION_ENTRY_STATEFUL: /* Check Box */
			state = g_variant_new_boolean (FALSE);
			if (state) action = g_simple_action_new_stateful (entries->name, NULL, state);
			else continue;
			break;
		default: /* Radio Button */
			state = g_variant_new_string (entries->state);
			if (state) action = g_simple_action_new_stateful (entries->name, G_VARIANT_TYPE_STRING, state);
			else continue;
			break;
		}
		if (action)
		{
			g_signal_connect (action, PAINT_SIGNAL_ACTIVATE, G_CALLBACK (entries->activate), user_data);
			g_action_map_add_action (action_map, G_ACTION (action));
			g_object_unref (action);
		}
	}
}

/**
 * 指定したリソース名から新しい Builder を作成します。
 */
GtkBuilder *paint_builder_new_from_resource (const char *resource_name)
{
	GtkBuilder *builder;
	gchar *path;
	path = paint_resource_get_path (resource_name);

	if (path)
	{
		builder = gtk_builder_new_from_resource (path);
		g_free (path);
	}
	else
	{
		builder = NULL;
	}

	return builder;
}

/**
 * ファイル ダイアログ用フィルターのコレクションを作成します。
 * @param entries フィルターの配列。
 * @param n_entries フィルターの配列の要素数。
 * @return フィルターのコレクションを返します。
 */
GListModel *paint_file_filter_list_new (const PaintFileFilter *entries, int n_entries)
{
	GListStore *store;
	GtkFileFilter *filter;
	int n;
	store = g_list_store_new (GTK_TYPE_FILE_FILTER);

	if (store) for (n = 0; n < n_entries; n++)
	{
		filter = paint_file_filter_new (entries++);

		if (filter)
		{
			g_list_store_append (store, filter);
			g_object_unref (filter);
		}
	}

	return G_LIST_MODEL (store);
}

/**
 * ファイル ダイアログ用フィルターを作成します。
 * @param entries フィルターを説明します。
 * @return フィルターを返します。
 */
GtkFileFilter *paint_file_filter_new (const PaintFileFilter *entry)
{
	GtkFileFilter *filter;
	const char *const *patterns;
	filter = gtk_file_filter_new ();

	if (filter)
	{
		if (entry->name)
		{
			gtk_file_filter_set_name (filter, entry->name);
		}
		if (entry->patterns)
		{
			for (patterns = entry->patterns; *patterns; patterns++)
			{
				gtk_file_filter_add_pattern (filter, *patterns);
			}
		}
	}

	return filter;
}

/**
 * 指定したメニューに新しい項目を追加します。
 * ラジオ ボタン用メニュー項目を作成します。
 * @param menu: 指定したメニュー。
 * @param label: ラジオ ボタンの見出し。
 * @param action: このラジオ ボタンが属するグループ。
 * @param target: このラジオ ボタンに名前を付けます。
 */
void paint_menu_append_item (GMenu *menu, const char *label, const char *action, const char *target)
{
	GMenuItem *item;
	GVariant *parameter;
	item = g_menu_item_new (label, NULL);

	if (item)
	{
		parameter = g_variant_new_string (target);

		if (parameter)
		{
			g_menu_item_set_action_and_target_value (item, action, parameter);
			g_menu_append_item (menu, item);
		}

		g_object_unref (item);
	}
}

/**
 * 新しいメニューを作成します。
 * @return 編集メニューを返します。
 */
GMenuModel *paint_menu_new_edit (void)
{
	GMenu *menu, *section;
	menu = g_menu_new ();

	if (menu)
	{
		section = g_menu_new ();

		if (section)
		{
			g_menu_append (section, TEXT (text_label_undo), PAINT_COMMAND_UNDO);
			g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
			g_object_unref (section);
		}
	}

	return G_MENU_MODEL (menu);
}

/**
 * 新しいメニューを作成します。
 * @return ファイル メニューを返します。
 */
GMenuModel *paint_menu_new_file (void)
{
	GMenu *menu, *section;
	menu = g_menu_new ();

	if (menu)
	{
		section = g_menu_new ();

		if (section)
		{
			g_menu_append (section, TEXT (text_label_new), PAINT_COMMAND_NEW);
			g_menu_append (section, TEXT (text_label_open), PAINT_COMMAND_OPEN);
			g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
			g_object_unref (section);
		}

		section = g_menu_new ();

		if (section)
		{
			g_menu_append (section, TEXT (text_label_save), PAINT_COMMAND_SAVE);
			g_menu_append (section, TEXT (text_label_save_as), PAINT_COMMAND_SAVE_AS);
			g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
			g_object_unref (section);
		}

		section = g_menu_new ();

		if (section)
		{
			g_menu_append (section, TEXT (text_label_quit), PAINT_COMMAND_CLOSE);
			g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
			g_object_unref (section);
		}
	}

	return G_MENU_MODEL (menu);
}

/**
 * 新しいメニューを作成します。
 * @return ヘルプ メニューを返します。
 */
GMenuModel *paint_menu_new_help (void)
{
	GMenu *menu, *section;
	menu = g_menu_new ();

	if (menu)
	{
		section = g_menu_new ();

		if (section)
		{
			g_menu_append (section, TEXT (text_label_about), PAINT_COMMAND_ABOUT);
			g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
			g_object_unref (section);
		}
	}

	return G_MENU_MODEL (menu);
}

/**
 * 新しいメニューを作成します。
 * @return メニュー バーを返します。
 */
GMenuModel *paint_menu_new_menubar (void)
{
	GMenu *menu;
	GMenuModel *submenu;
	menu = g_menu_new ();

	if (menu)
	{
		submenu = paint_menu_new_file ();

		if (submenu)
		{
			g_menu_append_submenu (menu, TEXT (text_label_file), submenu);
			g_object_unref (submenu);
		}

		submenu = paint_menu_new_edit ();

		if (submenu)
		{
			g_menu_append_submenu (menu, TEXT (text_label_edit), submenu);
			g_object_unref (submenu);
		}

		submenu = paint_menu_new_view ();

		if (submenu)
		{
			g_menu_append_submenu (menu, TEXT (text_label_view), submenu);
			g_object_unref (submenu);
		}

		submenu = paint_menu_new_tool ();

		if (submenu)
		{
			g_menu_append_submenu (menu, TEXT (text_label_tool), submenu);
			g_object_unref (submenu);
		}

		submenu = paint_menu_new_help ();

		if (submenu)
		{
			g_menu_append_submenu (menu, TEXT (text_label_help), submenu);
			g_object_unref (submenu);
		}
	}

	return G_MENU_MODEL (menu);
}

/**
 * 新しいメニューを作成します。
 * @return ツール メニューを返します。
 */
GMenuModel *paint_menu_new_tool (void)
{
	GMenu *menu, *section;
	menu = g_menu_new ();

	if (menu)
	{
		section = g_menu_new ();

		if (section)
		{
			paint_menu_append_item (section, TEXT (text_label_pencil), PAINT_COMMAND_MODE, PAINT_MODE_PENCIL);
			paint_menu_append_item (section, TEXT (text_label_brush), PAINT_COMMAND_MODE, PAINT_MODE_BRUSH);
			paint_menu_append_item (section, TEXT (text_label_eraser), PAINT_COMMAND_MODE, PAINT_MODE_ERASER);
			g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
			g_object_unref (section);
		}
	}

	return G_MENU_MODEL (menu);
}

/**
 * 新しいメニューを作成します。
 * @return 表示メニューを返します。
 */
GMenuModel *paint_menu_new_view (void)
{
	GMenu *menu, *section;
	menu = g_menu_new ();

	if (menu)
	{
		section = g_menu_new ();

		if (section)
		{
			g_menu_append (section, TEXT (text_label_flip_horizontal), PAINT_COMMAND_FLIP_HORIZONTAL);
			g_menu_append (section, TEXT (text_label_flip_vertical), PAINT_COMMAND_FLIP_VERTICAL);
			g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
			g_object_unref (section);
		}
	}

	return G_MENU_MODEL (menu);
}

/**
 * 指定したリソース名からリソースへのパスを作成します。
 */
gchar *paint_resource_get_path (const char *resource_name)
{
	GString *buffer;
	gchar *resource_path;
	buffer = g_string_new (PAINT_RESOURCE_PREFIX);

	if (buffer)
	{
		g_string_append (buffer, resource_name);
		resource_path = g_string_free_and_steal (buffer);
	}
	else
	{
		resource_path = NULL;
	}

	return resource_path;
}

/**
 * ツールバー上のボタンがクリックされた場合はアクションを実行します。
 */
void paint_toolbar_click (GtkButton *button, gpointer action)
{
	gtk_widget_activate_action (GTK_WIDGET (button), action, NULL);
}

/**
 * 指定したウィジェットにボタンを配置します。
 */
void paint_toolbar_init (GtkBox *toolbar)
{
	GtkWidget *button, *previous;
	button = gtk_button_new ();

	if (button) /* New */
	{
		gtk_widget_set_css_classes (button, paint_toolbar_classes);
		gtk_widget_set_tooltip_text (button, TEXT (text_tooltip_new));
		gtk_button_set_icon_name (GTK_BUTTON (button), GTK_ICON_NEW);
		gtk_box_append (toolbar, button);
		g_signal_connect (button, PAINT_SIGNAL_CLICKED, G_CALLBACK (paint_toolbar_click), PAINT_COMMAND_NEW);
	}

	button = gtk_button_new ();

	if (button) /* Open */
	{
		gtk_widget_set_css_classes (button, paint_toolbar_classes);
		gtk_widget_set_tooltip_text (button, TEXT (text_tooltip_open));
		gtk_button_set_icon_name (GTK_BUTTON (button), GTK_ICON_OPEN);
		gtk_box_append (toolbar, button);
		g_signal_connect (button, PAINT_SIGNAL_CLICKED, G_CALLBACK (paint_toolbar_click), PAINT_COMMAND_OPEN);
	}

	button = gtk_separator_new (0);

	if (button)
	{
		gtk_box_append (toolbar, button);
	}

	button = gtk_button_new ();

	if (button) /* Save */
	{
		gtk_widget_set_css_classes (button, paint_toolbar_classes);
		gtk_widget_set_tooltip_text (button, TEXT (text_tooltip_save));
		gtk_button_set_icon_name (GTK_BUTTON (button), GTK_ICON_SAVE);
		gtk_box_append (toolbar, button);
		g_signal_connect (button, PAINT_SIGNAL_CLICKED, G_CALLBACK (paint_toolbar_click), PAINT_COMMAND_SAVE);
	}

	button = gtk_button_new ();

	if (button) /* Print */
	{
		gtk_widget_set_css_classes (button, paint_toolbar_classes);
		gtk_widget_set_tooltip_text (button, TEXT (text_tooltip_print));
		gtk_button_set_icon_name (GTK_BUTTON (button), GTK_ICON_PRINT);
		gtk_box_append (toolbar, button);
		g_signal_connect (button, PAINT_SIGNAL_CLICKED, G_CALLBACK (paint_toolbar_click), PAINT_COMMAND_PRINT);
	}

	button = gtk_separator_new (0);

	if (button)
	{
		gtk_box_append (toolbar, button);
	}

	button = gtk_toggle_button_new ();

	if (button) /* Pencil */
	{
		gtk_widget_set_css_classes (button, paint_toolbar_classes);
		gtk_widget_set_tooltip_text (button, TEXT (text_tooltip_pencil));
		gtk_button_set_icon_name (GTK_BUTTON (button), GTK_ICON_NEW);
		gtk_box_append (toolbar, button);
		g_signal_connect (button, PAINT_SIGNAL_CLICKED, G_CALLBACK (paint_toolbar_click), PAINT_COMMAND_MODE);
	}

	previous = button;
	button = gtk_toggle_button_new ();

	if (button) /* Brush */
	{
		gtk_widget_set_css_classes (button, paint_toolbar_classes);
		gtk_widget_set_tooltip_text (button, TEXT (text_tooltip_brush));
		gtk_button_set_icon_name (GTK_BUTTON (button), GTK_ICON_NEW);
		gtk_toggle_button_set_group (GTK_TOGGLE_BUTTON (button), GTK_TOGGLE_BUTTON (previous));
		gtk_box_append (toolbar, button);
		g_signal_connect (button, PAINT_SIGNAL_CLICKED, G_CALLBACK (paint_toolbar_click), PAINT_COMMAND_MODE);
	}

	previous = button;
	button = gtk_toggle_button_new ();

	if (button) /* Eraser */
	{
		gtk_widget_set_css_classes (button, paint_toolbar_classes);
		gtk_widget_set_tooltip_text (button, TEXT (text_tooltip_eraser));
		gtk_button_set_icon_name (GTK_BUTTON (button), GTK_ICON_NEW);
		gtk_toggle_button_set_group (GTK_TOGGLE_BUTTON (button), GTK_TOGGLE_BUTTON (previous));
		gtk_box_append (toolbar, button);
		g_signal_connect (button, PAINT_SIGNAL_CLICKED, G_CALLBACK (paint_toolbar_click), PAINT_COMMAND_MODE);
	}

	button = gtk_separator_new (0);

	if (button)
	{
		gtk_box_append (toolbar, button);
	}
}

/**
 * 新しいツールバーを作成します。
 */
GtkWidget *paint_toolbar_new (void)
{
	GtkWidget *child, *toolbar;
	toolbar = gtk_box_new (GTK_ORIENTATION_VERTICAL, PAINT_TOOLBAR_SPACING);

	if (toolbar)
	{
		child = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, PAINT_TOOLBAR_SPACING);

		if (child)
		{
			paint_toolbar_init (GTK_BOX (child));
			gtk_box_append (GTK_BOX (toolbar), child);
		}

		child = gtk_separator_new (0);

		if (child)
		{
			gtk_box_append (GTK_BOX (toolbar), child);
		}
	}

	return toolbar;
}

/**
 * 現在のウィンドウをモーダルに設定します。
 */
void paint_window_set_modal (GtkWindow *dialog, GtkWindow *parent)
{
	gtk_window_set_destroy_with_parent (dialog, TRUE);
	gtk_window_set_modal (dialog, TRUE);
	gtk_window_set_transient_for (dialog, parent);
}
