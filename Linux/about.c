/*
Copyright 2025 Taichi Murakami.
バージョン情報ダイアログ ボックスを作成する方法を提供します。
*/

#include <gtk/gtk.h>
#include "paint.h"

#define CCH_VERSION             32
#define FORMAT_VERSION          "GTK %u.%u.%u"
#define LOGO_ICON_NAME          "application-x-executable"
#define SIGNAL_DESTROY          "destroy"

static void setup_about_dialog (GtkAboutDialog *dialog);
static void setup_about_dialog_version (GtkAboutDialog *dialog);
static void setup_window (GtkWindow *dialog, GtkWindow *parent);

/* 新しいバージョン情報ダイアログ ボックスを作成します。 */
void paint_about_dialog_show (GtkWindow *parent)
{
	GtkWidget *dialog;
	dialog = gtk_about_dialog_new ();

	if (dialog)
	{
		setup_window (GTK_WINDOW (dialog), parent);
		setup_about_dialog (GTK_ABOUT_DIALOG (dialog));
		setup_about_dialog_version (GTK_ABOUT_DIALOG (dialog));
		g_signal_connect_swapped(dialog, SIGNAL_DESTROY, G_CALLBACK (gtk_window_destroy), dialog);
		gtk_window_present (GTK_WINDOW (dialog));
	}
}

/* 指定したダイアログ ボックスに値を設定します。 */
static void setup_about_dialog (GtkAboutDialog *dialog)
{
	gtk_about_dialog_set_authors (dialog, paint_application_authors);
	gtk_about_dialog_set_copyright (dialog, paint_application_copyright);
	gtk_about_dialog_set_license_type (dialog, GTK_LICENSE_APACHE_2_0);
	gtk_about_dialog_set_logo_icon_name (dialog, LOGO_ICON_NAME);
	gtk_about_dialog_set_program_name (dialog, paint_application_name);
	gtk_about_dialog_set_website (dialog, paint_application_website);
}

/* 現在のバージョンを説明する文字列を作成します。 */
static void setup_about_dialog_version (GtkAboutDialog *dialog)
{
	guint major, minor, micro;
	char version [CCH_VERSION];
	major = gtk_get_major_version ();
	minor = gtk_get_minor_version ();
	micro = gtk_get_micro_version ();
	snprintf (version, CCH_VERSION, FORMAT_VERSION, major, minor, micro);
	gtk_about_dialog_set_version (dialog, version);
}

/* 指定したウィンドウに値を設定します。 */
static void setup_window (GtkWindow *dialog, GtkWindow *parent)
{
	gtk_window_set_destroy_with_parent (dialog, TRUE);
	gtk_window_set_modal (dialog, TRUE);
	gtk_window_set_transient_for (dialog, parent);
}
