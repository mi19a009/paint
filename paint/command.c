/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "share.h"

typedef struct _PaintCommandPrivate    PaintCommandPrivate;
typedef enum   _PaintCommandProperties PaintCommandProperties;

/* クラスのプロパティ */
enum _PaintCommandProperties
{
	NULL_PROPERTY_ID,
	ANTIALIAS_PROPERTY_ID,
};

/* クラスのインスタンス */
struct _PaintCommandPrivate
{
	unsigned char antialias;
};

static void paint_command_class_init        (PaintCommandClass *this_class);
static void paint_command_class_init_object (GObjectClass *this_class);
static void paint_command_get_property      (GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static void paint_command_init              (PaintCommand *self);
static void paint_command_set_property      (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);

/*******************************************************************************
* Paint Command クラス:
* 描画方法の基底クラス。
* コマンドを再生する方法を提供します。
*/
G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (PaintCommand, paint_command, G_TYPE_OBJECT);
#define paint_command_get_instance_private(self) ((PaintCommandPrivate *) (paint_command_get_instance_private) (self))

/* アンチエイリアス プロパティ */
#define ANTIALIAS_PROPERTY_NAME          "antialias"
#define ANTIALIAS_PROPERTY_NICK          "Anti-alias Enabled"
#define ANTIALIAS_PROPERTY_BLURB         "Anti-alias Enabled"
#define ANTIALIAS_PROPERTY_DEFAULT_VALUE TRUE
#define ANTIALIAS_PROPERTY_FLAGS         G_PARAM_READWRITE

static void
paint_command_class_init (PaintCommandClass *this_class)
{
	paint_command_class_init_object (G_OBJECT_CLASS (this_class));
}

static void
paint_command_class_init_object (GObjectClass *this_class)
{
	this_class->get_property = paint_command_get_property;
	this_class->set_property = paint_command_set_property;
	OBJECT_CLASS_INSTALL_PROPERTY_BOOLEAN (this_class, ANTIALIAS_PROPERTY);
}

gboolean
paint_command_get_antialias (PaintCommand *self)
{
	PaintCommandPrivate *properties;
	properties = paint_command_get_instance_private (self);
	return properties->antialias;
}

void
paint_command_execute (PaintCommand *self, cairo_t *cairo)
{
	PaintCommandExecuteFunc execute;
	execute = PAINT_COMMAND_GET_CLASS (self)->execute;
	g_assert (execute);
	execute (self, cairo);
}

static void
paint_command_get_property (GObject *self, guint property_id, GValue *value, GParamSpec *pspec)
{
	PaintCommandPrivate *properties;
	properties = paint_command_get_instance_private (PAINT_COMMAND (self));

	switch (property_id)
	{
	case ANTIALIAS_PROPERTY_ID:
		g_value_set_boolean (value, properties->antialias);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

static void
paint_command_init (PaintCommand *self)
{
	PaintCommandPrivate *properties;
	properties = paint_command_get_instance_private (self);
	properties->antialias = ANTIALIAS_PROPERTY_DEFAULT_VALUE;
}

void
paint_command_set_antialias (PaintCommand *self, gboolean antialias)
{
	PaintCommandPrivate *properties;
	properties = paint_command_get_instance_private (self);
	properties->antialias = antialias != 0;
}

static void
paint_command_set_property (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec)
{
	PaintCommand *properties;
	properties = PAINT_COMMAND (self);

	switch (property_id)
	{
	case ANTIALIAS_PROPERTY_ID:
		paint_command_set_antialias (properties, g_value_get_boolean (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

void
paint_command_update (PaintCommand *self, int x, int y)
{
	PaintCommandUpdateFunc update;
	update = PAINT_COMMAND_GET_CLASS (self)->update;
	g_assert (update);
	update (self, x, y);
}
