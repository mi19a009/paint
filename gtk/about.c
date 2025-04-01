/*
Copyright 2025 Taichi Murakami.
バージョン情報ダイアログ ボックスを実装します。
*/

#include <gtk/gtk.h>
#include "source.h"

static const gchar *Authors[] = { "Taichi Murakami", NULL };
static const gchar *Comments = "＼（＾ｏ＾）／";
static const gchar *Copyright = "Copyright © 2025 Taichi Murakami.";
static const gchar *Version = "2025";
static const gchar *Website = "https://github.com/mi19a009/paint";

void AboutDialog(void)
{
	GtkWidget *dialog;
	dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), Authors);
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), Comments);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), Copyright);
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_APACHE_2_0);
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), TitleTexts[LANG_JA]);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), Version);
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), Website);
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog), Website);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
