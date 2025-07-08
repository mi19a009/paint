/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "text.h"
#define PAINT_BITS_PER_SAMPLE 8
#define PAINT_RESOURCE_PATH "/com/github/mi19a009/paint/resource/%s"

typedef GtkFileFilter *(*PaintFileFilterNew) (void);

static GtkFileFilter *
paint_file_filter_new_all (void);
static GtkFileFilter *
paint_file_filter_new_images (void);

G_DEFINE_ENUM_TYPE (PaintVisibility, paint_visibility,
	G_DEFINE_ENUM_VALUE (PAINT_VISIBILITY_EDIT,    "Edit"),
	G_DEFINE_ENUM_VALUE (PAINT_VISIBILITY_VISIBLE, "Visible"),
	G_DEFINE_ENUM_VALUE (PAINT_VISIBILITY_HIDDEN,  "Hidden"));
G_DEFINE_BOXED_TYPE (PaintSurface, paint_surface,
	(GBoxedCopyFunc) cairo_surface_reference,
	(GBoxedFreeFunc) cairo_surface_destroy);

const char *
TEXT_AUTHORS [] = { "Taichi Murakami", NULL };
const char *
TEXT_COPYRIGHT = "Copyright © 2025 Taichi Murakami.";
const char *
TEXT_VERSION = "2025";
const char *
TEXT_WEBSITE = "https://mi19a009.github.io/paint/";

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

/*******************************************************************************
 * @brief 指定した画像ファイルを読み込む。
 * @param file 画像ファイル。
 * @return 作成したサーフィスを返す。
 ******************************************************************************/
PaintSurface *
paint_surface_new_from_file (GFile *file)
{
	cairo_surface_t *surface;
	GFileInputStream *input;
	GdkPixbuf *source, *destination;
	guchar *pixels;
	int width, height, stride;
	surface = NULL;
	input = g_file_read (file, NULL, NULL);

	if (input)
	{
		source = gdk_pixbuf_new_from_stream (G_INPUT_STREAM (input), NULL, NULL);

		if (source)
		{
			width = gdk_pixbuf_get_width (source);
			height = gdk_pixbuf_get_height (source);
			destination = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, PAINT_BITS_PER_SAMPLE, width, height);

			if (destination)
			{
				gdk_pixbuf_copy_area (source, 0, 0, width, height, destination, 0, 0);
				pixels = gdk_pixbuf_get_pixels (destination);
				stride = gdk_pixbuf_get_rowstride (destination);
				surface = cairo_image_surface_create_for_data (pixels, CAIRO_FORMAT_ARGB32, width, height, stride);
				g_object_unref (destination);
			}

			g_object_unref (source);
		}

		g_object_unref (input);
	}

	return surface;
}
