/* Copyright (C) 2025 Taichi Murakami. */
#include "paint.h"
#include "text.h"
#define PAINT_PREVIEW_WINDOW_DEFAULT_HEIGHT                300
#define PAINT_PREVIEW_WINDOW_DEFAULT_WIDTH                 300
#define PAINT_PREVIEW_WINDOW_PROPERTY_CANVAS_NAME          "canvas"
#define PAINT_PREVIEW_WINDOW_PROPERTY_CANVAS_NICK          "Canvas"
#define PAINT_PREVIEW_WINDOW_PROPERTY_CANVAS_BLURB         "Canvas"
#define PAINT_PREVIEW_WINDOW_PROPERTY_CANVAS_DEFAULT_VALUE NULL
#define PAINT_PREVIEW_WINDOW_PROPERTY_CANVAS_FLAGS         G_PARAM_READABLE

/* 縮小表示ウィンドウ クラスのプロパティ */
enum _PaintPreviewWindowProperty
{
	PAINT_PREVIEW_WINDOW_PROPERTY_0,
	PAINT_PREVIEW_WINDOW_PROPERTY_CANVAS,
	PAINT_PREVIEW_WINDOW_N_PROPERTIES,
};

/* 縮小表示ウィンドウ クラスのインスタンス */
struct _PaintPreviewWindow
{
	GtkWindow  parent_instance;
	GtkWidget *canvas;
};

static void
paint_preview_window_class_init (PaintPreviewWindowClass *);
static void
paint_preview_window_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
paint_preview_window_init (PaintPreviewWindow *);

/* 縮小表示ウィンドウ クラス */
G_DEFINE_FINAL_TYPE (PaintPreviewWindow, paint_preview_window, GTK_TYPE_WINDOW);

/*******************************************************************************
 * @brief オブジェクト クラスを初期化する。
 ******************************************************************************/
void
paint_preview_window_class_init (PaintPreviewWindowClass *preview)
{
	GObjectClass *object;
	object = G_OBJECT_CLASS (preview);
	object->get_property = paint_preview_window_get_property;
	g_object_class_install_property (object, PAINT_PREVIEW_WINDOW_PROPERTY_CANVAS, PAINT_PARAM_SPEC_OBJECT (PAINT_PREVIEW_WINDOW_PROPERTY_CANVAS, PAINT_TYPE_CANVAS));
}

/*******************************************************************************
 * @brief キャンバスを取得する。
 ******************************************************************************/
GtkWidget *
paint_preview_window_get_canvas (PaintPreviewWindow *preview)
{
	return preview->canvas;
}

/*******************************************************************************
 * @brief プロパティを取得する。
 ******************************************************************************/
void
paint_preview_window_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	switch (property_id)
	{
	case PAINT_PREVIEW_WINDOW_PROPERTY_CANVAS:
		g_value_set_object (value, PAINT_PREVIEW_WINDOW (object)->canvas);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

/*******************************************************************************
 * @brief キャンバスを作成する。
 ******************************************************************************/
void
paint_preview_window_init (PaintPreviewWindow *preview)
{
	preview->canvas = paint_canvas_new (TRUE);

	if (preview->canvas)
	{
		gtk_window_set_child (GTK_WINDOW (preview), preview->canvas);
	}
}

/*******************************************************************************
 * @brief 縮小表示ウィンドウ クラスのインスタンスを作成する。
 ******************************************************************************/
GtkWidget *
paint_preview_window_new (GtkWindow *parent)
{
	return g_object_new (PAINT_TYPE_PREVIEW_WINDOW,
		"default-height",      PAINT_PREVIEW_WINDOW_DEFAULT_HEIGHT,
		"default-width",       PAINT_PREVIEW_WINDOW_DEFAULT_WIDTH,
		"destroy-with-parent", TRUE,
		"title",               "Preview",
		"transient-for",       parent,
		NULL);
}
