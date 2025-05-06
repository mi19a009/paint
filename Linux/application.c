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
#define FORMAT_SETTINGS_INT     "%s=%d\n"
#define PROPERTY_APPLICATION_ID "application-id"
#define PROPERTY_FLAGS          "flags"
#define SETTINGS_RADIX          10
#define SETTINGS_WINDOW_HEIGHT  "WINDOW_HEIGHT"
#define SETTINGS_WINDOW_WIDTH   "WINDOW_WIDTH"
#define UI_MENU                 "menu"
#define UI_TEMPLATE             "application.ui"

typedef struct _PaintApplication
{
	GtkApplication super;
	int window_height;
	int window_width;
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
static void init_accel (GtkApplication *application);
static void init_application_class (GApplicationClass *application);
static void init_menu (GtkApplication *application);
static void init_object_class (GObjectClass *object);
static void load_settings (Self *self);
static void open (GApplication *application, GFile **files, gint n_files, const gchar *hint);
static void read_settings (GInputStream *input, Self *self);
static int read_settings_int (const char *value);
static gchar *read_settings_line (GDataInputStream *input, size_t *separator);
static void show_document_window (GApplication *application);
static void show_document_window_from_file (GApplication *application, GFile *file);
static void startup (GApplication *application);
static void store_settings (Self *self);
static void write_settings (GOutputStream *output, Self *self);
static void write_settings_int (GOutputStream *output, const char *key, int value);

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
static const PaintActionEntry action_entries[] =
{
	{ ACTION_NEW, activate_new },
	{ ACTION_QUIT, activate_quit },
	{ NULL },
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
	paint_action_map_add_action_entries (G_ACTION_MAP (self), action_entries);
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

/* この関数は現在のインスタンスが破棄される場合に呼び出されます。 */
static void dispose (GObject *object)
{
	store_settings (PAINT_APPLICATION (object));
	G_OBJECT_CLASS (SUPER_CLASS)->dispose (object);
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

/* クラスのコールバック関数を登録します。 */
static void init_application_class (GApplicationClass *application)
{
	application->startup = startup;
	application->activate = activate;
	application->open = open;
}

/* 現在のアプリケーションに新しいメニューを設定します。 */
static void init_menu (GtkApplication *application)
{
	GtkBuilder *builder;
	GMenuModel *menu;
	char name [CCH_RESOURCE_NAME];
	format_resource_name (name, CCH_RESOURCE_NAME, UI_TEMPLATE);
	builder = gtk_builder_new_from_resource (name);

	if (builder)
	{
		menu = G_MENU_MODEL (gtk_builder_get_object (builder, UI_MENU));

		if (menu)
		{
			gtk_application_set_menubar (application, menu);
		}

		g_object_unref (builder);
	}
}

/* クラスのコールバック関数を登録します。 */
static void init_object_class (GObjectClass *object)
{
	object->dispose = dispose;
}

static void load_settings (Self *self)
{
	gchar *path;
	GFile *file;
	GFileInputStream *input;
	path = paint_get_settings_filename ();

	if (path)
	{
		file = g_file_new_for_path (path);

		if (file)
		{
			input = g_file_read (file, NULL, NULL);

			if (input)
			{
				read_settings (G_INPUT_STREAM (input), self);
				g_object_unref (input);
			}

			g_object_unref (file);
		}

		g_free (path);
	}
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

/* 設定ファイルを読み込みます。 */
static void read_settings (GInputStream *input, Self *self)
{
	GDataInputStream *data;
	gchar *line;
	const char *value;
	size_t separator;
	data = g_data_input_stream_new (input);

	if (data)
	{
		while (TRUE)
		{
			separator = 0;
			line = read_settings_line (data, &separator);

			if (line)
			{
				if (separator)
				{
					line [separator] = 0;
					value = line + separator + 1;

					if (!strcmp (line, SETTINGS_WINDOW_HEIGHT))
					{
						self->window_height = read_settings_int (value);
					}
					else if (!strcmp (line, SETTINGS_WINDOW_WIDTH))
					{
						self->window_width = read_settings_int (value);
					}
				}

				g_free (line);
			}
			else
			{
				break;
			}
		}

		g_object_unref (data);
	}
}

/* 設定ファイルから値を読み込みます。 */
static int read_settings_int (const char *value)
{
	return (int) strtol (value, NULL, SETTINGS_RADIX);
}

/* 設定ファイルから行を読み込みます。 */
static gchar *read_settings_line (GDataInputStream *input, size_t *separator)
{
	gchar *found, *line;
	line = g_data_input_stream_read_line (input, NULL, NULL, NULL);

	if (line)
	{
		found = strchr (line, '=');

		if (found)
		{
			*separator = found - line;
		}
	}

	return line;
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
	load_settings (PAINT_APPLICATION (application));
}

/* 設定ファイルを書き込みます。 */
static void store_settings (Self *self)
{
	gchar *path;
	GFile *file;
	GFileOutputStream *output;
	path = paint_get_settings_filename ();

	if (path)
	{
		file = g_file_new_for_path (path);
	
		if (file)
		{
			output = g_file_create (file, 0, NULL, NULL);
		
			if (output)
			{
				write_settings (G_OUTPUT_STREAM (output), self);
				g_object_unref (output);
			}
	
			g_object_unref (file);
		}
	
		g_free (path);
	}
}

/* 設定ファイルに値を書き込みます。 */
static void write_settings (GOutputStream *output, Self *self)
{
	write_settings_int (output, SETTINGS_WINDOW_HEIGHT, self->window_height);
	write_settings_int (output, SETTINGS_WINDOW_WIDTH, self->window_width);
}

/* 設定ファイルに値を書き込みます。 */
static void write_settings_int (GOutputStream *output, const char *key, int value)
{
	g_output_stream_printf (output, NULL, NULL, NULL, FORMAT_SETTINGS_INT, key, value);
}
