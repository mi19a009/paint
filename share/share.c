/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include "share.h"
#define ALERT_FORMAT           "%s"
#define MSGINIT_CODESET        "UTF-8"
#define MSGINIT_DOMAIN         "msg"
#define MSGINIT_FORMAT         "%s"
#define MSGINIT_LINK           "/proc/self/exe"
#define MSGINIT_LOCALE         ""
#define MSGINIT_PATH           "locale"
#define PIXBUF_BITS_PER_SAMPLE 8
#define PIXBUF_OVERALL_ALPHA   255
#define PIXBUF_SCALE_X         1.0
#define PIXBUF_SCALE_Y         1.0

/*******************************************************************************
* @brief 指定したエラーを説明する文字列を表示します。
*/
void
alert (GtkWindow *parent, const GError *error)
{
	GtkAlertDialog *dialog;
	dialog = gtk_alert_dialog_new (ALERT_FORMAT, error->message);
	gtk_alert_dialog_show (dialog, parent);
	g_object_unref (dialog);
}

/*******************************************************************************
* @brief メッセージ カタログを初期化します。
*/
void
msginit (void)
{
	gchar *path;
	GError *error;
	GPathBuf buffer;
	error = NULL;
	setlocale (LC_ALL, MSGINIT_LOCALE);
	path = g_file_read_link (MSGINIT_LINK, &error);

	if (path)
	{
		g_path_buf_init_from_path (&buffer, path);
		g_free (path);
		g_path_buf_pop (&buffer);
		g_path_buf_pop (&buffer);
		g_path_buf_push (&buffer, MSGINIT_PATH);
		path = g_path_buf_clear_to_path (&buffer);
		bindtextdomain (MSGINIT_DOMAIN, path);
		g_free (path);
		bind_textdomain_codeset (MSGINIT_DOMAIN, MSGINIT_CODESET);
		textdomain (MSGINIT_DOMAIN);
	}
	if (error)
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_ERROR, MSGINIT_FORMAT, error->message);
		g_error_free (error);
	}
}

/*******************************************************************************
* @brief 指定した画像のチャンネルを並び替えます。
*/
void
pixcpy (const guchar *source, guchar *destination, int width, int height, int stride)
{
	const guchar *src;
	guchar *dest;
	int x, y;

	for (y = 0; y < height; y++)
	{
		src = source;
		dest = destination;

		for (x = 0; x < width; x++)
		{
			*(dest++) = src [2];
			*(dest++) = src [1];
			*(dest++) = src [0];
			*(dest++) = src [3];
			src += 4;
		}

		source += stride;
		destination += stride;
	}
}

/*******************************************************************************
* @brief 指定した画像ファイルを読み込みます。
*/
GdkPixbuf *
pixload (GFile *file, GError **error)
{
	GdkPixbuf *source, *destination;
	GFileInputStream *stream;
	int width, height;
	destination = NULL;
	stream = g_file_read (file, NULL, error);

	if (stream)
	{
		source = gdk_pixbuf_new_from_stream (G_INPUT_STREAM (stream), NULL, error);
		g_object_unref (stream);

		if (source)
		{
			width = gdk_pixbuf_get_width (source);
			height = gdk_pixbuf_get_height (source);
			destination = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, PIXBUF_BITS_PER_SAMPLE, width, height);
			gdk_pixbuf_composite (source, destination, 0, 0, width, height, 0, 0, PIXBUF_SCALE_X, PIXBUF_SCALE_Y, GDK_INTERP_NEAREST, PIXBUF_OVERALL_ALPHA);
			g_object_unref (source);
		}
	}

	return destination;
}
