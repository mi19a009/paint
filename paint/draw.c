/* Copyright (C) 2025 Taichi Murakami. */
#include <math.h>
#include <gtk/gtk.h>
#include "paint.h"
#include "share.h"

/* クラスのプロパティ */
enum _PaintCommandDrawProperties
{
	NULL_PROPERTY_ID,
	COLOR_PROPERTY_ID,
	LINE_WIDTH_PROPERTY_ID,
};

/* クラスのインスタンス */
struct _PaintCommandDraw
{
	PaintCommand parent_instance;
	GArray      *points;
	PaintColor   color;
	int          line_width;
};

static void paint_command_draw_class_init         (PaintCommandDrawClass *this_class);
static void paint_command_draw_class_init_command (PaintCommandClass *this_class);
static void paint_command_draw_class_init_object  (GObjectClass *this_class);
static void paint_command_draw_destroy            (PaintCommandDraw *self);
static void paint_command_draw_dispose            (GObject *self);
static void paint_command_draw_execute            (PaintCommand *self, cairo_t *cairo);
static void paint_command_draw_execute_line       (PaintCommandDraw *self, cairo_t *cairo, guint n_points);
static void paint_command_draw_execute_point      (PaintCommandDraw *self, cairo_t *cairo);
static void paint_command_draw_get_property       (GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static void paint_command_draw_init               (PaintCommandDraw *self);
static void paint_command_draw_set_property       (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);
static void paint_command_draw_update             (PaintCommand *self, int x, int y);
static void paint_command_draw_update_antialias   (PaintCommandDraw *self, cairo_t *cairo);
static void paint_command_draw_update_color       (PaintCommandDraw *self, cairo_t *cairo);

/*******************************************************************************
* Paint Command クラス:
* 領域上をなぞって描画する方法を提供します。
*/
G_DEFINE_FINAL_TYPE (PaintCommandDraw, paint_command_draw, PAINT_TYPE_COMMAND);
#define FACTOR 255.0

/* 色プロパティ */
#define COLOR_PROPERTY_NAME          "color"
#define COLOR_PROPERTY_NICK          "Color"
#define COLOR_PROPERTY_BLURB         "Color"
#define COLOR_PROPERTY_MINIMUM       0
#define COLOR_PROPERTY_MAXIMUM       G_MAXUINT
#define COLOR_PROPERTY_DEFAULT_VALUE RGBA (0, 0, 0, 255)
#define COLOR_PROPERTY_FLAGS         G_PARAM_READWRITE

/* ストローク幅プロパティ */
#define LINE_WIDTH_PROPERTY_NAME          "line-width"
#define LINE_WIDTH_PROPERTY_NICK          "Line Width"
#define LINE_WIDTH_PROPERTY_BLURB         "Line Width"
#define LINE_WIDTH_PROPERTY_MINIMUM       1
#define LINE_WIDTH_PROPERTY_MAXIMUM       G_MAXINT
#define LINE_WIDTH_PROPERTY_DEFAULT_VALUE 1
#define LINE_WIDTH_PROPERTY_FLAGS         G_PARAM_READWRITE

static void
paint_command_draw_class_init (PaintCommandDrawClass *this_class)
{
	paint_command_draw_class_init_object (G_OBJECT_CLASS (this_class));
	paint_command_draw_class_init_command (PAINT_COMMAND_CLASS (this_class));
}

static void
paint_command_draw_class_init_command (PaintCommandClass *this_class)
{
	this_class->execute = paint_command_draw_execute;
	this_class->update = paint_command_draw_update;
}

static void
paint_command_draw_class_init_object (GObjectClass *this_class)
{
	this_class->dispose = paint_command_draw_dispose;
	this_class->get_property = paint_command_draw_get_property;
	this_class->set_property = paint_command_draw_set_property;
	OBJECT_CLASS_INSTALL_PROPERTY_UINT (this_class, COLOR_PROPERTY);
	OBJECT_CLASS_INSTALL_PROPERTY_INT (this_class, LINE_WIDTH_PROPERTY);
}

static void
paint_command_draw_destroy (PaintCommandDraw *self)
{
	if (self->points)
	{
		g_array_free (self->points, FALSE);
		self->points = NULL;
	}
}

static void
paint_command_draw_dispose (GObject *self)
{
	paint_command_draw_destroy (PAINT_COMMAND_DRAW (self));
	G_OBJECT_CLASS (paint_command_draw_parent_class)->dispose (self);
}

static void
paint_command_draw_execute (PaintCommand *self, cairo_t *cairo)
{
	PaintCommandDraw *draw;
	int n_points;
	draw = PAINT_COMMAND_DRAW (self);
	n_points = draw->points->len;

	switch (n_points)
	{
	case 0:
		break;
	case 1:
		paint_command_draw_update_antialias (draw, cairo);
		paint_command_draw_update_color (draw, cairo);
		paint_command_draw_execute_point (draw, cairo);
		break;
	default:
		paint_command_draw_update_antialias (draw, cairo);
		paint_command_draw_update_color (draw, cairo);
		paint_command_draw_execute_line (draw, cairo, n_points);
		break;
	}
}

static void
paint_command_draw_execute_line (PaintCommandDraw *self, cairo_t *cairo, guint n_points)
{
	const PaintPoint *point;
	int n;
	point = (PaintPoint *) self->points->data;
	cairo_move_to (cairo, point->x, point->y);

	for (n = 1; n < n_points; n++)
	{
		point++;
		cairo_line_to (cairo, point->x, point->y);
	}

	cairo_set_line_cap (cairo, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join (cairo, CAIRO_LINE_JOIN_ROUND);
	cairo_set_line_width (cairo, self->line_width);
	cairo_stroke (cairo);
}

static void
paint_command_draw_execute_point (PaintCommandDraw *self, cairo_t *cairo)
{
	const PaintPoint *point;
	double radius, angle;
	point = (PaintPoint *) self->points->data;
	radius = self->line_width / 2.0;
	angle = M_PI * 2.0;
	cairo_arc (cairo, point->x, point->y, radius, 0, angle);
	cairo_fill (cairo);
}

PaintColor
paint_command_draw_get_color (PaintCommandDraw *self)
{
	return self->color;
}

int
paint_command_draw_get_line_width (PaintCommandDraw *self)
{
	return self->line_width;
}

static void
paint_command_draw_get_property (GObject *self, guint property_id, GValue *value, GParamSpec *pspec)
{
	PaintCommandDraw *properties;
	properties = PAINT_COMMAND_DRAW (self);

	switch (property_id)
	{
	case COLOR_PROPERTY_ID:
		g_value_set_uint (value, properties->color);
		break;
	case LINE_WIDTH_PROPERTY_ID:
		g_value_set_int (value, properties->line_width);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

static void
paint_command_draw_init (PaintCommandDraw *self)
{
	self->color = COLOR_PROPERTY_DEFAULT_VALUE;
	self->line_width = LINE_WIDTH_PROPERTY_DEFAULT_VALUE;
}

PaintCommand *
paint_command_draw_new (void)
{
	return g_object_new (PAINT_TYPE_COMMAND_DRAW, NULL);
}

void
paint_command_draw_set_color (PaintCommandDraw *self, PaintColor color)
{
	self->color = color;
}

void
paint_command_draw_set_line_width (PaintCommandDraw *self, int width)
{
	self->line_width = width;
}

static void
paint_command_draw_set_property (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec)
{
	PaintCommandDraw *properties;
	properties = PAINT_COMMAND_DRAW (self);

	switch (property_id)
	{
	case COLOR_PROPERTY_ID:
		paint_command_draw_set_color (properties, g_value_get_uint (value));
		break;
	case LINE_WIDTH_PROPERTY_ID:
		paint_command_draw_set_line_width (properties, g_value_get_int (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

static void
paint_command_draw_update (PaintCommand *self, int x, int y)
{
	PaintCommandDraw *draw;
	PaintPoint point;
	draw = PAINT_COMMAND_DRAW (self);

	if (!draw->points)
	{
		draw->points = g_array_new (FALSE, FALSE, sizeof (PaintPoint));
	}

	point.x = x;
	point.y = y;
	g_array_append_val (draw->points, point);
}

static void
paint_command_draw_update_antialias (PaintCommandDraw *self, cairo_t *cairo)
{
	cairo_antialias_t antialias;
	antialias = paint_command_get_antialias (PAINT_COMMAND (self)) ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE;
	cairo_set_antialias (cairo, antialias);
}

static void
paint_command_draw_update_color (PaintCommandDraw *self, cairo_t *cairo)
{
	double r, g, b, a;
	unsigned u;
	u = self->color;
	r = GetRValue (u) / FACTOR;
	g = GetGValue (u) / FACTOR;
	b = GetBValue (u) / FACTOR;
	u = GetAValue (u);

	if (u == G_MAXUINT8)
	{
		cairo_set_source_rgb (cairo, r, g, b);
	}
	else
	{
		a = u / FACTOR;
		cairo_set_source_rgba (cairo, r, g, b, a);
	}
}
