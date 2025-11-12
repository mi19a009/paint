/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "share.h"
#include "viewer.h"
#define APPLICATION_ID    "com.github.mi19a009.viewer"
#define APPLICATION_FLAGS G_APPLICATION_HANDLES_OPEN
#define RESOURCE_FORMAT   "/com/github/mi19a009/viewer/%s"
#define TITLE_OPEN        _("Open File")

/*******************************************************************************
* @brief アプリケーションのメイン エントリ ポイントです。
*/
int
main (int argc, char *argv [])
{
	GApplication *application;
	int exitcode;
	msginit ();
	application = viewer_application_new (APPLICATION_ID, APPLICATION_FLAGS);
	exitcode = g_application_run (application, argc, argv);
	g_object_unref (application);
	return exitcode;
}

/*******************************************************************************
* @brief ファイルを開くダイアログを表示します。
*/
void
viewer_choose_file (GtkWindow *parent, GFile *initial_file, GAsyncReadyCallback callback, gpointer user_data)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();
	gtk_file_dialog_set_initial_file (dialog, initial_file);
	gtk_file_dialog_set_modal (dialog, TRUE);
	gtk_file_dialog_set_title (dialog, TITLE_OPEN);
	gtk_file_dialog_open (dialog, parent, NULL, callback, user_data);
	g_object_unref (dialog);
}

/*******************************************************************************
* @brief リソースへのパスを取得します。
*/
int
viewer_get_resource_path (char *buffer, size_t maxlen, const char *name)
{
	return g_snprintf (buffer, maxlen, RESOURCE_FORMAT, name);
}

/*******************************************************************************
* @brief アプリケーションの環境設定を取得します。
*/
GSettings *
viewer_get_settings (void)
{
	return g_settings_new (APPLICATION_ID);
}
