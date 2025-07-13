/* Copyright (C) 2025 Taichi Murakami. */
#include "paint.h"
#define PAINT_CANVAS_N_SURFACES                           3
#define PAINT_CANVAS_PROPERTY_AUTO_SIZE_NAME              "auto-size"
#define PAINT_CANVAS_PROPERTY_AUTO_SIZE_NICK              "Image Height"
#define PAINT_CANVAS_PROPERTY_AUTO_SIZE_BLURB             "Image Height"
#define PAINT_CANVAS_PROPERTY_AUTO_SIZE_DEFAULT_VALUE     TRUE
#define PAINT_CANVAS_PROPERTY_AUTO_SIZE_FLAGS             G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT_NAME           "image-height"
#define PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT_NICK           "Image Height"
#define PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT_BLURB          "Image Height"
#define PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT_MINIMUM        1
#define PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT_MAXIMUM        G_MAXINT
#define PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT_DEFAULT_VALUE  256
#define PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT_FLAGS          G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_CANVAS_PROPERTY_IMAGE_WIDTH_NAME            "image-width"
#define PAINT_CANVAS_PROPERTY_IMAGE_WIDTH_NICK            "Image Width"
#define PAINT_CANVAS_PROPERTY_IMAGE_WIDTH_BLURB           "Image Width"
#define PAINT_CANVAS_PROPERTY_IMAGE_WIDTH_MINIMUM         1
#define PAINT_CANVAS_PROPERTY_IMAGE_WIDTH_MAXIMUM         G_MAXINT
#define PAINT_CANVAS_PROPERTY_IMAGE_WIDTH_DEFAULT_VALUE   256
#define PAINT_CANVAS_PROPERTY_IMAGE_WIDTH_FLAGS           G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_CANVAS_PROPERTY_ROTATION_NAME               "rotation"
#define PAINT_CANVAS_PROPERTY_ROTATION_NICK               "Rotation"
#define PAINT_CANVAS_PROPERTY_ROTATION_BLURB              "Rotation"
#define PAINT_CANVAS_PROPERTY_ROTATION_MINIMUM            -G_MAXDOUBLE
#define PAINT_CANVAS_PROPERTY_ROTATION_MAXIMUM            +G_MAXDOUBLE
#define PAINT_CANVAS_PROPERTY_ROTATION_DEFAULT_VALUE      0.0
#define PAINT_CANVAS_PROPERTY_ROTATION_FLAGS              G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_CANVAS_PROPERTY_SCALING_X_NAME              "scaling-x"
#define PAINT_CANVAS_PROPERTY_SCALING_X_NICK              "Scaling X"
#define PAINT_CANVAS_PROPERTY_SCALING_X_BLURB             "Scaling X"
#define PAINT_CANVAS_PROPERTY_SCALING_X_MINIMUM           -G_MAXDOUBLE
#define PAINT_CANVAS_PROPERTY_SCALING_X_MAXIMUM           +G_MAXDOUBLE
#define PAINT_CANVAS_PROPERTY_SCALING_X_DEFAULT_VALUE     1.0
#define PAINT_CANVAS_PROPERTY_SCALING_X_FLAGS             G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_CANVAS_PROPERTY_SCALING_Y_NAME              "scaling-y"
#define PAINT_CANVAS_PROPERTY_SCALING_Y_NICK              "Scaling Y"
#define PAINT_CANVAS_PROPERTY_SCALING_Y_BLURB             "Scaling Y"
#define PAINT_CANVAS_PROPERTY_SCALING_Y_MINIMUM           -G_MAXDOUBLE
#define PAINT_CANVAS_PROPERTY_SCALING_Y_MAXIMUM           +G_MAXDOUBLE
#define PAINT_CANVAS_PROPERTY_SCALING_Y_DEFAULT_VALUE     1.0
#define PAINT_CANVAS_PROPERTY_SCALING_Y_FLAGS             G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_CANVAS_PROPERTY_TRANSLATION_X_NAME          "translation-x"
#define PAINT_CANVAS_PROPERTY_TRANSLATION_X_NICK          "Translation X"
#define PAINT_CANVAS_PROPERTY_TRANSLATION_X_BLURB         "Translation X"
#define PAINT_CANVAS_PROPERTY_TRANSLATION_X_MINIMUM       -G_MAXDOUBLE
#define PAINT_CANVAS_PROPERTY_TRANSLATION_X_MAXIMUM       +G_MAXDOUBLE
#define PAINT_CANVAS_PROPERTY_TRANSLATION_X_DEFAULT_VALUE 0.0
#define PAINT_CANVAS_PROPERTY_TRANSLATION_X_FLAGS         G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_CANVAS_PROPERTY_TRANSLATION_Y_NAME          "translation-y"
#define PAINT_CANVAS_PROPERTY_TRANSLATION_Y_NICK          "Translation Y"
#define PAINT_CANVAS_PROPERTY_TRANSLATION_Y_BLURB         "Translation Y"
#define PAINT_CANVAS_PROPERTY_TRANSLATION_Y_MINIMUM       -G_MAXDOUBLE
#define PAINT_CANVAS_PROPERTY_TRANSLATION_Y_MAXIMUM       +G_MAXDOUBLE
#define PAINT_CANVAS_PROPERTY_TRANSLATION_Y_DEFAULT_VALUE 0.0
#define PAINT_CANVAS_PROPERTY_TRANSLATION_Y_FLAGS         G_PARAM_READWRITE | G_PARAM_CONSTRUCT

typedef void (*PaintCanvasDraw) (PaintCanvas *, cairo_t *, gint, gint);

/* Canvas クラスのプロパティ */
enum _PaintCanvasProperty
{
	PAINT_CANVAS_PROPERTY_0,
	PAINT_CANVAS_PROPERTY_AUTO_SIZE,
	PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT,
	PAINT_CANVAS_PROPERTY_IMAGE_WIDTH,
	PAINT_CANVAS_PROPERTY_ROTATION,
	PAINT_CANVAS_PROPERTY_SCALING_X,
	PAINT_CANVAS_PROPERTY_SCALING_Y,
	PAINT_CANVAS_PROPERTY_TRANSLATION_X,
	PAINT_CANVAS_PROPERTY_TRANSLATION_Y,
	PAINT_CANVAS_N_PROPERTIES,
};

/* Canvas クラスのインスタンス */
struct _PaintCanvas
{
	GtkDrawingArea  *parent_instance;
	gdouble          rotation;
	gdouble          scaling_x;
	gdouble          scaling_y;
	gdouble          translation_x;
	gdouble          translation_y;
	cairo_surface_t *surfaces [PAINT_CANVAS_N_SURFACES];
	gint             image_height;
	gint             image_width;
	gboolean         auto_size;
};

static void
paint_canvas_class_init (PaintCanvasClass *);
static void
paint_canvas_class_init_object (GObjectClass *);
static void
paint_canvas_destroy (PaintCanvas *);
static void
paint_canvas_dispose (GObject *);
static void
paint_canvas_draw_auto (PaintCanvas *, cairo_t *, gint, gint);
static void
paint_canvas_draw_normal (PaintCanvas *, cairo_t *, gint, gint);
static void
paint_canvas_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
paint_canvas_init (PaintCanvas *);
static void
paint_canvas_set_property (GObject *, guint, const GValue *, GParamSpec *);

/* Canvas クラス */
G_DEFINE_FINAL_TYPE (PaintCanvas, paint_canvas, GTK_TYPE_DRAWING_AREA);

/*******************************************************************************
 * @brief キャンバス クラスを初期化する。
 ******************************************************************************/
void
paint_canvas_class_init (PaintCanvasClass *canvas)
{
	paint_canvas_class_init_object (G_OBJECT_CLASS (canvas));
}

/*******************************************************************************
 * @brief オブジェクト クラスを初期化する。
 ******************************************************************************/
void
paint_canvas_class_init_object (GObjectClass *object)
{
	object->dispose      = paint_canvas_dispose;
	object->get_property = paint_canvas_get_property;
	object->set_property = paint_canvas_set_property;
	g_object_class_install_property (object, PAINT_CANVAS_PROPERTY_AUTO_SIZE,     PAINT_PARAM_SPEC_BOOLEAN (PAINT_CANVAS_PROPERTY_AUTO_SIZE));
	g_object_class_install_property (object, PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT,  PAINT_PARAM_SPEC_INT     (PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT));
	g_object_class_install_property (object, PAINT_CANVAS_PROPERTY_IMAGE_WIDTH,   PAINT_PARAM_SPEC_INT     (PAINT_CANVAS_PROPERTY_IMAGE_WIDTH));
	g_object_class_install_property (object, PAINT_CANVAS_PROPERTY_ROTATION,      PAINT_PARAM_SPEC_DOUBLE  (PAINT_CANVAS_PROPERTY_ROTATION));
	g_object_class_install_property (object, PAINT_CANVAS_PROPERTY_SCALING_X,     PAINT_PARAM_SPEC_DOUBLE  (PAINT_CANVAS_PROPERTY_SCALING_X));
	g_object_class_install_property (object, PAINT_CANVAS_PROPERTY_SCALING_Y,     PAINT_PARAM_SPEC_DOUBLE  (PAINT_CANVAS_PROPERTY_SCALING_Y));
	g_object_class_install_property (object, PAINT_CANVAS_PROPERTY_TRANSLATION_X, PAINT_PARAM_SPEC_DOUBLE  (PAINT_CANVAS_PROPERTY_TRANSLATION_X));
	g_object_class_install_property (object, PAINT_CANVAS_PROPERTY_TRANSLATION_Y, PAINT_PARAM_SPEC_DOUBLE  (PAINT_CANVAS_PROPERTY_TRANSLATION_Y));
}

/*******************************************************************************
 * @brief サーフィスを破棄する。
 ******************************************************************************/
void
paint_canvas_destroy (PaintCanvas *canvas)
{
	cairo_surface_t **surfaces;
	guint index;
	surfaces = canvas->surfaces;

	for (index = 0; index < PAINT_CANVAS_N_SURFACES; index++)
	{
		g_clear_pointer (surfaces, cairo_surface_destroy);
		surfaces++;
	}
}

/*******************************************************************************
 * @brief オブジェクトを破棄する。
 ******************************************************************************/
void
paint_canvas_dispose (GObject *object)
{
	paint_canvas_destroy (PAINT_CANVAS (object));
	G_OBJECT_CLASS (paint_canvas_parent_class)->dispose (object);
}

/*******************************************************************************
 * @brief 画像を描画する方法を選択する。
 ******************************************************************************/
void
paint_canvas_draw (GtkDrawingArea *area, cairo_t *cairo, gint width, gint height, gpointer)
{
	PaintCanvas *canvas;
	PaintCanvasDraw draw;
	canvas = PAINT_CANVAS (area);

	if (canvas->auto_size)
	{
		draw = paint_canvas_draw_auto;
	}
	else
	{
		draw = paint_canvas_draw_normal;
	}

	draw (canvas, cairo, width, height);
}

/*******************************************************************************
 * @brief キャンバスの大きさに応じて画像を描画する。
 ******************************************************************************/
void
paint_canvas_draw_auto (PaintCanvas *canvas, cairo_t *cairo, gint width, gint height)
{
	cairo_surface_t **surfaces;
	guint index;

	if ((canvas->image_width > 0) && (canvas->image_height > 0))
	{
		surfaces = canvas->surfaces;
		cairo_scale (cairo, width / (double) canvas->image_width, height / (double) canvas->image_height);

		for (index = 0; index < PAINT_CANVAS_N_SURFACES; index++)
		{
			if (*surfaces)
			{
				cairo_set_source_surface (cairo, *surfaces, 0, 0);
				cairo_paint (cairo);
			}

			surfaces++;
		}
	}
}

/*******************************************************************************
 * @brief 指定した配置に応じて画像を描画する。
 ******************************************************************************/
void
paint_canvas_draw_normal (PaintCanvas *, cairo_t *, gint, gint)
{
}

/*******************************************************************************
 * @brief 画像を描画する方法を取得する。
 ******************************************************************************/
gboolean
paint_canvas_get_auto_size (PaintCanvas *canvas)
{
	return canvas->auto_size;
}

/*******************************************************************************
 * @brief 画像の高さを取得する。
 ******************************************************************************/
gint
paint_canvas_get_image_height (PaintCanvas *canvas)
{
	return canvas->image_height;
}

/*******************************************************************************
 * @brief 画像の幅を取得する。
 ******************************************************************************/
gint
paint_canvas_get_image_width (PaintCanvas *canvas)
{
	return canvas->image_width;
}

/*******************************************************************************
 * @brief プロパティを取得する。
 ******************************************************************************/
void
paint_canvas_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	switch (property_id)
	{
	case PAINT_CANVAS_PROPERTY_AUTO_SIZE:
		g_value_set_boolean (value, PAINT_CANVAS (object)->auto_size);
		break;
	case PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT:
		g_value_set_int (value, PAINT_CANVAS (object)->image_height);
		break;
	case PAINT_CANVAS_PROPERTY_IMAGE_WIDTH:
		g_value_set_int (value, PAINT_CANVAS (object)->image_width);
		break;
	case PAINT_CANVAS_PROPERTY_ROTATION:
		g_value_set_double (value, PAINT_CANVAS (object)->rotation);
		break;
	case PAINT_CANVAS_PROPERTY_SCALING_X:
		g_value_set_double (value, PAINT_CANVAS (object)->scaling_x);
		break;
	case PAINT_CANVAS_PROPERTY_SCALING_Y:
		g_value_set_double (value, PAINT_CANVAS (object)->scaling_y);
		break;
	case PAINT_CANVAS_PROPERTY_TRANSLATION_X:
		g_value_set_double (value, PAINT_CANVAS (object)->translation_x);
		break;
	case PAINT_CANVAS_PROPERTY_TRANSLATION_Y:
		g_value_set_double (value, PAINT_CANVAS (object)->translation_y);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

/*******************************************************************************
 * @brief 回転角度を取得する。
 ******************************************************************************/
gdouble
paint_canvas_get_rotation (PaintCanvas *canvas)
{
	return canvas->rotation;
}

/*******************************************************************************
 * @brief 拡大率を取得する。
 ******************************************************************************/
gdouble
paint_canvas_get_scaling_x (PaintCanvas *canvas)
{
	return canvas->scaling_x;
}

/*******************************************************************************
 * @brief 拡大率を取得する。
 ******************************************************************************/
gdouble
paint_canvas_get_scaling_y (PaintCanvas *canvas)
{
	return canvas->scaling_y;
}

/*******************************************************************************
 * @brief 画像を取得する。
 ******************************************************************************/
cairo_surface_t *
paint_canvas_get_surface (PaintCanvas *canvas, guint index)
{
	cairo_surface_t *surface;

	if (index < PAINT_CANVAS_N_SURFACES)
	{
		surface = canvas->surfaces [index];
	}
	else
	{
		surface = NULL;
	}

	return surface;
}

/*******************************************************************************
 * @brief 平行移動を取得する。
 ******************************************************************************/
gdouble
paint_canvas_get_translation_x (PaintCanvas *canvas)
{
	return canvas->translation_x;
}

/*******************************************************************************
 * @brief 平行移動を取得する。
 ******************************************************************************/
gdouble
paint_canvas_get_translation_y (PaintCanvas *canvas)
{
	return canvas->translation_y;
}

/*******************************************************************************
 * @brief クラスのインスタンスを初期化する。
 ******************************************************************************/
void
paint_canvas_init (PaintCanvas *canvas)
{
	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (canvas), paint_canvas_draw, NULL, NULL);
}

/*******************************************************************************
 * @brief クラスのインスタンスを作成する。
 ******************************************************************************/
GtkWidget *
paint_canvas_new (gboolean auto_size)
{
	return g_object_new (PAINT_TYPE_CANVAS,
		"auto-size", auto_size,
		"hexpand",   TRUE,
		"vexpand",   TRUE,
		NULL);
}

/*******************************************************************************
 * @brief 画像を描画する方法を設定する。
 ******************************************************************************/
void
paint_canvas_set_auto_size (PaintCanvas *canvas, gboolean auto_size)
{
	canvas->auto_size = auto_size;
}

/*******************************************************************************
 * @brief 画像の大きさを設定する。
 ******************************************************************************/
void
paint_canvas_set_image (PaintCanvas *canvas, gint image_width, gint image_height)
{
	canvas->image_width = image_width;
	canvas->image_height = image_height;
}

/*******************************************************************************
 * @brief プロパティを設定する。
 ******************************************************************************/
void
paint_canvas_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	switch (property_id)
	{
	case PAINT_CANVAS_PROPERTY_AUTO_SIZE:
		PAINT_CANVAS (object)->auto_size = g_value_get_boolean (value);
		break;
	case PAINT_CANVAS_PROPERTY_IMAGE_HEIGHT:
		PAINT_CANVAS (object)->image_height = g_value_get_int (value);
		break;
	case PAINT_CANVAS_PROPERTY_IMAGE_WIDTH:
		PAINT_CANVAS (object)->image_width = g_value_get_int (value);
		break;
	case PAINT_CANVAS_PROPERTY_SCALING_X:
		PAINT_CANVAS (object)->scaling_x = g_value_get_double (value);
		break;
	case PAINT_CANVAS_PROPERTY_SCALING_Y:
		PAINT_CANVAS (object)->scaling_y = g_value_get_double (value);
		break;
	case PAINT_CANVAS_PROPERTY_TRANSLATION_X:
		PAINT_CANVAS (object)->translation_x = g_value_get_double (value);
		break;
	case PAINT_CANVAS_PROPERTY_TRANSLATION_Y:
		PAINT_CANVAS (object)->translation_y = g_value_get_double (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

/*******************************************************************************
 * @brief 回転角度を設定する。
 ******************************************************************************/
void
paint_canvas_set_rotation (PaintCanvas *canvas, gdouble rotation)
{
	canvas->rotation = rotation;
}

/*******************************************************************************
 * @brief 拡大率を設定する。
 ******************************************************************************/
void
paint_canvas_set_scaling (PaintCanvas *canvas, gdouble scaling_x, gdouble scaling_y)
{
	canvas->scaling_x = scaling_x;
	canvas->scaling_y = scaling_y;
}

/*******************************************************************************
 * @brief 画像を設定する。参照数を増やす。
 ******************************************************************************/
void
paint_canvas_set_surface (PaintCanvas *canvas, guint index, cairo_surface_t *surface)
{
	cairo_surface_t **surfaces;

	if (index < PAINT_CANVAS_N_SURFACES)
	{
		surfaces = canvas->surfaces + index;

		if (*surfaces != surface)
		{
			if (*surfaces)
			{
				cairo_surface_destroy (*surfaces);
			}
			if (surface)
			{
				*surfaces = cairo_surface_reference (surface);
			}
			else
			{
				*surfaces = NULL;
			}
		}
	}
}

/*******************************************************************************
 * @brief 平行移動を設定する。
 ******************************************************************************/
void
paint_canvas_set_translation (PaintCanvas *canvas, gdouble translation_x, gdouble translation_y)
{
	canvas->translation_x = translation_x;
	canvas->translation_y = translation_y;
}
