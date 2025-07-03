/* Copyright (C) 2025 Taichi Murakami. */
#include <cairo/cairo.h>
#include <gio/gio.h>
#include "paint.h"
#define PAINT_LAYER_PROPERTY_HEIGHT_NAME              "height"
#define PAINT_LAYER_PROPERTY_HEIGHT_NICK              "Layer Height"
#define PAINT_LAYER_PROPERTY_HEIGHT_BLURB             "Layer Height"
#define PAINT_LAYER_PROPERTY_HEIGHT_MINIMUM           1
#define PAINT_LAYER_PROPERTY_HEIGHT_MAXIMUM           (G_MAXUINT16 + 1)
#define PAINT_LAYER_PROPERTY_HEIGHT_DEFAULT_VALUE     480
#define PAINT_LAYER_PROPERTY_HEIGHT_FLAGS             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY
#define PAINT_LAYER_PROPERTY_N_PLANES_NAME            "n-planes"
#define PAINT_LAYER_PROPERTY_N_PLANES_NICK            "Number of Planes"
#define PAINT_LAYER_PROPERTY_N_PLANES_BLURB           "Number of Planes"
#define PAINT_LAYER_PROPERTY_N_PLANES_MINIMUM         2
#define PAINT_LAYER_PROPERTY_N_PLANES_MAXIMUM         G_MAXUINT8
#define PAINT_LAYER_PROPERTY_N_PLANES_DEFAULT_VALUE   16
#define PAINT_LAYER_PROPERTY_N_PLANES_FLAGS           G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY
#define PAINT_LAYER_PROPERTY_NAME_NAME                "name"
#define PAINT_LAYER_PROPERTY_NAME_NICK                "Layer Name"
#define PAINT_LAYER_PROPERTY_NAME_BLURB               "Layer Name"
#define PAINT_LAYER_PROPERTY_NAME_LENGTH              G_MAXINT16
#define PAINT_LAYER_PROPERTY_NAME_DEFAULT_VALUE       "Layer"
#define PAINT_LAYER_PROPERTY_NAME_FLAGS               G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_LAYER_PROPERTY_VISIBILITY_NAME          "visibility"
#define PAINT_LAYER_PROPERTY_VISIBILITY_NICK          "Layer Visibility"
#define PAINT_LAYER_PROPERTY_VISIBILITY_BLURB         "Layer Visibility"
#define PAINT_LAYER_PROPERTY_VISIBILITY_DEFAULT_VALUE PAINT_VISIBILITY_EDIT
#define PAINT_LAYER_PROPERTY_VISIBILITY_FLAGS         G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_LAYER_PROPERTY_WIDTH_NAME               "width"
#define PAINT_LAYER_PROPERTY_WIDTH_NICK               "Layer Width"
#define PAINT_LAYER_PROPERTY_WIDTH_BLURB              "Layer Width"
#define PAINT_LAYER_PROPERTY_WIDTH_MINIMUM            1
#define PAINT_LAYER_PROPERTY_WIDTH_MAXIMUM            (G_MAXUINT16 + 1)
#define PAINT_LAYER_PROPERTY_WIDTH_DEFAULT_VALUE      640
#define PAINT_LAYER_PROPERTY_WIDTH_FLAGS              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY

enum _PaintLayerProperty
{
	PAINT_LAYER_PROPERTY_0,
	PAINT_LAYER_PROPERTY_HEIGHT,
	PAINT_LAYER_PROPERTY_N_PLANES,
	PAINT_LAYER_PROPERTY_NAME,
	PAINT_LAYER_PROPERTY_VISIBILITY,
	PAINT_LAYER_PROPERTY_WIDTH,
};

/* クラスのインスタンス */
struct _PaintLayer
{
	GObject parent_instance;
	gchar  *name;
	GList  *planes;
	GQueue  planes_for_free;
	GQueue  planes_for_undo;
	GQueue  planes_for_work;
	gint32  width;
	gint32  height;
	guint32 n_planes;
	guint16 name_length;
	guint8  visibility;
};

static void
paint_layer_constructed (GObject *);
static void
paint_layer_destroy (PaintLayer *);
static void
paint_layer_destroy_name (PaintLayer *);
static void
paint_layer_destroy_planes (PaintLayer *);
static void
paint_layer_dispose (GObject *);
static void
paint_layer_class_init (PaintLayerClass *);
static void
paint_layer_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
paint_layer_init (PaintLayer *);
static void
paint_layer_init_planes (PaintLayer *);
static void
paint_layer_set_height (PaintLayer *, int);
static void
paint_layer_set_n_planes (PaintLayer *, unsigned);
static void
paint_layer_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
paint_layer_set_width (PaintLayer *, int);

G_DEFINE_FINAL_TYPE (PaintLayer, paint_layer, G_TYPE_OBJECT);

void
paint_layer_class_init (PaintLayerClass *layer)
{
	GObjectClass *object;
	object               = G_OBJECT_CLASS (layer);
	object->constructed  = paint_layer_constructed;
	object->dispose      = paint_layer_dispose;
	object->get_property = paint_layer_get_property;
	object->set_property = paint_layer_set_property;
	g_object_class_install_property (object, PAINT_LAYER_PROPERTY_HEIGHT,     PAINT_PARAM_SPEC_INT    (PAINT_LAYER_PROPERTY_HEIGHT));
	g_object_class_install_property (object, PAINT_LAYER_PROPERTY_N_PLANES,   PAINT_PARAM_SPEC_UINT   (PAINT_LAYER_PROPERTY_N_PLANES));
	g_object_class_install_property (object, PAINT_LAYER_PROPERTY_NAME,       PAINT_PARAM_SPEC_STRING (PAINT_LAYER_PROPERTY_NAME));
	g_object_class_install_property (object, PAINT_LAYER_PROPERTY_VISIBILITY, PAINT_PARAM_SPEC_ENUM   (PAINT_LAYER_PROPERTY_VISIBILITY, PAINT_TYPE_VISIBILITY));
	g_object_class_install_property (object, PAINT_LAYER_PROPERTY_WIDTH,      PAINT_PARAM_SPEC_INT    (PAINT_LAYER_PROPERTY_WIDTH));
}

void
paint_layer_constructed (GObject *object)
{
	paint_layer_init_planes (PAINT_LAYER (object));
	G_OBJECT_CLASS (paint_layer_parent_class)->constructed (object);
}

void
paint_layer_destroy (PaintLayer *layer)
{
	paint_layer_destroy_name (layer);
	paint_layer_destroy_planes (layer);
}

void
paint_layer_destroy_name (PaintLayer *layer)
{
	g_clear_pointer (&layer->name, g_free);
	layer->name_length = 0;
}

void
paint_layer_destroy_planes (PaintLayer *layer)
{
	GList *planes;
	unsigned index;
	planes = layer->planes;

	if (planes)
	{
		for (index = 0; index < layer->n_planes; index++)
		{
			cairo_surface_destroy ((planes++)->data);
		}

		g_free (layer->planes);
	}

	memset (&layer->planes_for_free, 0, sizeof (GQueue));
	memset (&layer->planes_for_undo, 0, sizeof (GQueue));
	memset (&layer->planes_for_work, 0, sizeof (GQueue));
	layer->planes = NULL;
	layer->n_planes = 0;
}

void
paint_layer_dispose (GObject *object)
{
	paint_layer_destroy (PAINT_LAYER (object));
	G_OBJECT_CLASS (paint_layer_parent_class)->dispose (object);
}

int
paint_layer_get_height (PaintLayer *layer)
{
	return layer->height;
}

unsigned
paint_layer_get_n_planes (PaintLayer *layer)
{
	return layer->n_planes;
}

const char *
paint_layer_get_name (PaintLayer *layer)
{
	return layer->name;
}

void
paint_layer_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	switch (property_id)
	{
	case PAINT_LAYER_PROPERTY_HEIGHT:
		g_value_set_int (value, PAINT_LAYER (object)->height);
		break;
	case PAINT_LAYER_PROPERTY_N_PLANES:
		g_value_set_uint (value, PAINT_LAYER (object)->n_planes);
		break;
	case PAINT_LAYER_PROPERTY_NAME:
		g_value_set_string (value, PAINT_LAYER (object)->name);
		break;
	case PAINT_LAYER_PROPERTY_VISIBILITY:
		g_value_set_enum (value, PAINT_LAYER (object)->visibility);
		break;
	case PAINT_LAYER_PROPERTY_WIDTH:
		g_value_set_int (value, PAINT_LAYER (object)->width);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

PaintSurface *
paint_layer_get_surface (PaintLayer *layer)
{
	return layer->planes_for_work.head->data;
}

PaintVisibility
paint_layer_get_visibility (PaintLayer *layer)
{
	return layer->visibility;
}

int
paint_layer_get_width (PaintLayer *layer)
{
	return layer->width;
}

void
paint_layer_init (PaintLayer *)
{
}

void
paint_layer_init_planes (PaintLayer *layer)
{
	GList *planes;
	GQueue *planes_for;
	unsigned index;
	gboolean result = FALSE;

	if (
		CHECK_PROPERTY (layer->width,    PAINT_LAYER_PROPERTY_WIDTH) &&
		CHECK_PROPERTY (layer->height,   PAINT_LAYER_PROPERTY_HEIGHT) &&
		CHECK_PROPERTY (layer->n_planes, PAINT_LAYER_PROPERTY_N_PLANES))
	{
		planes = g_malloc0_n (sizeof (GList), layer->n_planes);

		if (planes)
		{
			layer->planes = planes;
			planes_for = &layer->planes_for_work;
			result = TRUE;

			for (index = 0; result && (index < layer->n_planes); index++)
			{
				planes->data = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, layer->width, layer->height);

				if (planes->data)
				{
					g_queue_push_head_link (planes_for, planes++);
					planes_for = &layer->planes_for_free;
				}
				else
				{
					result = FALSE;
				}
			}
		}
	}
	if (!result)
	{
		g_assert (!"Paint Layer Property invalid.");
		exit (EXIT_FAILURE);
	}
}

PaintLayer *
paint_layer_new (int width, int height, unsigned n_planes)
{
	return g_object_new (PAINT_TYPE_LAYER,
		PAINT_LAYER_PROPERTY_WIDTH_NAME,    width,
		PAINT_LAYER_PROPERTY_HEIGHT_NAME,   height,
		PAINT_LAYER_PROPERTY_N_PLANES_NAME, n_planes,
		NULL);
}

void
paint_layer_set_height (PaintLayer *layer, int height)
{
	layer->height = CLAMP_PROPERTY (height, PAINT_LAYER_PROPERTY_HEIGHT);
}

void
paint_layer_set_n_planes (PaintLayer *layer, unsigned n_planes)
{
	layer->n_planes = CLAMP_PROPERTY (n_planes, PAINT_LAYER_PROPERTY_N_PLANES);
}

void
paint_layer_set_name (PaintLayer *layer, const char *name)
{
	char *buffer;
	unsigned length, size;

	if (name)
	{
		length = strlen (name);
		length = MIN (length, PAINT_LAYER_PROPERTY_NAME_LENGTH);
		size = length + 1;

		if (!layer->name)
		{
			buffer = g_malloc (size);
		}
		else if (layer->name_length < length)
		{
			buffer = g_realloc (layer->name, size);
		}
		if (buffer)
		{
			memcpy (buffer, name, length);
			buffer [length] = 0;
			layer->name = buffer;
			layer->name_length = length;
		}
	}
}

void
paint_layer_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	switch (property_id)
	{
	case PAINT_LAYER_PROPERTY_HEIGHT:
		paint_layer_set_height (PAINT_LAYER (object), g_value_get_int (value));
		break;
	case PAINT_LAYER_PROPERTY_N_PLANES:
		paint_layer_set_n_planes (PAINT_LAYER (object), g_value_get_uint (value));
		break;
	case PAINT_LAYER_PROPERTY_NAME:
		paint_layer_set_name (PAINT_LAYER (object), g_value_get_string (value));
		break;
	case PAINT_LAYER_PROPERTY_VISIBILITY:
		paint_layer_set_visibility (PAINT_LAYER (object), g_value_get_enum (value));
		break;
	case PAINT_LAYER_PROPERTY_WIDTH:
		paint_layer_set_width (PAINT_LAYER (object), g_value_get_int (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

void
paint_layer_set_visibility (PaintLayer *layer, PaintVisibility visibility)
{
	layer->visibility = visibility;
}

void
paint_layer_set_width (PaintLayer *layer, int width)
{
	layer->width = CLAMP_PROPERTY (width, PAINT_LAYER_PROPERTY_WIDTH);
}
