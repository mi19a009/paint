/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define BITS_PER_SAMPLE 8

static void             paint_surface_copy_3_channels (guchar *destination, const guchar *source, int width, int height, int stride);
static void             paint_surface_copy_4_channels (guchar *destination, const guchar *source, int width, int height, int stride);
static cairo_surface_t *paint_surface_create          (cairo_surface_t *other, guchar *data, int width, int height, int stride);
static guchar          *paint_surface_create_buffer   (const guchar *source, int width, int height, int stride, int n_channels);

/*******************************************************************************
ペイント ツールを取得します。
*/ PaintToolType
paint_get_tool_type (const char *name)
{
	PaintToolType result;

	if (!strcmp (name, "pencil"))
	{
		result = PAINT_TOOL_TYPE_PENCIL;
	}
	else if (!strcmp (name, "eraser"))
	{
		result = PAINT_TOOL_TYPE_ERASER;
	}
	else
	{
		result = PAINT_TOOL_TYPE_NULL;
	}

	return result;
}

/*******************************************************************************
画素の配列を複製します。
*/ static void
paint_surface_copy_3_channels (guchar *destination, const guchar *source, int width, int height, int stride)
{
	guchar *a;
	const guchar *z;
	int y, x;

	for (y = 0; y < height; y++)
	{
		x = y * stride;
		a = destination + x;
		z = source + x;

		for (x = 0; x < width; x++)
		{
			*(a++) = z [2];
			*(a++) = z [1];
			*(a++) = z [0];
			z += 3;
		}
	}
}

/*******************************************************************************
画素の配列を複製します。
*/ static void
paint_surface_copy_4_channels (guchar *destination, const guchar *source, int width, int height, int stride)
{
	guchar *a;
	const guchar *z;
	int y, x;

	for (y = 0; y < height; y++)
	{
		x = y * stride;
		a = destination + x;
		z = source + x;

		for (x = 0; x < width; x++)
		{
			*(a++) = z [2];
			*(a++) = z [1];
			*(a++) = z [0];
			*(a++) = z [3];
			z += 4;
		}
	}
}

/*******************************************************************************
サーフィスを作成します。
*/ static cairo_surface_t *
paint_surface_create (cairo_surface_t *other, guchar *data, int width, int height, int stride)
{
	cairo_surface_t *result, *image;
	cairo_t *context;
	image = cairo_image_surface_create_for_data (data, CAIRO_FORMAT_ARGB32, width, height, stride);

	if (image)
	{
		result = cairo_surface_create_similar_image (other, CAIRO_FORMAT_ARGB32, width, height);

		if (result)
		{
			context = cairo_create (result);

			if (context)
			{
				cairo_set_antialias (context, CAIRO_ANTIALIAS_NONE);
				cairo_set_operator (context, CAIRO_OPERATOR_SOURCE);
				cairo_set_source_surface (context, image, 0, 0);
				cairo_paint (context);
				cairo_destroy (context);
			}
		}

		cairo_surface_destroy (image);
	}
	else
	{
		result = NULL;
	}

	return result;
}

/*******************************************************************************
画素の配列を作成します。
*/ static guchar *
paint_surface_create_buffer (const guchar *source, int width, int height, int stride, int n_channels)
{
	guchar *result;
	size_t size;
	size = height * (size_t) stride;
	result = g_malloc (size);

	if (result)
	{
		switch (n_channels)
		{
		case 3:
			paint_surface_copy_3_channels (result, source, width, height, stride);
			break;
		case 4:
			paint_surface_copy_4_channels (result, source, width, height, stride);
			break;
		default:
			memcpy (result, source, size);
			break;
		}
	}

	return result;
}

/*******************************************************************************
画像ファイルを読み込みます。
*/ cairo_surface_t *
paint_surface_create_from_file (cairo_surface_t *other, GFile *file)
{
	cairo_surface_t *result;
	GFileInputStream *stream;
	GdkPixbuf *pixbuf;
	guchar *buffer;
	int width, height, stride;
	result = NULL;
	stream = g_file_read (file, NULL, NULL);

	if (stream)
	{
		pixbuf = gdk_pixbuf_new_from_stream (G_INPUT_STREAM (stream), NULL, NULL);

		if (pixbuf)
		{
			if (gdk_pixbuf_get_bits_per_sample (pixbuf) == BITS_PER_SAMPLE)
			{
				width = gdk_pixbuf_get_width (pixbuf);
				height = gdk_pixbuf_get_height (pixbuf);
				stride = gdk_pixbuf_get_rowstride (pixbuf);
				buffer = paint_surface_create_buffer (gdk_pixbuf_get_pixels (pixbuf), width, height, stride, gdk_pixbuf_get_n_channels (pixbuf));

				if (buffer)
				{
					result = paint_surface_create (other, buffer, width, height, stride);
					g_free (buffer);
				}
			}

			g_object_unref (pixbuf);
		}

		g_object_unref (stream);
	}

	return result;
}

const char *
paint_tool_get_icon_name (PaintToolType tool)
{
	const char *result;

	switch (tool)
	{
	case PAINT_TOOL_TYPE_ERASER:
		result = "eraser-symbolic";
		break;
	case PAINT_TOOL_TYPE_PENCIL:
		result = "pencil-symbolic";
		break;
	default:
		result = "";
		break;
	}

	return result;
}
