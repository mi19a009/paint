/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include "share.h"
#define ALERT_FORMAT    "%s"
#define MSGINIT_CODESET "UTF-8"
#define MSGINIT_DOMAIN  "msg"
#define MSGINIT_FORMAT  "%s"
#define MSGINIT_LINK    "/proc/self/exe"
#define MSGINIT_LOCALE  ""
#define MSGINIT_PATH    "locale"

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
