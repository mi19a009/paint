/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "text.h"
#define TEXT_COPYRIGHT "Copyright © 2025 Taichi Murakami."
#define TEXT_VERSION   "2025"
#define TEXT_WEBSITE   "https://mi19a009.github.io/paint/"

static void
paint_about_dialog_init (GtkAboutDialog *);
static void
paint_about_dialog_init_logo (GtkAboutDialog *dialog);

/* バージョン情報ダイアログの内容 */
static const char *
TEXT_AUTHORS [] = { "Taichi Murakami", NULL };

/*******************************************************************************
 * @brief バージョン情報ダイアログ ボックスを初期化する。
 ******************************************************************************/
void
paint_about_dialog_init (GtkAboutDialog *dialog)
{
	gtk_about_dialog_set_authors        (dialog, TEXT_AUTHORS);
	gtk_about_dialog_set_copyright      (dialog, TEXT_COPYRIGHT);
	gtk_about_dialog_set_license_type   (dialog, GTK_LICENSE_APACHE_2_0);
	gtk_about_dialog_set_program_name   (dialog, TEXT_TITLE);
	gtk_about_dialog_set_version        (dialog, TEXT_VERSION);
	gtk_about_dialog_set_website        (dialog, TEXT_WEBSITE);
	paint_about_dialog_init_logo (dialog);
}

void
paint_about_dialog_init_logo (GtkAboutDialog *dialog)
{
	GdkTexture *logo;
	logo = gdk_texture_new_from_resource ("/com/github/mi19a009/paint/resource/pencil.png");

	if (logo)
	{
		gtk_about_dialog_set_logo (dialog, GDK_PAINTABLE (logo));
		g_object_unref (logo);
	}
}

/*******************************************************************************
 * @brief バージョン情報ダイアログ ボックスを作成する。
 * @return 作成したウィンドウを返す。
 ******************************************************************************/
GtkWidget *
paint_about_dialog_new (void)
{
	GtkWidget *dialog;
	dialog = gtk_about_dialog_new ();

	if (dialog)
	{
		paint_about_dialog_init (GTK_ABOUT_DIALOG (dialog));
	}

	return dialog;
}

/*******************************************************************************
 * @brief バージョン情報ダイアログ ボックスを表示する。
 * @param self 親ウィンドウ。
 ******************************************************************************/
void
paint_activate_about (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	GtkWindow *parent;
	GtkWidget *dialog;
	parent = GTK_WINDOW (self);

	if (parent)
	{
		dialog = paint_about_dialog_new ();

		if (dialog)
		{
			paint_present_modal (GTK_WINDOW (dialog), parent);
		}
	}
}

/*******************************************************************************
 * @brief モーダル ウィンドウを表示する。
 * @param modal モーダル ウィンドウ。
 * @param parent 親ウィンドウ。
 ******************************************************************************/
void
paint_present_modal (GtkWindow *modal, GtkWindow *parent)
{
	g_signal_connect_swapped           (modal, "destroy", G_CALLBACK (gtk_window_destroy), modal);
	gtk_window_set_destroy_with_parent (modal, TRUE);
	gtk_window_set_modal               (modal, TRUE);
	gtk_window_set_transient_for       (modal, parent);
	gtk_window_present                 (modal);
}
