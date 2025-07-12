/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "text.h"
#define PAINT_BITS_PER_SAMPLE 8
#define PAINT_RESOURCE_PATH "/com/github/mi19a009/paint/resource/%s"
#define PAINT_SURFACE_N_CHANNELS 4

typedef GtkFileFilter *(*PaintFileFilterNew) (void);
typedef void (*PaintSurfaceInitChannel) (guchar *, const guchar *, int, int, int);

static void
paint_about_dialog_init (GtkAboutDialog *);
static void
paint_about_dialog_present (GtkWindow *, GtkWindow *);
static void
paint_file_dialog_init (GtkFileDialog *, GFile *);
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
// G_DEFINE_BOXED_TYPE (PaintSurface, paint_surface,
// 	(GBoxedCopyFunc) cairo_surface_reference,
// 	(GBoxedFreeFunc) cairo_surface_destroy);

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
 * @brief バージョン情報ダイアログ ボックスを初期化する。
 ******************************************************************************/
void
paint_about_dialog_init (GtkAboutDialog *dialog)
{
	GdkTexture *logo;
	char path [PAINT_RESOURCE_PATH_CCH];
	gtk_about_dialog_set_authors        (dialog, TEXT_AUTHORS);
	gtk_about_dialog_set_copyright      (dialog, TEXT_COPYRIGHT);
	gtk_about_dialog_set_license_type   (dialog, GTK_LICENSE_APACHE_2_0);
	gtk_about_dialog_set_program_name   (dialog, TEXT_TITLE);
	gtk_about_dialog_set_version        (dialog, TEXT_VERSION);
	gtk_about_dialog_set_website        (dialog, TEXT_WEBSITE);
	paint_resource_format_path (path, PAINT_RESOURCE_PATH_CCH, "pencil.png");
	logo = gdk_texture_new_from_resource (path);

	if (logo)
	{
		gtk_about_dialog_set_logo (dialog, GDK_PAINTABLE (logo));
		g_object_unref (logo);
	}
}

/*******************************************************************************
 * @brief モーダル ウィンドウを表示する。
 ******************************************************************************/
void
paint_about_dialog_present (GtkWindow *dialog, GtkWindow *parent)
{
	gtk_window_set_destroy_with_parent (dialog, TRUE);
	gtk_window_set_modal               (dialog, TRUE);
	gtk_window_set_transient_for       (dialog, parent);
	gtk_window_present                 (dialog);
}

/*******************************************************************************
 * @brief 現在のバージョンを説明するウィンドウを表示する。
 ******************************************************************************/
void
paint_about_dialog_show (GtkWindow *parent)
{
	GtkAboutDialog *dialog;
	dialog = gtk_about_dialog_new ();

	if (dialog)
	{
		g_signal_connect_swapped   (dialog, "destroy", G_CALLBACK (gtk_window_destroy), dialog);
		paint_about_dialog_init    (dialog);
		paint_about_dialog_present (dialog, parent);
	}
}

/*******************************************************************************
 * @brief 指定したエラーを説明するウィンドウを表示する。
 ******************************************************************************/
void
paint_error_dialog_show (GtkWindow *parent, GError *error)
{
	GtkAlertDialog *dialog;

	if (error)
	{
		dialog = gtk_alert_dialog_new ("%s", error->message);

		if (dialog)
		{
			gtk_alert_dialog_set_modal (dialog, TRUE);
			gtk_alert_dialog_show (dialog, parent);
			g_object_unref (dialog);
		}
	}
}

/*******************************************************************************
 * @brief 各ファイル ダイアログ ボックスに共通する初期化を実行する。
 * @param dialog 現在のダイアログ ボックス。
 * @param file 最初のファイルまたは NULL。
 ******************************************************************************/
void
paint_file_dialog_init (GtkFileDialog *dialog, GFile *file)
{
	GListModel *filters;
	filters = paint_file_filter_list_new ();

	if (filters)
	{
		gtk_file_dialog_set_filters (dialog, filters);
		g_object_unref (filters);
	}
	if (file)
	{
		gtk_file_dialog_set_initial_file (dialog, file);
	}
}

/*******************************************************************************
 * @brief ファイルを開くダイアログ ボックスを表示する。
 * @param parent 親ウィンドウ。
 * @param file 最初のファイルまたは NULL。
 * @param callback 完了時のコールバック関数。
 * @param user_data コールバック関数へのデータ。
 ******************************************************************************/
void
paint_file_dialog_open (GtkWindow *parent, GFile *file, GAsyncReadyCallback callback, gpointer user_data)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		paint_file_dialog_init (dialog, file);
		gtk_file_dialog_set_title (dialog, "Open File");
		gtk_file_dialog_open (dialog, parent, NULL, callback, user_data);
		g_object_unref (dialog);
	}
}

/*******************************************************************************
 * @brief ファイルを保存するダイアログ ボックスを表示する。
 * @param parent 親ウィンドウ。
 * @param file 最初のファイルまたは NULL。
 * @param callback 完了時のコールバック関数。
 * @param user_data コールバック関数へのデータ。
 ******************************************************************************/
void
paint_file_dialog_save (GtkWindow *parent, GFile *file, GAsyncReadyCallback callback, gpointer user_data)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		paint_file_dialog_init (dialog, file);
		gtk_file_dialog_set_title (dialog, "Save File");
		gtk_file_dialog_save (dialog, parent, NULL, callback, user_data);
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
paint_surface_init_channel_1 (guchar *destination, const guchar *source, int width, int height, int stride)
{
	const guchar *p;
	int x, y, z;

	for (y = 0; y < height; y++)
	{
		p = source + y * stride;

		for (x = 0; x < width; x++)
		{
			z = *(p++);
			/* B */ *(destination++) = z;
			/* G */ *(destination++) = z;
			/* R */ *(destination++) = z;
			/* A */ *(destination++) = G_MAXUINT8;
		}
	}
}

void
paint_surface_init_channel_3 (guchar *destination, const guchar *source, int width, int height, int stride)
{
	const guchar *p;
	int x, y;

	for (y = 0; y < height; y++)
	{
		p = source + y * stride;

		for (x = 0; x < width; x++)
		{
			/* B */ *(destination++) = p [2];
			/* G */ *(destination++) = p [1];
			/* R */ *(destination++) = p [0];
			/* A */ *(destination++) = G_MAXUINT8;
			p += 3;
		}
	}
}

void
paint_surface_init_channel_4 (guchar *destination, const guchar *source, int width, int height, int stride)
{
	const guchar *p;
	int x, y;

	for (y = 0; y < height; y++)
	{
		p = source + y * stride;

		for (x = 0; x < width; x++)
		{
			/* B */ *(destination++) = p [2];
			/* G */ *(destination++) = p [1];
			/* R */ *(destination++) = p [0];
			/* A */ *(destination++) = p [3];
			p += 4;
		}
	}
}

/*******************************************************************************
 * @brief 指定した画像ファイルを読み込む。
 * @param file 画像ファイル。
 * @return 作成したサーフィスを返す。
 ******************************************************************************/
cairo_surface_t *
paint_surface_new_from_file (GFile *file, cairo_t *context, GError **error)
{
	cairo_surface_t *surface, *image;
	GFileInputStream *input;
	GdkPixbuf *bitmap;
	PaintSurfaceInitChannel init;
	guchar *data;
	int width, height;
	surface = NULL;
	input = g_file_read (file, NULL, NULL);

	if (input)
	{
		bitmap = gdk_pixbuf_new_from_stream (G_INPUT_STREAM (input), NULL, error);
		g_object_unref (input);

		if (bitmap)
		{
			width = gdk_pixbuf_get_width (bitmap);
			height = gdk_pixbuf_get_height (bitmap);
			data = g_malloc_n (PAINT_SURFACE_N_CHANNELS, (size_t) width * height);

			if (data)
			{
				image = cairo_image_surface_create_for_data (data, CAIRO_FORMAT_ARGB32, width, height, gdk_pixbuf_get_rowstride (bitmap));

				if (image)
				{
					surface = cairo_surface_create_similar (cairo_get_target (context), CAIRO_CONTENT_COLOR_ALPHA, width, height);

					if (surface)
					{
						context = cairo_create (surface);

						if (context)
						{
							switch (gdk_pixbuf_get_n_channels (bitmap))
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

							init (data, gdk_pixbuf_get_pixels (bitmap), width, height, gdk_pixbuf_get_rowstride (bitmap));
							cairo_set_source_surface (context, image, 0, 0);
							cairo_paint (context);
							cairo_destroy (context);
						}
					}

					cairo_surface_destroy (image);
				}

				g_free (data);
			}

			g_object_unref (bitmap);
		}
	}

	return surface;
}
