/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "text.h"
#define PAINT_BITS_PER_SAMPLE 8
#define PAINT_RESOURCE_PATH "/com/github/mi19a009/paint/resource/%s"
#define PAINT_SURFACE_N_CHANNELS 4

typedef GtkFileFilter *(*PaintFileFilterNew) (void);
typedef void (*PaintSurfaceInitChannel) (guchar *, const guchar *, int, int, int);

static GtkFileFilter *
paint_file_filter_new_all (void);
static GtkFileFilter *
paint_file_filter_new_images (void);
static void
paint_surface_init_channel_1 (guchar *, const guchar *, int, int, int);
static void
paint_surface_init_channel_3 (guchar *, const guchar *, int, int, int);
static void
paint_surface_init_channel_4 (guchar *, const guchar *, int, int, int);

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

int
muldiv (int number, int numerator, int denominator)
{
	gint64 value;

	if (denominator)
	{
		value = number * (gint64) numerator;
		number = (int) (value / denominator);
	}
	else
	{
		number = 0;
	}

	return number;
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

PaintMemento *
paint_memento_copy (PaintMemento *source)
{
	PaintMemento *destination;

	if (source)
	{
		destination = g_malloc (sizeof (PaintMemento));

		if (destination)
		{
			destination->surface = source->surface ? cairo_surface_reference (source->surface) : NULL;
			destination->layer = source->layer ? g_object_ref (source->layer) : NULL;
		}
	}
	else
	{
		destination = NULL;
	}

	return destination;
}

void
paint_memento_free (PaintMemento *memento)
{
	if (memento)
	{
		if (memento->surface)
		{
			cairo_surface_destroy (memento->surface);
		}
		if (memento->layer)
		{
			g_object_unref (memento->layer);
		}

		g_free (memento);
	}
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

void
paint_surface_init_channel_1 (guchar *data, const guchar *pixels, int width, int height, int stride)
{
	const guchar *p;
	int x, y, z;

	for (y = 0; y < height; y++)
	{
		p = pixels + y * stride;

		for (x = 0; x < width; x++)
		{
			z = *(p++);
			*(data++) = z;
			*(data++) = z;
			*(data++) = z;
			*(data++) = G_MAXUINT8;
		}
	}
}

void
paint_surface_init_channel_3 (guchar *data, const guchar *pixels, int width, int height, int stride)
{
	const guchar *p;
	int x, y;

	for (y = 0; y < height; y++)
	{
		p = pixels + y * stride;

		for (x = 0; x < width; x++)
		{
			*(data++) = p [2];
			*(data++) = p [1];
			*(data++) = p [0];
			*(data++) = G_MAXUINT8;
			p += 4;
		}
	}
}

void
paint_surface_init_channel_4 (guchar *data, const guchar *pixels, int width, int height, int stride)
{
	const guchar *p;
	int x, y;

	for (y = 0; y < height; y++)
	{
		p = pixels + y * stride;

		for (x = 0; x < width; x++)
		{
			/* B */ *(data++) = p [2];
			/* G */ *(data++) = p [1];
			/* R */ *(data++) = p [0];
			/* A */ *(data++) = p [3];
			p += 4;
		}
	}
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
	GdkPixbuf *pixbuf;
	const guchar *pixels;
	guchar *data;
	PaintSurfaceInitChannel init;
	int width, height, stride, n_channels;
	surface = NULL;
	input = g_file_read (file, NULL, NULL);

	if (input)
	{
		pixbuf = gdk_pixbuf_new_from_stream (G_INPUT_STREAM (input), NULL, NULL);

		if (pixbuf)
		{
			width = gdk_pixbuf_get_width (pixbuf);
			height = gdk_pixbuf_get_height (pixbuf);
			//width = 256;
			//height = 256;
			data = g_malloc (width * height * (size_t) PAINT_SURFACE_N_CHANNELS);

			if (data)
			{
				pixels = gdk_pixbuf_get_pixels (pixbuf);
				stride = gdk_pixbuf_get_rowstride (pixbuf);
				n_channels = gdk_pixbuf_get_n_channels (pixbuf);

				switch (n_channels)
				{
				case 3:
					init = paint_surface_init_channel_3;
					break;
				case 4:
					init = paint_surface_init_channel_4;
					break;
				default:
					init = paint_surface_init_channel_1;
					break;
				}

				init (data, pixels, width, height, stride);
				surface = cairo_image_surface_create_for_data (data, CAIRO_FORMAT_ARGB32, width, height, stride);
				//g_free (data);
			}

			g_object_unref (pixbuf);
		}

		g_object_unref (input);
	}

	return surface;
}
