/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define SIGNAL_CLICKED "clicked"
#define SIGNAL_TOGGLED "toggled"
#define TEMPLATE_NAME "/com/github/mi19a009/paint/paint-window.ui"

/* Paint Window クラスのプロパティ */
enum _PaintWindowProperty
{
	PAINT_WINDOW_PROPERTY_NULL,
	PAINT_WINDOW_PROPERTY_FILE,
	PAINT_WINDOW_PROPERTY_TOOL,
};

/* Paint Window クラスのインスタンス */
struct _PaintWindow
{
	GtkApplicationWindow parent_instance;
	/* Template Widgets */
	GtkAdjustment  *hadjustment;
	GtkAdjustment  *vadjustment;
	GtkDrawingArea *canvas;
	GtkImage       *tool_image;
	GtkLabel       *tool_label;
	GtkLabel       *tool_width_label;
	/* Private Values */
	cairo_surface_t *surface;
	GFile           *file;
	PaintTool       *tool;
};

static void     paint_window_change_adjustment_value (GtkAdjustment *adjustment, gpointer user_data);
static void     paint_window_class_init        (PaintWindowClass *self);
static void     paint_window_class_init_object (GObjectClass *self);
static void     paint_window_class_init_widget (GtkWidgetClass *self);
static void     paint_window_destroy           (PaintWindow *self);
static void     paint_window_dispose           (GObject *self);
static void     paint_window_draw              (GtkDrawingArea *canvas, cairo_t *context, int width, int height, gpointer self);
static void     paint_window_get_property      (GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static void     paint_window_init              (PaintWindow *self);
static void     paint_window_init_event_scroll (PaintWindow *self);
static gboolean paint_window_on_scroll         (GtkEventControllerScroll *scroll, gdouble dx, gdouble dy, gpointer user_data);
static void     paint_window_on_tool_width     (GObject *other, GParamSpec *pspec, gpointer user_data);
static void     paint_window_resize_canvas     (GtkDrawingArea *area, int width, int height, gpointer user_data);
static void     paint_window_set_property      (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);
static void     paint_window_update_scroll     (PaintWindow *self);
static void     paint_window_update_tool       (PaintWindow *self);
static void     paint_window_update_tool_width (PaintWindow *self);

/*******************************************************************************
Paint Window クラス:
アプリケーションのメイン ウィンドウを表します。
*/ G_DEFINE_FINAL_TYPE (PaintWindow, paint_window, GTK_TYPE_APPLICATION_WINDOW);
#define PAINT_WINDOW_PROPERTY_APPLICATION_NAME  "application"
#define PAINT_WINDOW_PROPERTY_SHOW_MENUBAR_NAME "show-menubar"
#define PAINT_WINDOW_PROPERTY_TITLE_NAME        "title"

/* File プロパティ */
#define PAINT_WINDOW_PROPERTY_FILE_NAME        "file"
#define PAINT_WINDOW_PROPERTY_FILE_NICK        "File"
#define PAINT_WINDOW_PROPERTY_FILE_BLURB       "File"
#define PAINT_WINDOW_PROPERTY_FILE_OBJECT_TYPE G_TYPE_FILE
#define PAINT_WINDOW_PROPERTY_FILE_FLAGS       G_PARAM_READWRITE

/* Tool プロパティ */
#define PAINT_WINDOW_PROPERTY_TOOL_NAME        "tool"
#define PAINT_WINDOW_PROPERTY_TOOL_NICK        "Tool"
#define PAINT_WINDOW_PROPERTY_TOOL_BLURB       "Tool"
#define PAINT_WINDOW_PROPERTY_TOOL_OBJECT_TYPE PAINT_TYPE_TOOL
#define PAINT_WINDOW_PROPERTY_TOOL_FLAGS       G_PARAM_READWRITE

/*******************************************************************************
スクロール バーの値を変更します。
*/ static void
paint_window_change_adjustment_value (GtkAdjustment *adjustment, gpointer user_data)
{
	gtk_widget_queue_draw (GTK_WIDGET (PAINT_WINDOW (user_data)->canvas));
}

/*******************************************************************************
クラスを初期化します。
*/ static void
paint_window_class_init (PaintWindowClass *self)
{
	paint_window_class_init_object (G_OBJECT_CLASS (self));
	paint_window_class_init_widget (GTK_WIDGET_CLASS (self));
}

/*******************************************************************************
GObject クラスを初期化します。
*/ static void
paint_window_class_init_object (GObjectClass *self)
{
	self->dispose = paint_window_dispose;
	self->get_property = paint_window_get_property;
	self->set_property = paint_window_set_property;
	PAINT_OBJECT_CLASS_INSTALL_PROPERTY (self, PAINT_WINDOW_PROPERTY_FILE, PAINT_PARAM_SPEC_OBJECT);
	PAINT_OBJECT_CLASS_INSTALL_PROPERTY (self, PAINT_WINDOW_PROPERTY_TOOL, PAINT_PARAM_SPEC_OBJECT);
}

/*******************************************************************************
GtkWidget クラスを初期化します。
*/ static void
paint_window_class_init_widget (GtkWidgetClass *self)
{
	gtk_widget_class_set_template_from_resource (self, TEMPLATE_NAME);
	gtk_widget_class_bind_template_child (self, PaintWindow, canvas);
	gtk_widget_class_bind_template_child (self, PaintWindow, tool_image);
	gtk_widget_class_bind_template_child (self, PaintWindow, tool_label);
	gtk_widget_class_bind_template_child (self, PaintWindow, tool_width_label);
	gtk_widget_class_bind_template_child (self, PaintWindow, hadjustment);
	gtk_widget_class_bind_template_child (self, PaintWindow, vadjustment);
	gtk_widget_class_bind_template_callback (self, paint_window_resize_canvas);
	gtk_widget_class_bind_template_callback (self, paint_window_change_adjustment_value);
}

/*******************************************************************************
プロパティを破棄します。
*/ static void
paint_window_destroy (PaintWindow *self)
{
	g_clear_pointer (&self->surface, cairo_surface_destroy);
	g_clear_object (&self->file);
	g_clear_object (&self->tool);
}

/*******************************************************************************
クラスのインスタンスを破棄します。
*/ static void
paint_window_dispose (GObject *self)
{
	paint_window_destroy (PAINT_WINDOW (self));
	gtk_widget_dispose_template (GTK_WIDGET (self), PAINT_TYPE_WINDOW);
	G_OBJECT_CLASS (paint_window_parent_class)->dispose (self);
}

/*******************************************************************************
キャンバスを描画します。
*/ static void
paint_window_draw (GtkDrawingArea *canvas, cairo_t *context, int width, int height, gpointer self)
{
	PaintWindow *properties;
	properties = PAINT_WINDOW (self);

	if (!properties->surface && properties->file)
	{
		properties->surface = paint_surface_create_from_file (cairo_get_target (context), properties->file);
		paint_window_update_scroll (properties);
	}
	if (properties->surface)
	{
		cairo_set_source_surface (context, properties->surface, -gtk_adjustment_get_value (properties->hadjustment), -gtk_adjustment_get_value (properties->vadjustment));
		cairo_paint (context);
	}
}

/*******************************************************************************
現在のファイルを取得します。
オブジェクトの参照数を増やします。
*/ GFile *
paint_window_get_file (PaintWindow *self)
{
	GFile *file;

	if (self->file)
	{
		file = g_object_ref (self->file);
	}
	else
	{
		file = NULL;
	}

	return file;
}

/*******************************************************************************
プロパティを取得します。
*/ static void
paint_window_get_property (GObject *self, guint property_id, GValue *value, GParamSpec *pspec)
{
	PaintWindow *properties;
	properties = PAINT_WINDOW (self);

	switch (property_id)
	{
	case PAINT_WINDOW_PROPERTY_FILE:
		g_value_set_object (value, properties->file);
		break;
	case PAINT_WINDOW_PROPERTY_TOOL:
		g_value_set_object (value, properties->tool);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

/*******************************************************************************
現在のツールを取得します。
オブジェクトの参照数を増やします。
*/ PaintTool *
paint_window_get_tool (PaintWindow *self)
{
	PaintTool *tool;

	if (self->tool)
	{
		tool = g_object_ref (self->tool);
	}
	else
	{
		tool = NULL;
	}

	return tool;
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/ static void
paint_window_init (PaintWindow *self)
{
	gtk_widget_init_template (GTK_WIDGET (self));
	paint_window_init_event_scroll (self);
	gtk_drawing_area_set_draw_func (self->canvas, paint_window_draw, self, NULL);
}

/*******************************************************************************
スクロール イベントを登録します。
*/ static void
paint_window_init_event_scroll (PaintWindow *self)
{
	GtkEventController *controller;
	controller = gtk_event_controller_scroll_new (GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES);

	if (controller)
	{
		g_signal_connect (controller, "scroll", G_CALLBACK (paint_window_on_scroll), self);
		gtk_widget_add_controller (GTK_WIDGET (self->canvas), controller);
	}
}

/*******************************************************************************
クラスのインスタンスを作成します。
*/ GtkWidget *
paint_window_new (GApplication *application, gboolean show_menubar, GFile *file)
{
	return g_object_new (PAINT_TYPE_WINDOW,
		PAINT_WINDOW_PROPERTY_APPLICATION_NAME, application,
		PAINT_WINDOW_PROPERTY_SHOW_MENUBAR_NAME, show_menubar,
		PAINT_WINDOW_PROPERTY_FILE_NAME, file,
		NULL);
}

/*******************************************************************************
ウィンドウをスクロールします。
イベントを処理した場合は TRUE を返します。
*/ static gboolean
paint_window_on_scroll (GtkEventControllerScroll *scroll, gdouble dx, gdouble dy, gpointer user_data)
{
	PaintWindow *self;
	self = PAINT_WINDOW (user_data);
	dx += gtk_adjustment_get_value (self->hadjustment);
	dy += gtk_adjustment_get_value (self->vadjustment);
	gtk_adjustment_set_value (self->hadjustment, dx);
	gtk_adjustment_set_value (self->vadjustment, dy);
	return TRUE;
}

static void
paint_window_on_tool_width (GObject *other, GParamSpec *pspec, gpointer user_data)
{
	paint_window_update_tool_width (PAINT_WINDOW (user_data));
}

/*******************************************************************************
描画領域の大きさを変更します。
*/ static void
paint_window_resize_canvas (GtkDrawingArea *area, int width, int height, gpointer user_data)
{
	GtkWidget *widget;
	PaintWindow *self;
	widget = GTK_WIDGET (area);
	self = PAINT_WINDOW (user_data);
	gtk_adjustment_set_page_size (self->hadjustment, gtk_widget_get_width (widget));
	gtk_adjustment_set_page_size (self->vadjustment, gtk_widget_get_height (widget));
	paint_window_update_scroll (PAINT_WINDOW (self));
}

/*******************************************************************************
現在のファイルを設定します。
オブジェクトの参照数を増やします。
*/ void
paint_window_set_file (PaintWindow *self, GFile *file)
{
	if (self->file != file)
	{
		if (self->file)
		{
			g_object_unref (self->file);
		}
		if (file)
		{
			self->file = g_object_ref (file);
		}
		else
		{
			self->file = NULL;
		}
	}
}

/*******************************************************************************
プロパティを設定します。
*/ static void
paint_window_set_property (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec)
{
	PaintWindow *properties;
	properties = PAINT_WINDOW (self);

	switch (property_id)
	{
	case PAINT_WINDOW_PROPERTY_FILE:
		paint_window_set_file (properties, g_value_get_object (value));
		break;
	case PAINT_WINDOW_PROPERTY_TOOL:
		paint_window_set_tool (properties, g_value_get_object (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

/*******************************************************************************
現在のツールを設定します。
オブジェクトの参照数を増やします。
*/ void
paint_window_set_tool (PaintWindow *self, PaintTool *tool)
{
	if (self->tool != tool)
	{
		if (self->tool)
		{
			g_object_unref (self->tool);
			g_signal_handlers_disconnect_by_func (self->tool, paint_window_on_tool_width, self);
		}
		if (tool)
		{
			self->tool = g_object_ref (tool);
			g_signal_connect (tool, "notify::width", G_CALLBACK (paint_window_on_tool_width), self);
		}
		else
		{
			self->tool = NULL;
		}

		paint_window_update_tool (self);
	}
}

/*******************************************************************************
スクロール可能な範囲を更新します。
*/ static void
paint_window_update_scroll (PaintWindow *self)
{
	cairo_surface_t *image;
	int width, height;
	width = 0;
	height = 0;

	if (self->surface)
	{
		image = cairo_surface_map_to_image (self->surface, NULL);

		if (image)
		{
			width = cairo_image_surface_get_width (image);
			height = cairo_image_surface_get_height (image);
			cairo_surface_unmap_image (self->surface, image);
		}
	}

	gtk_adjustment_set_upper (self->hadjustment, width);
	gtk_adjustment_set_upper (self->vadjustment, height);
}

/*******************************************************************************
現在のツールに関する表示を更新します。
*/ static void
paint_window_update_tool (PaintWindow *self)
{
	const char *icon_name;
	const char *nick;
	float width;
	char buffer [8];

	if (self->tool)
	{
		icon_name = paint_tool_get_icon_name (self->tool);
		nick = paint_tool_get_nick (self->tool);
		width = paint_tool_get_width (self->tool);
	}
	else
	{
		icon_name = NULL;
		nick = NULL;
		width = 0;
	}

	snprintf (buffer, G_N_ELEMENTS (buffer), "%u", (guint16) width);
	gtk_image_set_from_icon_name (self->tool_image, icon_name);
	gtk_label_set_label (self->tool_label, nick);
	gtk_label_set_label (self->tool_width_label, buffer);
}

/*******************************************************************************
ツールの幅を表示します。
*/ static void
paint_window_update_tool_width (PaintWindow *self)
{
	float width;
	char buffer [8];

	if (self->tool)
	{
		width = paint_tool_get_width (self->tool);
	}
	else
	{
		width = 0;
	}

	snprintf (buffer, G_N_ELEMENTS (buffer), "%u", (guint16) width);
	gtk_label_set_label (self->tool_width_label, buffer);
}
