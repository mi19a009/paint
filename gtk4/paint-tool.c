/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"

typedef struct _PaintToolPrivate  PaintToolPrivate;
typedef enum   _PaintToolProperty PaintToolProperty;

/* Paint Tool クラスのプロパティ */
enum
{
	PAINT_TOOL_PROPERTY_NULL,
	PAINT_TOOL_PROPERTY_COLOR,
	PAINT_TOOL_PROPERTY_WIDTH,
};

/* Paint Tool クラスのインスタンス */
struct _PaintToolPrivate
{
	GdkRGBA color;
	float width;
};

static void paint_tool_class_init        (PaintToolClass *this_class);
static void paint_tool_class_init_object (GObjectClass *this_class);
static void paint_tool_get_property      (GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static void paint_tool_init              (PaintTool *self);
static void paint_tool_set_property      (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);

/*******************************************************************************
Paint Tool クラス:
各ツールの基底クラスを表します。
*/ G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (PaintTool, paint_tool, G_TYPE_OBJECT);
#define paint_tool_get_instance_private(self) ((PaintToolPrivate *) (paint_tool_get_instance_private) (self))

/* Color プロパティ */
#define PAINT_TOOL_PROPERTY_COLOR_NAME       "color"
#define PAINT_TOOL_PROPERTY_COLOR_NICK       "Color"
#define PAINT_TOOL_PROPERTY_COLOR_BLURB      "Color"
#define PAINT_TOOL_PROPERTY_COLOR_BOXED_TYPE GDK_TYPE_RGBA
#define PAINT_TOOL_PROPERTY_COLOR_FLAGS      G_PARAM_READWRITE

/* Width プロパティ */
#define PAINT_TOOL_PROPERTY_WIDTH_NAME          "width"
#define PAINT_TOOL_PROPERTY_WIDTH_NICK          "Width"
#define PAINT_TOOL_PROPERTY_WIDTH_BLURB         "Width"
#define PAINT_TOOL_PROPERTY_WIDTH_MINIMUM       0
#define PAINT_TOOL_PROPERTY_WIDTH_MAXIMUM       G_MAXFLOAT
#define PAINT_TOOL_PROPERTY_WIDTH_DEFAULT_VALUE 0
#define PAINT_TOOL_PROPERTY_WIDTH_FLAGS         G_PARAM_READWRITE

/*******************************************************************************
クラスを初期化します。
*/ static void
paint_tool_class_init (PaintToolClass *this_class)
{
	paint_tool_class_init_object (G_OBJECT_CLASS (this_class));
}

/*******************************************************************************
Object クラスを初期化します。
*/ static void
paint_tool_class_init_object (GObjectClass *this_class)
{
	this_class->get_property = paint_tool_get_property;
	this_class->set_property = paint_tool_set_property;
	PAINT_OBJECT_CLASS_INSTALL_PROPERTY (this_class, PAINT_TOOL_PROPERTY_COLOR, PAINT_PARAM_SPEC_BOXED);
	PAINT_OBJECT_CLASS_INSTALL_PROPERTY (this_class, PAINT_TOOL_PROPERTY_WIDTH, PAINT_PARAM_SPEC_FLOAT);
}

/*******************************************************************************
ツールの色を取得します。
*/ GdkRGBA *
paint_tool_get_color (PaintTool *self)
{
	return gdk_rgba_copy (&paint_tool_get_instance_private (self)->color);
}

/*******************************************************************************
アイコン名を取得します。
*/ const char *
paint_tool_get_icon_name (PaintTool *self)
{
	return PAINT_TOOL_GET_CLASS (self)->icon_name;
}

/*******************************************************************************
メニューのアクション名を取得します。
*/ const char *
paint_tool_get_name (PaintTool *self)
{
	return PAINT_TOOL_GET_CLASS (self)->name;
}

/*******************************************************************************
表示名を取得します。
*/ const char *
paint_tool_get_nick (PaintTool *self)
{
	return PAINT_TOOL_GET_CLASS (self)->nick;
}

/*******************************************************************************
プロパティを取得します。
*/ static void
paint_tool_get_property (GObject *self, guint property_id, GValue *value, GParamSpec *pspec)
{
	PaintToolPrivate *properties;
	properties = paint_tool_get_instance_private (PAINT_TOOL (self));

	switch (property_id)
	{
	case PAINT_TOOL_PROPERTY_COLOR:
		g_value_set_boxed (value, &properties->color);
		break;
	case PAINT_TOOL_PROPERTY_WIDTH:
		g_value_set_double (value, properties->width);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

/*******************************************************************************
ツールの幅を取得します。
*/ float
paint_tool_get_width (PaintTool *self)
{
	return paint_tool_get_instance_private (self)->width;
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/ static void
paint_tool_init (PaintTool *self)
{
}

/*******************************************************************************
プロパティを設定します。
*/ void
paint_tool_load (PaintTool *self, PaintTool *other)
{
	const PaintToolPrivate *properties;
	properties = paint_tool_get_instance_private (other);
	paint_tool_set_color (self, &properties->color);
	paint_tool_set_width (self, properties->width);
}

/*******************************************************************************
ツールの色を設定します。
*/ void
paint_tool_set_color (PaintTool *self, const GdkRGBA *color)
{
	PaintToolPrivate *properties;
	properties = paint_tool_get_instance_private (self);

	if (memcmp (&properties->color, color, sizeof (GdkRGBA)))
	{
		memcpy (&properties->color, color, sizeof (GdkRGBA));
		g_object_notify (G_OBJECT (self), PAINT_TOOL_PROPERTY_COLOR_NAME);
	}
}

/*******************************************************************************
プロパティを設定します。
*/ static void
paint_tool_set_property (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec)
{
	PaintTool *properties;
	properties = PAINT_TOOL (self);

	switch (property_id)
	{
	case PAINT_TOOL_PROPERTY_COLOR:
		paint_tool_set_color (properties, g_value_get_boxed (value));
		break;
	case PAINT_TOOL_PROPERTY_WIDTH:
		paint_tool_set_width (properties, g_value_get_double (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

/*******************************************************************************
ツールの幅を設定します。
*/ void
paint_tool_set_width (PaintTool *self, float width)
{
	PaintToolPrivate *properties;
	properties = paint_tool_get_instance_private (self);

	if (properties->width != width)
	{
		properties->width = width;
		g_object_notify (G_OBJECT (self), PAINT_TOOL_PROPERTY_WIDTH_NAME);
	}
}
