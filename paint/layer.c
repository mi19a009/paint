// /*
// Copyright (C) 2025 Taichi Murakami.
// Layer クラスを実装する。
// このクラスはユーザーがレイヤーとして選択可能な編集単位を表す。
// このクラスはユーザーが編集する対象として Cairo Image サーフィスを所有する。
// このクラスは [元に戻す] および [やり直し] を実行するためにサーフィスの配列を所有する。
// このクラスのコンストラクターが呼び出されると、中身が無いインスタンスができあがる。
// ヘルパー関数を用いてこのクラスのインスタンスの中身を指定できる。
// */
// #include <cairo/cairo.h>
// #include <gio/gio.h>
// #include <gdk/gdk.h>
// #include "paint.h"
// #define PAINT_LAYER_BITS_PER_SAMPLE 8
// #define PAINT_LAYER_SURFACE_FORMAT CAIRO_FORMAT_ARGB32
// #define PAINT_LAYER_PROPERTY_HEIGHT_NAME              "height"
// #define PAINT_LAYER_PROPERTY_HEIGHT_NICK              "Layer Height"
// #define PAINT_LAYER_PROPERTY_HEIGHT_BLURB             "Layer Height"
// #define PAINT_LAYER_PROPERTY_HEIGHT_MINIMUM           1
// #define PAINT_LAYER_PROPERTY_HEIGHT_MAXIMUM           (G_MAXUINT16 + 1)
// #define PAINT_LAYER_PROPERTY_HEIGHT_DEFAULT_VALUE     480
// #define PAINT_LAYER_PROPERTY_HEIGHT_FLAGS             G_PARAM_READABLE
// #define PAINT_LAYER_PROPERTY_N_PLANES_NAME            "n-planes"
// #define PAINT_LAYER_PROPERTY_N_PLANES_NICK            "Number of Planes"
// #define PAINT_LAYER_PROPERTY_N_PLANES_BLURB           "Number of Planes"
// #define PAINT_LAYER_PROPERTY_N_PLANES_MINIMUM         2
// #define PAINT_LAYER_PROPERTY_N_PLANES_MAXIMUM         G_MAXUINT8
// #define PAINT_LAYER_PROPERTY_N_PLANES_DEFAULT_VALUE   16
// #define PAINT_LAYER_PROPERTY_N_PLANES_FLAGS           G_PARAM_READABLE
// #define PAINT_LAYER_PROPERTY_NAME_NAME                "name"
// #define PAINT_LAYER_PROPERTY_NAME_NICK                "Layer Name"
// #define PAINT_LAYER_PROPERTY_NAME_BLURB               "Layer Name"
// #define PAINT_LAYER_PROPERTY_NAME_LENGTH              G_MAXINT16
// #define PAINT_LAYER_PROPERTY_NAME_DEFAULT_VALUE       "Layer"
// #define PAINT_LAYER_PROPERTY_NAME_FLAGS               G_PARAM_READWRITE | G_PARAM_CONSTRUCT
// #define PAINT_LAYER_PROPERTY_VISIBILITY_NAME          "visibility"
// #define PAINT_LAYER_PROPERTY_VISIBILITY_NICK          "Layer Visibility"
// #define PAINT_LAYER_PROPERTY_VISIBILITY_BLURB         "Layer Visibility"
// #define PAINT_LAYER_PROPERTY_VISIBILITY_DEFAULT_VALUE PAINT_VISIBILITY_EDIT
// #define PAINT_LAYER_PROPERTY_VISIBILITY_FLAGS         G_PARAM_READWRITE | G_PARAM_CONSTRUCT
// #define PAINT_LAYER_PROPERTY_WIDTH_NAME               "width"
// #define PAINT_LAYER_PROPERTY_WIDTH_NICK               "Layer Width"
// #define PAINT_LAYER_PROPERTY_WIDTH_BLURB              "Layer Width"
// #define PAINT_LAYER_PROPERTY_WIDTH_MINIMUM            1
// #define PAINT_LAYER_PROPERTY_WIDTH_MAXIMUM            (G_MAXUINT16 + 1)
// #define PAINT_LAYER_PROPERTY_WIDTH_DEFAULT_VALUE      640
// #define PAINT_LAYER_PROPERTY_WIDTH_FLAGS              G_PARAM_READABLE

// enum _PaintLayerProperty
// {
// 	PAINT_LAYER_PROPERTY_0,
// 	PAINT_LAYER_PROPERTY_HEIGHT,
// 	PAINT_LAYER_PROPERTY_N_PLANES,
// 	PAINT_LAYER_PROPERTY_NAME,
// 	PAINT_LAYER_PROPERTY_VISIBILITY,
// 	PAINT_LAYER_PROPERTY_WIDTH,
// };

// /* クラスのインスタンス */
// struct _PaintLayer
// {
// 	GObject parent_instance;
// 	gchar  *name;
// 	GList   plane;
// 	GList  *planes;
// 	GQueue  planes_for_free;
// 	GQueue  planes_for_undo;
// 	GQueue  planes_for_work;
// 	gint32  width;
// 	gint32  height;
// 	guint32 n_planes;
// 	guint16 name_length;
// 	guint8  visibility;
// };

// static void
// paint_layer_destroy (PaintLayer *);
// static void
// paint_layer_destroy_name (PaintLayer *);
// static void
// paint_layer_destroy_planes (PaintLayer *);
// static void
// paint_layer_dispose (GObject *);
// static void
// paint_layer_class_init (PaintLayerClass *);
// static void
// paint_layer_get_property (GObject *, guint, GValue *, GParamSpec *);
// static void
// paint_layer_init (PaintLayer *);
// static void
// paint_layer_init_planes (PaintLayer *layer, PaintSurface *surface, unsigned n_planes);
// static void
// paint_layer_set_property (GObject *, guint, const GValue *, GParamSpec *);

// G_DEFINE_FINAL_TYPE (PaintLayer, paint_layer, G_TYPE_OBJECT);

// /*******************************************************************************
//  * @brief クラスを初期化する。
//  ******************************************************************************/
// void
// paint_layer_class_init (PaintLayerClass *layer)
// {
// 	GObjectClass *object;
// 	object               = G_OBJECT_CLASS (layer);
// 	object->dispose      = paint_layer_dispose;
// 	object->get_property = paint_layer_get_property;
// 	object->set_property = paint_layer_set_property;
// 	g_object_class_install_property (object, PAINT_LAYER_PROPERTY_HEIGHT,     PAINT_PARAM_SPEC_INT    (PAINT_LAYER_PROPERTY_HEIGHT));
// 	g_object_class_install_property (object, PAINT_LAYER_PROPERTY_N_PLANES,   PAINT_PARAM_SPEC_UINT   (PAINT_LAYER_PROPERTY_N_PLANES));
// 	g_object_class_install_property (object, PAINT_LAYER_PROPERTY_NAME,       PAINT_PARAM_SPEC_STRING (PAINT_LAYER_PROPERTY_NAME));
// 	g_object_class_install_property (object, PAINT_LAYER_PROPERTY_VISIBILITY, PAINT_PARAM_SPEC_ENUM   (PAINT_LAYER_PROPERTY_VISIBILITY, PAINT_TYPE_VISIBILITY));
// 	g_object_class_install_property (object, PAINT_LAYER_PROPERTY_WIDTH,      PAINT_PARAM_SPEC_INT    (PAINT_LAYER_PROPERTY_WIDTH));
// }

// /*******************************************************************************
//  * @brief メンバー変数を破棄する。
//  ******************************************************************************/
// void
// paint_layer_destroy (PaintLayer *layer)
// {
// 	paint_layer_destroy_name (layer);
// 	paint_layer_destroy_planes (layer);
// }

// /*******************************************************************************
//  * @brief サーフィスの名前を破棄する。
//  ******************************************************************************/
// void
// paint_layer_destroy_name (PaintLayer *layer)
// {
// 	g_clear_pointer (&layer->name, g_free);
// 	layer->name_length = 0;
// }

// /*******************************************************************************
//  * @brief 面の配列を破棄する。
//  ******************************************************************************/
// void
// paint_layer_destroy_planes (PaintLayer *layer)
// {
// 	cairo_surface_t *surface;
// 	GList *planes;
// 	unsigned index, n_planes;
// 	surface = layer->plane.data;
// 	planes = layer->planes;

// 	if (surface)
// 	{
// 		cairo_surface_destroy (surface);
// 		memset (&layer->plane, 0, sizeof (GList));
// 	}
// 	if (planes)
// 	{
// 		n_planes = layer->n_planes;

// 		for (index = 0; index < n_planes; index++)
// 		{
// 			surface = (planes++)->data;

// 			if (surface)
// 			{
// 				cairo_surface_destroy (surface);
// 			}
// 		}

// 		g_free (layer->planes);
// 		layer->planes = NULL;
// 	}

// 	layer->width = 0;
// 	layer->height = 0;
// 	layer->n_planes = 0;
// 	memset (&layer->planes_for_free, 0, sizeof (GQueue));
// 	memset (&layer->planes_for_undo, 0, sizeof (GQueue));
// 	memset (&layer->planes_for_work, 0, sizeof (GQueue));
// }

// /*******************************************************************************
//  * @brief クラスのインスタンスを破棄する。
//  ******************************************************************************/
// void
// paint_layer_dispose (GObject *object)
// {
// 	paint_layer_destroy (PAINT_LAYER (object));
// 	G_OBJECT_CLASS (paint_layer_parent_class)->dispose (object);
// }

// /*******************************************************************************
//  * @brief サーフィスの高さを取得する。
//  ******************************************************************************/
// int
// paint_layer_get_height (PaintLayer *layer)
// {
// 	return layer->height;
// }

// /*******************************************************************************
//  * @brief 面の配列の要素数を取得する。
//  ******************************************************************************/
// unsigned
// paint_layer_get_n_planes (PaintLayer *layer)
// {
// 	return layer->n_planes;
// }

// /*******************************************************************************
//  * @brief サーフィスの名前を取得する。
//  ******************************************************************************/
// const char *
// paint_layer_get_name (PaintLayer *layer)
// {
// 	return layer->name;
// }

// /*******************************************************************************
//  * @brief プロパティを取得する。
//  ******************************************************************************/
// void
// paint_layer_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
// {
// 	switch (property_id)
// 	{
// 	case PAINT_LAYER_PROPERTY_HEIGHT:
// 		g_value_set_int (value, PAINT_LAYER (object)->height);
// 		break;
// 	case PAINT_LAYER_PROPERTY_N_PLANES:
// 		g_value_set_uint (value, PAINT_LAYER (object)->n_planes);
// 		break;
// 	case PAINT_LAYER_PROPERTY_NAME:
// 		g_value_set_string (value, PAINT_LAYER (object)->name);
// 		break;
// 	case PAINT_LAYER_PROPERTY_VISIBILITY:
// 		g_value_set_enum (value, PAINT_LAYER (object)->visibility);
// 		break;
// 	case PAINT_LAYER_PROPERTY_WIDTH:
// 		g_value_set_int (value, PAINT_LAYER (object)->width);
// 		break;
// 	default:
// 		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
// 		break;
// 	}
// }

// /*******************************************************************************
//  * @brief 現在のサーフィスを取得する。
//  ******************************************************************************/
// PaintSurface *
// paint_layer_get_surface (PaintLayer *layer)
// {
// 	GList *plane;
// 	plane = layer->planes_for_work.head;
// 	return plane ? plane->data : NULL;
// }

// /*******************************************************************************
//  * @brief レイヤーの表示状態を取得する。
//  ******************************************************************************/
// PaintVisibility
// paint_layer_get_visibility (PaintLayer *layer)
// {
// 	return layer->visibility;
// }

// /*******************************************************************************
//  * @brief サーフィスの幅を取得する。
//  ******************************************************************************/
// int
// paint_layer_get_width (PaintLayer *layer)
// {
// 	return layer->width;
// }

// /*******************************************************************************
//  * @brief クラスのインスタンスを初期化する。
//  ******************************************************************************/
// void
// paint_layer_init (PaintLayer *)
// {
// }

// /*******************************************************************************
//  * @brief 面の配列を作成する。
//  * @param surface サーフィス。
//  * @param n_planes 配列の要素数。
//  ******************************************************************************/
// void
// paint_layer_init_planes (PaintLayer *layer, PaintSurface *surface, unsigned n_planes)
// {
// 	GList *planes;
// 	GQueue *queue;
// 	int format, width, height;
// 	unsigned index;

// 	if (surface && !layer->plane.data && !layer->planes)
// 	{
// 		format = cairo_image_surface_get_format (surface);
// 		width = cairo_image_surface_get_width (surface);
// 		height = cairo_image_surface_get_height (surface);
// 		layer->plane.data = surface;
// 		layer->width = width;
// 		layer->height = height;
// 		g_queue_push_tail_link (&layer->planes_for_work, &layer->plane);

// 		if (n_planes)
// 		{
// 			planes = g_malloc_n (sizeof (GList), n_planes);

// 			if (planes)
// 			{
// 				queue = &layer->planes_for_free;
// 				layer->planes = planes;
// 				layer->n_planes = n_planes;

// 				for (index = 0; index < n_planes; index++)
// 				{
// 					planes->data = cairo_image_surface_create (format, width, height);
// 					planes->next = NULL;
// 					planes->prev = NULL;
// 					g_queue_push_tail_link (queue, planes++);
// 				}
// 			}
// 		}
// 	}
// }

// /*******************************************************************************
//  * @brief 新しいレイヤーを作成する。
//  * @param width サーフィスの幅。
//  * @param height サーフィスの高さ。
//  * @param n_planes 面の配列の要素数。
//  ******************************************************************************/
// PaintLayer *
// paint_layer_new (int width, int height, unsigned n_planes)
// {
// 	PaintLayer *layer;
// 	layer = g_object_new (PAINT_TYPE_LAYER, NULL);
// 	paint_layer_reset (layer, cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height), n_planes);
// 	return layer;
// }

// /*******************************************************************************
//  * @brief 新しいレイヤーを作成する。画像ファイルを読み込む。
//  * @param file 画像ファイル。
//  * @param n_planes 面の配列の要素数。
//  ******************************************************************************/
// PaintLayer *
// paint_layer_new_from_file (GFile *file, unsigned n_planes)
// {
// 	PaintLayer *layer;
// 	layer = g_object_new (PAINT_TYPE_LAYER, NULL);
// 	paint_layer_reset (layer, paint_surface_new_from_file (file), n_planes);
// 	return layer;
// }

// /*******************************************************************************
//  * @brief サーフィスを初期化する。
//  * @param surface サーフィス。
//  * @param n_planes 面の配列の要素数。
//  ******************************************************************************/
// void
// paint_layer_reset (PaintLayer *layer, PaintSurface *surface, unsigned n_planes)
// {
// 	paint_layer_destroy_planes (layer);
// 	paint_layer_init_planes (layer, surface, n_planes);
// }

// /*******************************************************************************
//  * @brief レイヤーの名前を設定する。
//  * 指定した文字列に対応して格納先が伸縮する。
//  * @param name レイヤーの名前を指定する。
//  ******************************************************************************/
// void
// paint_layer_set_name (PaintLayer *layer, const char *name)
// {
// 	char *buffer;
// 	size_t size;

// 	if (name)
// 	{
// 		size = strlen (name) + 1;

// 		if (layer->name)
// 		{
// 			buffer = g_realloc (layer->name, size);
// 		}
// 		else
// 		{
// 			buffer = g_malloc (size);
// 		}
// 		if (buffer)
// 		{
// 			memcpy (buffer, name, size);
// 			layer->name = buffer;
// 		}
// 	}
// 	else if (layer->name)
// 	{
// 		g_free (layer->name);
// 		layer->name = NULL;
// 	}
// }

// /*******************************************************************************
//  * @brief プロパティを設定する。
//  ******************************************************************************/
// void
// paint_layer_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
// {
// 	switch (property_id)
// 	{
// 	case PAINT_LAYER_PROPERTY_NAME:
// 		paint_layer_set_name (PAINT_LAYER (object), g_value_get_string (value));
// 		break;
// 	case PAINT_LAYER_PROPERTY_VISIBILITY:
// 		paint_layer_set_visibility (PAINT_LAYER (object), g_value_get_enum (value));
// 		break;
// 	default:
// 		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
// 		break;
// 	}
// }

// /*******************************************************************************
//  * @brief 表示状態を設定する。
//  ******************************************************************************/
// void
// paint_layer_set_visibility (PaintLayer *layer, PaintVisibility visibility)
// {
// 	layer->visibility = visibility;
// }
