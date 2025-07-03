/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "text.h"
#define PAINT_RESOURCE_PATH "/com/github/mi19a009/paint/resource/%s"

typedef GtkFileFilter *(*PaintFileFilterNew) (void);

static void
paint_about_dialog_init (GtkAboutDialog *);
static void
paint_about_dialog_init_logo (GtkAboutDialog *);
static GtkFileFilter *
paint_file_filter_new_all (void);
static GtkFileFilter *
paint_file_filter_new_images (void);
static void
paint_window_present_modal (GtkWindow *modal, GtkWindow *parent);

G_DEFINE_ENUM_TYPE (PaintVisibility, paint_visibility,
	G_DEFINE_ENUM_VALUE (PAINT_VISIBILITY_EDIT,    "Edit"),
	G_DEFINE_ENUM_VALUE (PAINT_VISIBILITY_VISIBLE, "Visible"),
	G_DEFINE_ENUM_VALUE (PAINT_VISIBILITY_HIDDEN,  "Hidden"));
G_DEFINE_BOXED_TYPE (PaintSurface, paint_surface,
	(GBoxedCopyFunc) cairo_surface_reference,
	(GBoxedFreeFunc) cairo_surface_destroy);

static const char *
TEXT_AUTHORS [] = { "Taichi Murakami", NULL };
static const char *
TEXT_COPYRIGHT = "Copyright © 2025 Taichi Murakami.";
static const char *
TEXT_VERSION = "2025";
static const char *
TEXT_WEBSITE = "https://mi19a009.github.io/paint/";

/*******************************************************************************
 * @brief バージョン情報ダイアログ ボックスを初期化する。
 * @param dialog ダイアログ ボックス。
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
	paint_about_dialog_init_logo        (dialog);
}

/*******************************************************************************
 * @brief ダイアログ ボックス用画像を読み込む。
 * @param dialog ダイアログ ボックス。
 ******************************************************************************/
void
paint_about_dialog_init_logo (GtkAboutDialog *dialog)
{
	GdkTexture *logo;
	char resource_path [PAINT_RESOURCE_PATH_CCH];
	paint_resource_format_path (resource_path, PAINT_RESOURCE_PATH_CCH, "pencil.png");
	logo = gdk_texture_new_from_resource (resource_path);

	if (logo)
	{
		gtk_about_dialog_set_logo (dialog, GDK_PAINTABLE (logo));
		g_object_unref (logo);
	}
}

/*******************************************************************************
 * @brief バージョン情報ダイアログ ボックスを表示する。
 * @param parent 親ウィンドウ。
 ******************************************************************************/
void
paint_about_dialog_show (GtkWindow *parent)
{
	GtkWidget *dialog;
	dialog = gtk_about_dialog_new ();

	if (dialog)
	{
		paint_about_dialog_init (GTK_ABOUT_DIALOG (dialog));
		paint_window_present_modal (GTK_WINDOW (dialog), parent);
	}
}

/*******************************************************************************
 * @brief ファイルを開くダイアログ ボックスを表示する。
 * @param parent 親ウィンドウ。
 ******************************************************************************/
void
paint_file_dialog_show_open (GtkWindow *parent, GAsyncReadyCallback callback, gpointer user_data)
{
	GtkFileDialog *dialog;
	GListModel *filters;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		filters = paint_file_filter_list_new ();

		if (filters)
		{
			gtk_file_dialog_set_filters (dialog, filters);
			g_object_unref (filters);
		}

		gtk_file_dialog_set_title (dialog, "Open File");
		gtk_file_dialog_open (dialog, parent, NULL, callback, user_data);
		g_object_unref (dialog);
	}
}

/*******************************************************************************
 * @brief ファイル フィルターを格納するリストを作成する。
 * @return 作成したリストを返す。
 ******************************************************************************/
GListModel *
paint_file_filter_list_new (void)
{
	const PaintFileFilterNew functions [] = { paint_file_filter_new_images, paint_file_filter_new_all };
	GListStore *list;
	GtkFileFilter *filter;
	unsigned index;
	list = g_list_store_new (GTK_TYPE_FILE_FILTER);

	if (list)
	{
		for (index = 0; index < G_N_ELEMENTS (functions); index++)
		{
			filter = functions [index] ();

			if (filter)
			{
				g_list_store_append (list, filter);
				g_object_unref (filter);
			}
		}
	}

	return G_LIST_MODEL (list);
}

/*******************************************************************************
 * @brief すべてのファイル フィルターを作成する。
 * @return 作成したフィルターを返す。
 ******************************************************************************/
GtkFileFilter *
paint_file_filter_new_all (void)
{
	GtkFileFilter *filter;
	filter = gtk_file_filter_new ();

	if (filter)
	{
		gtk_file_filter_set_name (filter, "All Files");
		gtk_file_filter_add_pattern (filter, "*.*");
	}

	return filter;
}

/*******************************************************************************
 * @brief 画像ファイル フィルターを作成する。
 * @return 作成したフィルターを返す。
 ******************************************************************************/
GtkFileFilter *
paint_file_filter_new_images (void)
{
	GtkFileFilter *filter;
	filter = gtk_file_filter_new ();

	if (filter)
	{
		gtk_file_filter_set_name (filter, "Image Files");
		gtk_file_filter_add_pattern (filter, "*.png");
	}

	return filter;
}

/*******************************************************************************
 * @brief モーダル ウィンドウを表示する。
 * @param modal モーダル ウィンドウ。
 * @param parent 親ウィンドウ。
 ******************************************************************************/
void
paint_window_present_modal (GtkWindow *modal, GtkWindow *parent)
{
	g_signal_connect_swapped           (modal, "destroy", G_CALLBACK (gtk_window_destroy), modal);
	gtk_window_set_destroy_with_parent (modal, TRUE);
	gtk_window_set_modal               (modal, TRUE);
	gtk_window_set_transient_for       (modal, parent);
	gtk_window_present                 (modal);
}

/*******************************************************************************
 * @brief リソースへのパスを作成する。
 * @param resource_path リソースへのパス。
 * @param resource_path_cch 配列の要素数。
 * @param resource_name リソースの名前。
 ******************************************************************************/
int
paint_resource_format_path (char *resource_path, size_t resource_path_cch, const char *resource_name)
{
	return snprintf (resource_path, resource_path_cch, PAINT_RESOURCE_PATH, resource_name);
}
