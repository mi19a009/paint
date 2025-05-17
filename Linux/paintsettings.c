/*
Copyright 2025 Taichi Murakami.
環境設定を格納するクラスを実装します。
*/

#include "paint.h"
#define FORMAT_SETTINGS_LONG    "%s=%ld\n"
#define SETTINGS_EXTENSION      ".ini"
#define SETTINGS_FILENAME       "/proc/self/exe"
#define SETTINGS_RADIX          10
#define SETTINGS_SEPARATOR      '='
#define SETTINGS_WINDOW_HEIGHT  "WINDOW_HEIGHT"
#define SETTINGS_WINDOW_WIDTH   "WINDOW_WIDTH"
#define SETTINGS_WINDOW_X       "WINDOW_X"
#define SETTINGS_WINDOW_Y       "WINDOW_Y"

static void   paint_settings_class_init        (PaintSettingsClass *settings);
static gchar *paint_settings_get_path          (void);
static void   paint_settings_init              (PaintSettings *settings);
static gchar *paint_settings_read_line         (GDataInputStream *input, size_t *separator);
static long   paint_settings_to_long         (const char *value);
static void   paint_settings_read_settings     (GInputStream *input, PaintSettings *settings);
static void   paint_settings_set_long        (PaintSettings *settings, const char *key, const char *value);
static void   paint_settings_write_long        (GOutputStream *output, const char *key, long value);
static void   paint_settings_write_settings    (GOutputStream *output, PaintSettings *settings);

/* Paint Settings クラス。 */
struct _PaintSettings
{
	GObject super;
	int window_x;
	int window_y;
	int window_width;
	int window_height;
};

G_DEFINE_FINAL_TYPE (PaintSettings, paint_settings, G_TYPE_OBJECT);

/* クラスを初期化します。 */
void paint_settings_class_init (PaintSettingsClass *settings)
{
}

/* 設定ファイルの名前を取得します。 */
gchar *paint_settings_get_path (void)
{
	gchar *link, *path;
	size_t length;
	link = g_file_read_link (SETTINGS_FILENAME, NULL);

	if (link)
	{
		length = strlen (link);
		path = g_renew (char, link, length + G_N_ELEMENTS (SETTINGS_EXTENSION));

		if (path)
		{
			memcpy (path + length, SETTINGS_EXTENSION, G_N_ELEMENTS (SETTINGS_EXTENSION));
		}
		else
		{
			g_free (link);
		}
	}
	else
	{
		path = NULL;
	}

	return path;
}

/* ウィンドウの位置を取得します。 */
void paint_settings_get_window_position (PaintSettings *settings, int *x, int *y)
{
	*x = settings->window_x;
	*y = settings->window_y;
}

/* ウィンドウの大きさを取得します。 */
void paint_settings_get_window_size (PaintSettings *settings, int *width, int *height)
{
	*width = settings->window_width;
	*height = settings->window_height;
}

/* クラスの新しいインスタンスを初期化します。 */
void paint_settings_init (PaintSettings *settings)
{
}

/* クラスの新しいインスタンスを初期化します。 */
PaintSettings *paint_settings_new (void)
{
	return (PaintSettings *) g_object_new (PAINT_TYPE_SETTINGS, NULL);
}

/* 設定ファイルを読み込みます。 */
void paint_settings_read_file (PaintSettings *settings)
{
	gchar *path;
	GFile *file;
	GFileInputStream *input;
	path = paint_settings_get_path ();

	if (path)
	{
		file = g_file_new_for_path (path);
		g_free (path);

		if (file)
		{
			input = g_file_read (file, NULL, NULL);
			g_object_unref (file);

			if (input)
			{
				paint_settings_read_settings (G_INPUT_STREAM (input), settings);
				g_object_unref (input);
			}
		}
	}
}

/* 現在の設定ファイルから行を取得します。 */
gchar *paint_settings_read_line (GDataInputStream *input, size_t *separator)
{
	gchar *line;
	const char *found;
	line = g_data_input_stream_read_line (input, NULL, NULL, NULL);

	if (line)
	{
		found = strchr (line, SETTINGS_SEPARATOR);

		if (found)
		{
			*separator = found - line;
		}
	}

	return line;
}

/* 設定を読み込みます。 */
void paint_settings_read_settings (GInputStream *input, PaintSettings *settings)
{
	GDataInputStream *data;
	gchar *line;
	const char *value;
	size_t separator;
	data = g_data_input_stream_new (input);

	if (data)
	{
		do
		{
			separator = 0;
			line = paint_settings_read_line (data, &separator);

			if (line)
			{
				if (separator)
				{
					line [separator] = 0;
					value = line + separator + 1;
					paint_settings_set_long (settings, line, value);
				}

				g_free (line);
			}
			else
			{
				break;
			}
		} while (TRUE);

		g_object_unref (data);
	}
}

/* 指定した項目に値を設定します。 */
void paint_settings_set_long (PaintSettings *settings, const char *key, const char *value)
{
	if (!strcmp (key, SETTINGS_WINDOW_HEIGHT))
	{
		settings->window_height = (int) paint_settings_to_long (value);
	}
	else if (!strcmp (key, SETTINGS_WINDOW_WIDTH))
	{
		settings->window_width = (int) paint_settings_to_long (value);
	}
}

/* ウィンドウの位置を設定します。 */
void paint_settings_set_window_position (PaintSettings *settings, int x, int y)
{
	settings->window_x = x;
	settings->window_y = y;
}

/* ウィンドウの大きさを設定します。 */
void paint_settings_set_window_size (PaintSettings *settings, int width, int height)
{
	settings->window_width = width;
	settings->window_height = height;
}

/* 指定した文字列が表す値を返します。 */
long paint_settings_to_long (const char *value)
{
	return strtol (value, NULL, SETTINGS_RADIX);
}

/* 設定ファイルを書き込みます。 */
void paint_settings_wirte_file (PaintSettings *settings)
{
	gchar *path;
	GFile *file;
	GFileOutputStream *output;
	path = paint_settings_get_path ();

	if (path)
	{
		file = g_file_new_for_path (path);
		g_free (path);

		if (file)
		{
			output = g_file_create (file, 0, NULL, NULL);
			g_object_unref (file);

			if (output)
			{
				paint_settings_write_settings (G_OUTPUT_STREAM (output), settings);
				g_object_unref (output);
			}
		}
	}
}

/* 指定した値を書き込みます。 */
void paint_settings_write_long (GOutputStream *output, const char *key, long value)
{
	g_output_stream_printf (output, NULL, NULL, NULL, FORMAT_SETTINGS_LONG, key, value);
}

/* 設定を書き込みます。 */
void paint_settings_write_settings (GOutputStream *output, PaintSettings *settings)
{
	paint_settings_write_long (output, SETTINGS_WINDOW_HEIGHT, settings->window_height);
	paint_settings_write_long (output, SETTINGS_WINDOW_WIDTH, settings->window_width);
	paint_settings_write_long (output, SETTINGS_WINDOW_X, settings->window_x);
	paint_settings_write_long (output, SETTINGS_WINDOW_Y, settings->window_y);
}
