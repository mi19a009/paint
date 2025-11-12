/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define AREA_COLUMN       0
#define AREA_ROW          0
#define AREA_WIDTH        1
#define AREA_HEIGHT       1
#define HSCROLLBAR_COLUMN 0
#define HSCROLLBAR_ROW    1
#define HSCROLLBAR_WIDTH  1
#define HSCROLLBAR_HEIGHT 1
#define VSCROLLBAR_COLUMN 1
#define VSCROLLBAR_ROW    0
#define VSCROLLBAR_WIDTH  1
#define VSCROLLBAR_HEIGHT 1

/* クラスのインスタンス */
struct _PaintCanvas
{
	GtkGrid parent_instance;
};

static void paint_canvas_class_init      (PaintCanvasClass *this_class);
static void paint_canvas_draw            (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data);
static void paint_canvas_init            (PaintCanvas *self);
static void paint_canvas_init_area       (GtkGrid *self);
static void paint_canvas_init_hscrollbar (GtkGrid *self);
static void paint_canvas_init_vscrollbar (GtkGrid *self);

/*******************************************************************************
* Paint Canvas クラス。
* 描画領域とスクロール バーを格納します。
*/
G_DEFINE_FINAL_TYPE (PaintCanvas, paint_canvas, GTK_TYPE_GRID);

/*******************************************************************************
* @brief クラスを初期化します。
*/
static void
paint_canvas_class_init (PaintCanvasClass *this_class)
{
}

/*******************************************************************************
* @brief 描画します。
*/
static void
paint_canvas_draw (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data)
{
}

/*******************************************************************************
* @brief 描画領域を取得します。
*/
GtkWidget *
paint_canvas_get_area (PaintCanvas *self)
{
	return gtk_grid_get_child_at (GTK_GRID (self), AREA_COLUMN, AREA_ROW);
}

/*******************************************************************************
* @brief 水平スクロール バーを取得します。
*/
GtkWidget *
paint_canvas_get_hscrollbar (PaintCanvas *self)
{
	return gtk_grid_get_child_at (GTK_GRID (self), HSCROLLBAR_COLUMN, HSCROLLBAR_ROW);
}

/*******************************************************************************
* @brief 垂直スクロール バーを取得します。
*/
GtkWidget *
paint_canvas_get_vscrollbar (PaintCanvas *self)
{
	return gtk_grid_get_child_at (GTK_GRID (self), VSCROLLBAR_COLUMN, VSCROLLBAR_ROW);
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
paint_canvas_init (PaintCanvas *self)
{
	GtkGrid *grid;
	grid = GTK_GRID (self);
	paint_canvas_init_area (grid);
	paint_canvas_init_vscrollbar (grid);
	paint_canvas_init_hscrollbar (grid);
}

/*******************************************************************************
* @brief 描画領域を作成します。
*/
static void
paint_canvas_init_area (GtkGrid *self)
{
	GtkWidget *child;
	child = gtk_drawing_area_new ();
	gtk_widget_set_hexpand (child, TRUE);
	gtk_widget_set_vexpand (child, TRUE);
	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (child), paint_canvas_draw, self, NULL);
	gtk_grid_attach (self, child, AREA_COLUMN, AREA_ROW, AREA_WIDTH, AREA_HEIGHT);
}

/*******************************************************************************
* @brief 水平スクロール バーを作成します。
*/
static void
paint_canvas_init_hscrollbar (GtkGrid *self)
{
	GtkWidget *child;
	child = gtk_scrollbar_new (GTK_ORIENTATION_HORIZONTAL, NULL);
	gtk_widget_set_hexpand (child, TRUE);
	gtk_grid_attach (self, child, HSCROLLBAR_COLUMN, HSCROLLBAR_ROW, HSCROLLBAR_WIDTH, HSCROLLBAR_HEIGHT);
}

/*******************************************************************************
* @brief 垂直スクロール バーを作成します。
*/
static void
paint_canvas_init_vscrollbar (GtkGrid *self)
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
paint_canvas_new (void)
{
	return g_object_new (PAINT_TYPE_CANVAS, NULL);
}
