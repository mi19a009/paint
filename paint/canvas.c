/* Copyright (C) 2025 Taichi Murakami. */
#include <math.h>
#include <gtk/gtk.h>
#include "paint.h"
#include "share.h"

/* クラスのプロパティ */
enum _PaintCanvasProperties
{
	NULL_PROPERTY_ID,
	CONTENT_HEIGHT_PROPERTY_ID,
	CONTENT_WIDTH_PROPERTY_ID,
	LINE_WIDTH_PROPERTY_ID,
	ZOOM_PROPERTY_ID,
};

/* クラスのインスタンス */
struct _PaintCanvas
{
	GtkGrid          parent_instance;
	GtkWidget       *area;
	GtkAdjustment   *hadjustment;
	GtkAdjustment   *vadjustment;
	PaintCommand    *command;
	GQueue          *command_queue;
	cairo_surface_t *surface;
	GdkPixbuf       *surface_source;
	PaintColor       color;
	PaintPoint       offset;
	PaintPoint       point;
	int              content_width;
	int              content_height;
	int              line_width;
	int              surface_width;
	int              surface_height;
	int              resize_width;
	int              resize_height;
	int              zoom;
	unsigned char    antialias;
	unsigned char    command_type;
};

static void paint_canvas_change_hadjustment  (GtkAdjustment *adjustment, gpointer user_data);
static void paint_canvas_change_vadjustment  (GtkAdjustment *adjustment, gpointer user_data);
static void paint_canvas_class_init          (PaintCanvasClass *this_class);
static void paint_canvas_class_init_object   (GObjectClass *this_class);
static void paint_canvas_click_pressed       (GtkGestureClick *click, int n_press, double x, double y, gpointer user_data);
static void paint_canvas_click_released      (GtkGestureClick *click, int n_press, double x, double y, gpointer user_data);
static void paint_canvas_destroy             (PaintCanvas *self);
static void paint_canvas_destroy_commands    (PaintCanvas *self);
static void paint_canvas_dispose             (GObject *self);
static void paint_canvas_draw                (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data);
static void paint_canvas_draw_commands       (PaintCanvas *self, cairo_t *cairo);
static void paint_canvas_draw_surface        (PaintCanvas *self, cairo_t *cairo);
static void paint_canvas_init                (PaintCanvas *self);
static void paint_canvas_init_area           (PaintCanvas *self);
static void paint_canvas_init_command        (PaintCanvas *self, GType type);
static void paint_canvas_init_command_draw   (PaintCanvas *self);
static void paint_canvas_init_hscrollbar     (PaintCanvas *self);
static void paint_canvas_init_vscrollbar     (PaintCanvas *self);
static void paint_canvas_load_surface        (PaintCanvas *self);
static void paint_canvas_motion_enter        (GtkEventControllerMotion *motion, double x, double y, gpointer user_data);
static void paint_canvas_motion_leave        (GtkEventControllerMotion *motion, gpointer user_data);
static void paint_canvas_motion_move         (GtkEventControllerMotion *motion, double x, double y, gpointer user_data);
static void paint_canvas_resize_area         (GtkDrawingArea *area, int width, int height, gpointer user_data);
static void paint_canvas_resize_surface      (PaintCanvas *self, cairo_t *cairo);
static void paint_canvas_transform           (PaintCanvas *self, cairo_t *cairo);
static void paint_canvas_update_offset_x     (PaintCanvas *self);
static void paint_canvas_update_offset_y     (PaintCanvas *self);
static void paint_canvas_update_point        (PaintCanvas *self, double x, double y);
static void paint_canvas_update_range_height (PaintCanvas *self);
static void paint_canvas_update_range_width  (PaintCanvas *self);

/*******************************************************************************
* Paint Canvas クラス:
* 描画領域とスクロール バーを格納します。
* 領域に画像を描画します。
* スクロール バーの値に応じて画像を平行移動します。
*/
G_DEFINE_FINAL_TYPE (PaintCanvas, paint_canvas, GTK_TYPE_GRID);

/* 描画領域 */
#define AREA_COLUMN 0
#define AREA_ROW    0
#define AREA_WIDTH  1
#define AREA_HEIGHT 1

/* 水平スクロール バー */
#define HSCROLLBAR_COLUMN 0
#define HSCROLLBAR_ROW    1
#define HSCROLLBAR_WIDTH  1
#define HSCROLLBAR_HEIGHT 1

/* 垂直スクロール バー */
#define VSCROLLBAR_COLUMN 1
#define VSCROLLBAR_ROW    0
#define VSCROLLBAR_WIDTH  1
#define VSCROLLBAR_HEIGHT 1

/* シグナル */
#define SIGNAL_ENTER         "enter"
#define SIGNAL_LEAVE         "leave"
#define SIGNAL_MOTION        "motion"
#define SIGNAL_PRESSED       "pressed"
#define SIGNAL_RELEASED      "released"
#define SIGNAL_RESIZE        "resize"
#define SIGNAL_VALUE_CHANGED "value-changed"

/* 色プロパティ */
#define COLOR_PROPERTY_NAME          "color"
#define COLOR_PROPERTY_NICK          "Color"
#define COLOR_PROPERTY_BLURB         "Color"
#define COLOR_PROPERTY_MINIMUM       0
#define COLOR_PROPERTY_MAXIMUM       G_MAXUINT
#define COLOR_PROPERTY_DEFAULT_VALUE RGBA (0, 0, 0, 255)
#define COLOR_PROPERTY_FLAGS         G_PARAM_READWRITE

/* スクロール可能な高さプロパティ */
#define CONTENT_HEIGHT_PROPERTY_NAME          "content-height"
#define CONTENT_HEIGHT_PROPERTY_NICK          "Content Height"
#define CONTENT_HEIGHT_PROPERTY_BLURB         "Content Height"
#define CONTENT_HEIGHT_PROPERTY_MINIMUM       0
#define CONTENT_HEIGHT_PROPERTY_MAXIMUM       G_MAXINT
#define CONTENT_HEIGHT_PROPERTY_DEFAULT_VALUE 0
#define CONTENT_HEIGHT_PROPERTY_FLAGS         G_PARAM_READWRITE

/* スクロール可能な幅プロパティ */
#define CONTENT_WIDTH_PROPERTY_NAME          "content-width"
#define CONTENT_WIDTH_PROPERTY_NICK          "Content Width"
#define CONTENT_WIDTH_PROPERTY_BLURB         "Content Width"
#define CONTENT_WIDTH_PROPERTY_MINIMUM       0
#define CONTENT_WIDTH_PROPERTY_MAXIMUM       G_MAXINT
#define CONTENT_WIDTH_PROPERTY_DEFAULT_VALUE 0
#define CONTENT_WIDTH_PROPERTY_FLAGS         G_PARAM_READWRITE

/* ストローク幅プロパティ */
#define LINE_WIDTH_PROPERTY_NAME          "line-width"
#define LINE_WIDTH_PROPERTY_NICK          "Line Width"
#define LINE_WIDTH_PROPERTY_BLURB         "Line Width"
#define LINE_WIDTH_PROPERTY_MINIMUM       1
#define LINE_WIDTH_PROPERTY_MAXIMUM       G_MAXINT
#define LINE_WIDTH_PROPERTY_DEFAULT_VALUE 10
#define LINE_WIDTH_PROPERTY_FLAGS         G_PARAM_READWRITE

/* 拡大率プロパティ */
#define ZOOM_PROPERTY_NAME          "zoom"
#define ZOOM_PROPERTY_NICK          "Zoom%%"
#define ZOOM_PROPERTY_BLURB         "Zoom%%"
#define ZOOM_PROPERTY_MINIMUM       5
#define ZOOM_PROPERTY_MAXIMUM       2000
#define ZOOM_PROPERTY_DEFAULT_VALUE 100
#define ZOOM_PROPERTY_FLAGS         G_PARAM_READWRITE

static void
paint_canvas_change_hadjustment (GtkAdjustment *adjustment, gpointer user_data)
{
	PaintCanvas *self;
	self = PAINT_CANVAS (user_data);
	paint_canvas_update_offset_x (self);
	gtk_widget_queue_draw (self->area);
}

static void
paint_canvas_change_vadjustment (GtkAdjustment *adjustment, gpointer user_data)
{
	PaintCanvas *self;
	self = PAINT_CANVAS (user_data);
	paint_canvas_update_offset_y (self);
	gtk_widget_queue_draw (self->area);
}

/*******************************************************************************
* @brief クラスを初期化します。
*/
static void
paint_canvas_class_init (PaintCanvasClass *this_class)
{
	paint_canvas_class_init_object (G_OBJECT_CLASS (this_class));
}

/*******************************************************************************
* @brief Object クラスを初期化します。
*/
static void
paint_canvas_class_init_object (GObjectClass *this_class)
{
	this_class->dispose = paint_canvas_dispose;
}

/*******************************************************************************
* @brief マウス ボタンを押しました。
*/
static void
paint_canvas_click_pressed (GtkGestureClick *click, int n_press, double x, double y, gpointer user_data)
{
	PaintCanvas *self;
	self = PAINT_CANVAS (user_data);

	switch (self->command_type)
	{
	case PAINT_COMMAND_TYPE_DRAW:
		paint_canvas_init_command_draw (self);
		break;
	default:
		g_clear_object (&self->command);
		break;
	}
	if (self->command)
	{
		g_queue_push_tail (self->command_queue, g_object_ref (self->command));
	}

	paint_canvas_update_point (self, x, y);
}

/*******************************************************************************
* @brief マウス ボタンを離しました。
*/
static void
paint_canvas_click_released (GtkGestureClick *click, int n_press, double x, double y, gpointer user_data)
{
	PaintCanvas *self;
	self = PAINT_CANVAS (user_data);
	g_clear_object (&self->command);
}

/*******************************************************************************
* @brief プロパティを破棄します。
*/
static void
paint_canvas_destroy (PaintCanvas *self)
{
	paint_canvas_destroy_commands (self);
	g_clear_pointer (&self->surface, cairo_surface_destroy);
	g_clear_object (&self->command);
	g_clear_object (&self->surface_source);
}

static void
paint_canvas_destroy_commands (PaintCanvas *self)
{
	GList *list;

	if (self->command_queue)
	{
		list = self->command_queue->head;

		while (list)
		{
			if (list->data)
			{
				g_object_unref (list->data);
			}

			list = list->next;
		}

		g_queue_free (self->command_queue);
		self->command_queue = NULL;
	}
}

/*******************************************************************************
* @brief クラスのインスタンスを破棄します。
*/
static void
paint_canvas_dispose (GObject *self)
{
	paint_canvas_destroy (PAINT_CANVAS (self));
	G_OBJECT_CLASS (paint_canvas_parent_class)->dispose (self);
}

/*******************************************************************************
* @brief 領域に描画します。
*/
static void
paint_canvas_draw (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data)
{
	PaintCanvas *self;
	self = PAINT_CANVAS (user_data);
	paint_canvas_resize_surface (self, cairo);
	paint_canvas_load_surface (self);
	paint_canvas_transform (self, cairo);
	paint_canvas_draw_surface (self, cairo);
	paint_canvas_draw_commands (self, cairo);
}

static void
paint_canvas_draw_commands (PaintCanvas *self, cairo_t *cairo)
{
	GList *list;
	PaintCommand *command;
	list = self->command_queue->head;

	while (list)
	{
		if (list->data)
		{
			command = PAINT_COMMAND (list->data);
			paint_command_execute (command, cairo);
		}

		list = list->next;
	}
}

/*******************************************************************************
* @brief 画像を描画します。
*/
static void
paint_canvas_draw_surface (PaintCanvas *self, cairo_t *cairo)
{
	if (self->surface)
	{
		cairo_set_source_surface (cairo, self->surface, 0, 0);
		cairo_paint (cairo);
	}
}

/*******************************************************************************
* @brief アンチエイリアスを取得します。
*/
gboolean
paint_canvas_get_antialias (PaintCanvas *self)
{
	return self->antialias;
}

/*******************************************************************************
* @brief 色を取得します。
*/
PaintColor
paint_canvas_get_color (PaintCanvas *self)
{
	return self->color;
}

/*******************************************************************************
* @brief コマンドを取得します。
*/
PaintCommandType
paint_canvas_get_command_type (PaintCanvas *self)
{
	return self->command_type;
}

/*******************************************************************************
* @brief 領域の高さを取得します。
*/
int
paint_canvas_get_content_height (PaintCanvas *self)
{
	return gtk_drawing_area_get_content_height (GTK_DRAWING_AREA (self->area));
}

/*******************************************************************************
* @brief 領域の幅を取得します。
*/
int
paint_canvas_get_content_width (PaintCanvas *self)
{
	return gtk_drawing_area_get_content_width (GTK_DRAWING_AREA (self->area));
}

/*******************************************************************************
* @brief ストローク幅を取得します。
*/
int
paint_canvas_get_line_width (PaintCanvas *self)
{
	return self->line_width;
}

/*******************************************************************************
* @brief カーソル位置を取得します。
*/
void
paint_cnavas_get_point (PaintCanvas *self, int *x, int *y)
{
	*x = self->point.x;
	*y = self->point.y;
}

/*******************************************************************************
* @brief 画像の高さを取得します。
*/
int
paint_canvas_get_surface_height (PaintCanvas *self)
{
	return self->surface_height;
}

/*******************************************************************************
* @brief 画像の幅を取得します。
*/
int
paint_canvas_get_surface_width (PaintCanvas *self)
{
	return self->surface_width;
}

/*******************************************************************************
* @brief 拡大率を取得します。
*/
int
paint_canvas_get_zoom (PaintCanvas *self)
{
	return self->zoom;
}

/*******************************************************************************
* @brief 拡大率を取得します。
*/
double
paint_canvas_get_zoom_percent (PaintCanvas *self)
{
	return self->zoom / (double) ZOOM_PROPERTY_DEFAULT_VALUE;
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
paint_canvas_init (PaintCanvas *self)
{
	self->antialias = TRUE;
	self->color = COLOR_PROPERTY_DEFAULT_VALUE;
	self->command_queue = g_queue_new ();
	self->command_type = PAINT_COMMAND_TYPE_DRAW;
	self->line_width = 10;
	self->zoom = ZOOM_PROPERTY_DEFAULT_VALUE;
	paint_canvas_init_area (self);
	paint_canvas_init_vscrollbar (self);
	paint_canvas_init_hscrollbar (self);
}

/*******************************************************************************
* @brief 描画領域を作成します。
*/
static void
paint_canvas_init_area (PaintCanvas *self)
{
	GtkEventController *controller;
	self->area = gtk_drawing_area_new ();
	g_signal_connect (self->area, SIGNAL_RESIZE, G_CALLBACK (paint_canvas_resize_area), self);
	gtk_widget_set_hexpand (self->area, TRUE);
	gtk_widget_set_vexpand (self->area, TRUE);
	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (self->area), paint_canvas_draw, self, NULL);
	gtk_grid_attach (GTK_GRID (self), self->area, AREA_COLUMN, AREA_ROW, AREA_WIDTH, AREA_HEIGHT);
	controller = gtk_event_controller_motion_new ();
	g_signal_connect (controller, SIGNAL_ENTER, G_CALLBACK (paint_canvas_motion_enter), self);
	g_signal_connect (controller, SIGNAL_LEAVE, G_CALLBACK (paint_canvas_motion_leave), self);
	g_signal_connect (controller, SIGNAL_MOTION, G_CALLBACK (paint_canvas_motion_move), self);
	gtk_widget_add_controller (self->area, controller);
	controller = GTK_EVENT_CONTROLLER (gtk_gesture_click_new ());
	g_signal_connect (controller, SIGNAL_PRESSED, G_CALLBACK (paint_canvas_click_pressed), self);
	g_signal_connect (controller, SIGNAL_RELEASED, G_CALLBACK (paint_canvas_click_released), self);
	gtk_widget_add_controller (self->area, controller);
}

static void
paint_canvas_init_command (PaintCanvas *self, GType type)
{
	if (self->command)
	{
		g_object_unref (self->command);
	}

	self->command = PAINT_COMMAND (g_object_new (type, NULL));
	paint_command_set_antialias (self->command, self->antialias);
}

static void
paint_canvas_init_command_draw (PaintCanvas *self)
{
	PaintCommandDraw *command;
	paint_canvas_init_command (self, PAINT_TYPE_COMMAND_DRAW);
	command = PAINT_COMMAND_DRAW (self->command);
	paint_command_draw_set_color (command, self->color);
	paint_command_draw_set_line_width (command, self->line_width);
}

/*******************************************************************************
* @brief 水平スクロール バーを作成します。
*/
static void
paint_canvas_init_hscrollbar (PaintCanvas *self)
{
	GtkWidget *scrollbar;
	scrollbar = gtk_scrollbar_new (GTK_ORIENTATION_HORIZONTAL, NULL);
	self->hadjustment = gtk_scrollbar_get_adjustment (GTK_SCROLLBAR (scrollbar));
	g_signal_connect (self->hadjustment, SIGNAL_VALUE_CHANGED, G_CALLBACK (paint_canvas_change_hadjustment), self);
	gtk_widget_set_hexpand (scrollbar, TRUE);
	gtk_grid_attach (GTK_GRID (self), scrollbar, HSCROLLBAR_COLUMN, HSCROLLBAR_ROW, HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT);
}

/*******************************************************************************
* @brief 垂直スクロール バーを作成します。
*/
static void
paint_canvas_init_vscrollbar (PaintCanvas *self)
{
	GtkWidget *scrollbar;
	scrollbar = gtk_scrollbar_new (GTK_ORIENTATION_VERTICAL, NULL);
	self->vadjustment = gtk_scrollbar_get_adjustment (GTK_SCROLLBAR (scrollbar));
	g_signal_connect (self->vadjustment, SIGNAL_VALUE_CHANGED, G_CALLBACK (paint_canvas_change_vadjustment), self);
	gtk_widget_set_vexpand (scrollbar, TRUE);
	gtk_grid_attach (GTK_GRID (self), scrollbar, VSCROLLBAR_COLUMN, VSCROLLBAR_ROW, VSCROLLBAR_WIDTH, VSCROLLBAR_HEIGHT);
}

/*******************************************************************************
* @brief 画像を設定します。
*/
void
paint_canvas_load (PaintCanvas *self, GdkPixbuf *source)
{
	if (self->surface_source != source)
	{
		if (self->surface_source)
		{
			g_object_unref (self->surface_source);
		}
		if (source)
		{
			self->surface_source = g_object_ref (source);
		}
		else
		{
			self->surface_source = NULL;
		}

		gtk_widget_queue_draw (self->area);
	}
}

/*******************************************************************************
* @brief 画像を設定します。
*/
static void
paint_canvas_load_surface (PaintCanvas *self)
{
	if (self->surface_source)
	{
		share_surface_load (self->surface, self->surface_source);
		g_object_unref (self->surface_source);
		self->surface_source = NULL;
	}
}

/*******************************************************************************
* @brief カーソルを移動しました。
*/
static void
paint_canvas_motion_enter (GtkEventControllerMotion *motion, double x, double y, gpointer user_data)
{
	PaintCanvas *self;
	self = PAINT_CANVAS (user_data);
	paint_canvas_update_point (self, x, y);
}

/*******************************************************************************
* @brief カーソルを移動しました。
*/
static void
paint_canvas_motion_leave (GtkEventControllerMotion *motion, gpointer user_data)
{
}

/*******************************************************************************
* @brief カーソルを移動しました。
*/
static void
paint_canvas_motion_move (GtkEventControllerMotion *motion, double x, double y, gpointer user_data)
{
	PaintCanvas *self;
	self = PAINT_CANVAS (user_data);
	paint_canvas_update_point (self, x, y);
}

/*******************************************************************************
* @brief クラスのインスタンスを作成します。
*/
GtkWidget *
paint_canvas_new (void)
{
	return g_object_new (PAINT_TYPE_CANVAS, NULL);
}

/*******************************************************************************
* @brief 画像の大きさを設定します。
* 画像の大きさは描画時に更新されます。
*/
void
paint_canvas_resize (PaintCanvas *self, int width, int height)
{
	self->resize_width = width;
	self->resize_height = height;
	gtk_widget_queue_draw (self->area);
}

/*******************************************************************************
* @brief 描画領域の大きさを変更します。
*/
static void
paint_canvas_resize_area (GtkDrawingArea *area, int width, int height, gpointer user_data)
{
	PaintCanvas *self;
	self = PAINT_CANVAS (user_data);
	gtk_adjustment_set_page_size (self->hadjustment, width);
	gtk_adjustment_set_page_size (self->vadjustment, height);
}

/*******************************************************************************
* @brief サーフィスを作成します。
*/
static void
paint_canvas_resize_surface (PaintCanvas *self, cairo_t *cairo)
{
	cairo_surface_t *surface;

	if ((self->surface_width != self->resize_width) || (self->surface_height != self->resize_height))
	{
		surface = cairo_surface_create_similar (cairo_get_target (cairo), CAIRO_CONTENT_COLOR_ALPHA, self->resize_width, self->resize_height);

		if (surface)
		{
			if (self->surface)
			{
				cairo_surface_destroy (self->surface);
			}

			self->surface = surface;
			self->surface_width = self->resize_width;
			self->surface_height = self->resize_height;
		}
		else
		{
			self->resize_width = self->surface_width;
			self->resize_height = self->surface_height;
		}
	}
}

/*******************************************************************************
* @brief アンチエイリアスを設定します。
*/
void
paint_canvas_set_antialias (PaintCanvas *self, gboolean antialias)
{
	self->antialias = antialias != 0;
}

/*******************************************************************************
* @brief 色を設定します。
*/
void
paint_canvas_set_color (PaintCanvas *self, PaintColor color)
{
	self->color = color;
}

/*******************************************************************************
* @brief コマンドを設定します。
*/
void
paint_canvas_set_command_type (PaintCanvas *self, PaintCommandType type)
{
	self->command_type = type;
}

/*******************************************************************************
* @brief 領域の高さを設定します。
*/
void
paint_canvas_set_content_height (PaintCanvas *self, int height)
{
	self->content_height = height;
	paint_canvas_update_range_height (self);
}

/*******************************************************************************
* @brief 領域の幅を設定します。
*/
void
paint_canvas_set_content_width (PaintCanvas *self, int width)
{
	self->content_width = width;
	paint_canvas_update_range_width (self);
}

/*******************************************************************************
* @brief ストローク幅を設定します。
*/
void
paint_canvas_set_line_width (PaintCanvas *self, int width)
{
	self->line_width = width;
}

/*******************************************************************************
* @brief 拡大率を設定します。
*/
void
paint_canvas_set_zoom (PaintCanvas *self, int zoom)
{
	self->zoom = zoom;
	paint_canvas_update_range_width (self);
	paint_canvas_update_range_height (self);
	gtk_widget_queue_draw (self->area);
}

/*******************************************************************************
* @brief 拡大率を設定します。
*/
void
paint_canvas_set_zoom_percent (PaintCanvas *self, double percent)
{
	int zoom;
	zoom = lround (percent * ZOOM_PROPERTY_DEFAULT_VALUE);
	paint_canvas_set_zoom (self, zoom);
}

/*******************************************************************************
* @brief 平行移動を適用します。
*/
static void
paint_canvas_transform (PaintCanvas *self, cairo_t *cairo)
{
	double scale;
	scale = paint_canvas_get_zoom_percent (self);
	cairo_scale (cairo, scale, scale);
	cairo_translate (cairo, self->offset.x, self->offset.y);
}

/*******************************************************************************
* @brief 平行移動量を更新します。
*/
static void
paint_canvas_update_offset_x (PaintCanvas *self)
{
	double value, scale;
	value = gtk_adjustment_get_value (self->hadjustment);
	scale = paint_canvas_get_zoom_percent (self);
	self->offset.x = -lround (value / scale);
}

/*******************************************************************************
* @brief 平行移動量を更新します。
*/
static void
paint_canvas_update_offset_y (PaintCanvas *self)
{
	double value, scale;
	value = gtk_adjustment_get_value (self->vadjustment);
	scale = paint_canvas_get_zoom_percent (self);
	self->offset.y = -lround (value / scale);
}

/*******************************************************************************
* @brief カーソル位置を更新します。
*/
static void
paint_canvas_update_point (PaintCanvas *self, double x, double y)
{
	double scale;
	scale = paint_canvas_get_zoom_percent (self);
	self->point.x = lround (x / scale - self->offset.x);
	self->point.y = lround (y / scale - self->offset.y);

	if (self->command)
	{
		paint_command_update (self->command, self->point.x, self->point.y);
		gtk_widget_queue_draw (self->area);
	}
}

/*******************************************************************************
* @brief スクロール範囲を更新します。
*/
static void
paint_canvas_update_range_height (PaintCanvas *self)
{
	double value;
	value = self->content_height * paint_canvas_get_zoom_percent (self);
	gtk_adjustment_set_upper (self->vadjustment, value);
}

/*******************************************************************************
* @brief スクロール範囲を更新します。
*/
static void
paint_canvas_update_range_width (PaintCanvas *self)
{
	double value;
	value = self->content_width * paint_canvas_get_zoom_percent (self);
	gtk_adjustment_set_upper (self->hadjustment, value);
}
