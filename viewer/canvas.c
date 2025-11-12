/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"
#include "share.h"
#define AREA_COLUMN       0
#define AREA_ROW          0
#define AREA_WIDTH        1
#define AREA_HEIGHT       1
#define HSCROLLBAR_COLUMN 0
#define HSCROLLBAR_ROW    1
#define HSCROLLBAR_WIDTH  1
#define HSCROLLBAR_HEIGHT 1
#define SIGNAL_RESIZE     "resize"
#define VSCROLLBAR_COLUMN 1
#define VSCROLLBAR_ROW    0
#define VSCROLLBAR_WIDTH  1
#define VSCROLLBAR_HEIGHT 1

/* クラスのインスタンス */
struct _ViewerCanvas
{
	GtkGrid          parent_instance;
	cairo_pattern_t *pattern;
	cairo_surface_t *surface;
	GdkPixbuf       *pixbuf;
	GdkRGBA          rgba;
};

static void           viewer_canvas_class_init        (ViewerCanvasClass *this_class);
static void           viewer_canvas_class_init_object (GObjectClass *this_class);
static void           viewer_canvas_clear_pattern     (ViewerCanvas *self);
static void           viewer_canvas_clear_pixbuf      (ViewerCanvas *self);
static void           viewer_canvas_clear_surface     (ViewerCanvas *self);
static void           viewer_canvas_destroy           (ViewerCanvas *self);
static void           viewer_canvas_dispose           (GObject *self);
static void           viewer_canvas_draw              (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data);
static GtkAdjustment *viewer_canvas_get_hadjustment   (ViewerCanvas *self);
static void           viewer_canvas_get_property      (GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static GtkAdjustment *viewer_canvas_get_vadjustment   (ViewerCanvas *self);
static void           viewer_canvas_init              (ViewerCanvas *self);
static void           viewer_canvas_init_area         (GtkGrid *self);
static void           viewer_canvas_init_hscrollbar   (GtkGrid *self);
static void           viewer_canvas_init_pattern      (ViewerCanvas *self);
static void           viewer_canvas_init_surface      (ViewerCanvas *self, cairo_t *cairo);
static void           viewer_canvas_init_vscrollbar   (GtkGrid *self);
static void           viewer_canvas_redraw            (ViewerCanvas *self);
static void           viewer_canvas_resize            (GtkDrawingArea *area, int width, int height, gpointer user_data);
static void           viewer_canvas_set_property      (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);
static void           viewer_canvas_update_adjustment (ViewerCanvas *self);

/*******************************************************************************
* Viewer Canvas クラス。
* 描画領域とスクロール バーを格納します。
*/
G_DEFINE_FINAL_TYPE (ViewerCanvas, viewer_canvas, GTK_TYPE_GRID);
static const GdkRGBA DEFAULT_RGBA = { 1.0F / 16.0F, 1.0F / 8.0F, 1.0F / 4.0F, 1.0F };

/*******************************************************************************
* @brief クラスを初期化します。
*/
static void
viewer_canvas_class_init (ViewerCanvasClass *this_class)
{
	viewer_canvas_class_init_object (G_OBJECT_CLASS (this_class));
}

/*******************************************************************************
* @brief Object クラスを初期化します。
*/
static void
viewer_canvas_class_init_object (GObjectClass *this_class)
{
	this_class->dispose = viewer_canvas_dispose;
	this_class->get_property = viewer_canvas_get_property;
	this_class->set_property = viewer_canvas_set_property;
}

/*******************************************************************************
* @brief 背景を破棄します。
*/
static void
viewer_canvas_clear_pattern (ViewerCanvas *self)
{
	g_clear_pointer (&self->pattern, cairo_pattern_destroy);
}

/*******************************************************************************
* @brief 画像を破棄します。
*/
static void
viewer_canvas_clear_pixbuf (ViewerCanvas *self)
{
	g_clear_object (&self->pixbuf);
}

/*******************************************************************************
* @brief 画像を破棄します。
*/
static void
viewer_canvas_clear_surface (ViewerCanvas *self)
{
	g_clear_pointer (&self->surface, cairo_surface_destroy);
}

/*******************************************************************************
* @brief プロパティを破棄します。
*/
static void
viewer_canvas_destroy (ViewerCanvas *self)
{
	viewer_canvas_clear_pattern (self);
	viewer_canvas_clear_surface (self);
	viewer_canvas_clear_pixbuf (self);
}

/*******************************************************************************
* @brief クラスのインスタンスを破棄します。
*/
static void
viewer_canvas_dispose (GObject *self)
{
	viewer_canvas_destroy (VIEWER_CANVAS (self));
	G_OBJECT_CLASS (viewer_canvas_parent_class)->dispose (self);
}

/*******************************************************************************
* @brief 描画します。
*/
static void
viewer_canvas_draw (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data)
{
	ViewerCanvas *self;
	self = VIEWER_CANVAS (user_data);
	viewer_canvas_init_pattern (self);
	viewer_canvas_init_surface (self, cairo);

	if (self->pattern)
	{
		cairo_set_source (cairo, self->pattern);
		cairo_paint (cairo);
	}
	if (self->surface)
	{
		cairo_set_source_surface (cairo, self->surface, 0, 0);
		cairo_paint (cairo);
	}
}

/*******************************************************************************
* @brief 描画領域を取得します。
*/
GtkWidget *
viewer_canvas_get_area (ViewerCanvas *self)
{
	return gtk_grid_get_child_at (GTK_GRID (self), AREA_COLUMN, AREA_ROW);
}

/*******************************************************************************
* @brief 水平スクロールを取得します。
*/
static GtkAdjustment *
viewer_canvas_get_hadjustment (ViewerCanvas *self)
{
	GtkWidget *scrollbar;
	scrollbar = viewer_canvas_get_hscrollbar (self);
	return gtk_scrollbar_get_adjustment (GTK_SCROLLBAR (scrollbar));
}

/*******************************************************************************
* @brief 水平スクロール バーを取得します。
*/
GtkWidget *
viewer_canvas_get_hscrollbar (ViewerCanvas *self)
{
	return gtk_grid_get_child_at (GTK_GRID (self), HSCROLLBAR_COLUMN, HSCROLLBAR_ROW);
}

/*******************************************************************************
* @brief 画像を取得します。
*/
GdkPixbuf *
viewer_canvas_get_pixbuf (ViewerCanvas *self)
{
	return OBJECT_REF (self->pixbuf);
}

/*******************************************************************************
* @brief プロパティを取得します。
*/
static void
viewer_canvas_get_property (GObject *self, guint property_id, GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
}

/*******************************************************************************
* @brief 背景色を取得します。
*/
GdkRGBA *
viewer_canvas_get_rgba (ViewerCanvas *self)
{
	return gdk_rgba_copy (&self->rgba);
}

/*******************************************************************************
* @brief 垂直スクロールを取得します。
*/
static GtkAdjustment *
viewer_canvas_get_vadjustment (ViewerCanvas *self)
{
	GtkWidget *scrollbar;
	scrollbar = viewer_canvas_get_vscrollbar (self);
	return gtk_scrollbar_get_adjustment (GTK_SCROLLBAR (scrollbar));
}

/*******************************************************************************
* @brief 垂直スクロール バーを取得します。
*/
GtkWidget *
viewer_canvas_get_vscrollbar (ViewerCanvas *self)
{
	return gtk_grid_get_child_at (GTK_GRID (self), VSCROLLBAR_COLUMN, VSCROLLBAR_ROW);
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
viewer_canvas_init (ViewerCanvas *self)
{
	GtkGrid *grid;
	grid = GTK_GRID (self);
	memcpy (&self->rgba, &DEFAULT_RGBA, sizeof DEFAULT_RGBA);
	viewer_canvas_init_area (grid);
	viewer_canvas_init_vscrollbar (grid);
	viewer_canvas_init_hscrollbar (grid);
}

/*******************************************************************************
* @brief 描画領域を作成します。
*/
static void
viewer_canvas_init_area (GtkGrid *self)
{
	GtkWidget *child;
	child = gtk_drawing_area_new ();
	g_signal_connect (child, SIGNAL_RESIZE, G_CALLBACK (viewer_canvas_resize), self);
	gtk_widget_set_hexpand (child, TRUE);
	gtk_widget_set_vexpand (child, TRUE);
	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (child), viewer_canvas_draw, self, NULL);
	gtk_grid_attach (self, child, AREA_COLUMN, AREA_ROW, AREA_WIDTH, AREA_HEIGHT);
}

/*******************************************************************************
* @brief 水平スクロール バーを作成します。
*/
static void
viewer_canvas_init_hscrollbar (GtkGrid *self)
{
	GtkWidget *child;
	child = gtk_scrollbar_new (GTK_ORIENTATION_HORIZONTAL, NULL);
	gtk_widget_set_hexpand (child, TRUE);
	gtk_grid_attach (self, child, HSCROLLBAR_COLUMN, HSCROLLBAR_ROW, HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT);
}

/*******************************************************************************
* @brief 背景を作成します。
*/
static void
viewer_canvas_init_pattern (ViewerCanvas *self)
{
	const GdkRGBA *rgba;
	rgba = &self->rgba;

	if (!self->pattern)
	{
		if (gdk_rgba_is_opaque (rgba))
		{
			self->pattern = cairo_pattern_create_rgb (rgba->red, rgba->green, rgba->blue);
		}
		else
		{
			self->pattern = cairo_pattern_create_rgba (rgba->red, rgba->green, rgba->blue, rgba->alpha);
		}
	}
}

/*******************************************************************************
* @brief 画像を作成します。
*/
static void
viewer_canvas_init_surface (ViewerCanvas *self, cairo_t *cairo)
{
	cairo_surface_t *image;
	guchar *data;
	int width, height, stride;

	if (!self->surface && self->pixbuf)
	{
		width = gdk_pixbuf_get_width (self->pixbuf);
		height = gdk_pixbuf_get_height (self->pixbuf);
		stride = gdk_pixbuf_get_rowstride (self->pixbuf);
		data = g_malloc (height * stride);
		pixcpy (gdk_pixbuf_read_pixels (self->pixbuf), data, width, height, stride);
		image = cairo_image_surface_create_for_data (data, CAIRO_FORMAT_ARGB32, width, height, stride);

		if (image)
		{
			self->surface = cairo_surface_create_similar (cairo_get_target (cairo), CAIRO_CONTENT_COLOR_ALPHA, width, height);

			if (self->surface)
			{
				cairo = cairo_create (self->surface);

				if (cairo)
				{
					cairo_set_antialias (cairo, CAIRO_ANTIALIAS_NONE);
					cairo_set_operator (cairo, CAIRO_OPERATOR_SOURCE);
					cairo_set_source_surface (cairo, image, 0, 0);
					cairo_paint (cairo);
					cairo_destroy (cairo);
				}
			}

			cairo_surface_destroy (image);
		}
	}
}

/*******************************************************************************
* @brief 垂直スクロール バーを作成します。
*/
static void
viewer_canvas_init_vscrollbar (GtkGrid *self)
{
	GtkWidget *child;
	child = gtk_scrollbar_new (GTK_ORIENTATION_VERTICAL, NULL);
	gtk_widget_set_vexpand (child, TRUE);
	gtk_grid_attach (self, child, VSCROLLBAR_COLUMN, VSCROLLBAR_ROW, VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT);
}

/*******************************************************************************
* @brief クラスのインスタンスを作成します。
*/
GtkWidget *
viewer_canvas_new (void)
{
	return g_object_new (VIEWER_TYPE_CANVAS, NULL);
}

/*******************************************************************************
* @brief 領域を再描画します。
*/
static void
viewer_canvas_redraw (ViewerCanvas *self)
{
	GtkWidget *area;
	area = viewer_canvas_get_area (self);
	gtk_widget_queue_draw (area);
}

/*******************************************************************************
* @brief スクロール範囲を設定します。
*/
static void
viewer_canvas_resize (GtkDrawingArea *area, int width, int height, gpointer user_data)
{
	ViewerCanvas *self;
	GtkAdjustment *adjustment;
	self = VIEWER_CANVAS (user_data);
	adjustment = viewer_canvas_get_hadjustment (self);
	gtk_adjustment_set_page_size (adjustment, width);
	adjustment = viewer_canvas_get_vadjustment (self);
	gtk_adjustment_set_page_size (adjustment, height);
}

/*******************************************************************************
* @brief 画像を設定します。
*/
void
viewer_canvas_set_pixbuf (ViewerCanvas *self, GdkPixbuf *pixbuf)
{
	if (self->pixbuf != pixbuf)
	{
		if (self->pixbuf)
		{
			g_object_unref (self->pixbuf);
		}
		if (pixbuf)
		{
			self->pixbuf = g_object_ref (pixbuf);
		}
		else
		{
			self->pixbuf = NULL;
		}

		viewer_canvas_clear_surface (self);
		viewer_canvas_update_adjustment (self);
		viewer_canvas_redraw (self);
	}
}

/*******************************************************************************
* @brief プロパティを設定します。
*/
static void
viewer_canvas_set_property (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
}

/*******************************************************************************
* @brief 背景色を設定します。
*/
void
viewer_canvas_set_rgba (ViewerCanvas *self, const GdkRGBA *rgba)
{
	if (memcmp (&self->rgba, rgba, sizeof self->rgba))
	{
		memcpy (&self->rgba, rgba, sizeof self->rgba);
		viewer_canvas_clear_pattern (self);
	}
}

/*******************************************************************************
* @brief スクロール範囲を設定します。
*/
static void
viewer_canvas_update_adjustment (ViewerCanvas *self)
{
	GtkAdjustment *adjustment;
	int width, height;

	if (self->pixbuf)
	{
		width = gdk_pixbuf_get_width (self->pixbuf);
		height = gdk_pixbuf_get_height (self->pixbuf);
	}
	else
	{
		width = 0;
		height = 0;
	}

	adjustment = viewer_canvas_get_hadjustment (self);
	gtk_adjustment_set_upper (adjustment, width);
	adjustment = viewer_canvas_get_vadjustment (self);
	gtk_adjustment_set_upper (adjustment, height);
}
