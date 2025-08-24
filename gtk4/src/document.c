/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define LOG_SETTINGS_LOADED  "Settings cannot be loaded."
#define LOG_SETTINGS_SAVED   "Settings cannot be saved."
#define SETTINGS_FULLSCREEN  "fullscreen"
#define SETTINGS_MAXIMIZED   "maximized"
#define SETTINGS_SIZE        "window-size"
#define SETTINGS_SIZE_FORMAT "(ii)"
#define SIGNAL_NOTIFY_STATE  "notify::state"
#define TEMPLATE_NAME        "gtk/document.ui"

/* Paint Document Window クラスのインスタンス */
struct _PaintDocumentWindow
{
	GtkApplicationWindow parent_instance;
	int                  height;
	int                  width;
	unsigned char        fullscreen;
	unsigned char        maximized;
};

static void       paint_document_window_change_state      (GtkWidget *widget);
static void       paint_document_window_class_init        (PaintDocumentWindowClass *window);
static void       paint_document_window_class_init_object (GObjectClass *object);
static void       paint_document_window_class_init_widget (GtkWidgetClass *widget);
static void       paint_document_window_constructed       (GObject *object);
static void       paint_document_window_dispose           (GObject *object);
static void       paint_document_window_init              (PaintDocumentWindow *window);
static void       paint_document_window_realize           (GtkWidget *widget);
static void       paint_document_window_settings_apply    (PaintDocumentWindow *settings);
static void       paint_document_window_settings_load     (PaintDocumentWindow *window);
static GSettings *paint_document_window_settings_new      (GtkWindow *window);
static void       paint_document_window_settings_save     (PaintDocumentWindow *window);
static void       paint_document_window_size_allocate     (GtkWidget *widget, int width, int height, int baseline);
static void       paint_document_window_size_update       (PaintDocumentWindow *window);
static void       paint_document_window_unrealize         (GtkWidget *widget);

/*******************************************************************************
Paint Document Window クラス:
ドキュメント ウィンドウを表します。
ユーザーがドキュメントを編集する方法を提供します。
*/ G_DEFINE_FINAL_TYPE (PaintDocumentWindow, paint_document_window, GTK_TYPE_APPLICATION_WINDOW);
#define PAINT_DOCUMENT_WINDOW_PROPERTY_APPLICATION_NAME "application"
#define PAINT_DOCUMENT_WINDOW_PROPERTY_SHOW_MENUBAR_NAME "show-menubar"

/*******************************************************************************
現在のウィンドウの状態を更新します。
*/ static void
paint_document_window_change_state (GtkWidget *widget)
{
	PaintDocumentWindow *window;
	GdkSurface *surface;
	GdkToplevelState state;
	surface = gtk_native_get_surface (GTK_NATIVE (widget));

	if (surface)
	{
		state = gdk_toplevel_get_state (GDK_TOPLEVEL (surface));
		window = PAINT_DOCUMENT_WINDOW (widget);
		window->fullscreen = (state & GDK_TOPLEVEL_STATE_FULLSCREEN) != 0;
		window->maximized = (state & GDK_TOPLEVEL_STATE_MAXIMIZED) != 0;
	}
}

/*******************************************************************************
Class Init:
クラスを初期化します。
*/ static void
paint_document_window_class_init (PaintDocumentWindowClass *window)
{
	paint_document_window_class_init_object (G_OBJECT_CLASS (window));
	paint_document_window_class_init_widget (GTK_WIDGET_CLASS (window));
}

/*******************************************************************************
Class Init Object:
Objectクラスを初期化します。
*/ static void
paint_document_window_class_init_object (GObjectClass *object)
{
	object->constructed = paint_document_window_constructed;
	object->dispose = paint_document_window_dispose;
}

/*******************************************************************************
Class Init Widget:
Widget クラスを初期化します。
*/ static void
paint_document_window_class_init_widget (GtkWidgetClass *widget)
{
	char path [PAINT_RESOURCE_PATH_CCH];
	paint_get_resource_path (path, PAINT_RESOURCE_PATH_CCH, TEMPLATE_NAME);
	gtk_widget_class_set_template_from_resource (widget, path);
	widget->size_allocate = paint_document_window_size_allocate;
	widget->realize = paint_document_window_realize;
	widget->unrealize = paint_document_window_unrealize;
}

/*******************************************************************************
環境設定を読み込みます。
*/ static void
paint_document_window_constructed (GObject *object)
{
	PaintDocumentWindow *window;
	window = PAINT_DOCUMENT_WINDOW (object);
	paint_document_window_settings_load (window);
	paint_document_window_settings_apply (window);
	G_OBJECT_CLASS (paint_document_window_parent_class)->constructed (object);
}

/*******************************************************************************
クラスのインスタンスを破棄します。
*/ static void
paint_document_window_dispose (GObject *object)
{
	paint_document_window_settings_save (PAINT_DOCUMENT_WINDOW (object));
	gtk_widget_dispose_template (GTK_WIDGET (object), PAINT_TYPE_DOCUMENT_WINDOW);
	G_OBJECT_CLASS (paint_document_window_parent_class)->dispose (object);
}

/*******************************************************************************
Init:
クラスのインスタンスを初期化します。
*/ static void
paint_document_window_init (PaintDocumentWindow *window)
{
	window->height = -1;
	window->width = -1;
	gtk_widget_init_template (GTK_WIDGET (window));
}

/*******************************************************************************
New:
新しいインスタンスを作成します。
*/ GtkWidget *
paint_document_window_new (GApplication *application)
{
	return g_object_new (PAINT_TYPE_DOCUMENT_WINDOW,
		PAINT_DOCUMENT_WINDOW_PROPERTY_APPLICATION_NAME, application,
		PAINT_DOCUMENT_WINDOW_PROPERTY_SHOW_MENUBAR_NAME, TRUE,
		NULL);
}

/*******************************************************************************
ウィンドウ状態監視を開始します。
*/ static void
paint_document_window_realize (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (paint_document_window_parent_class)->realize (widget);
	g_signal_connect_swapped (gtk_native_get_surface (GTK_NATIVE (widget)), SIGNAL_NOTIFY_STATE, G_CALLBACK (paint_document_window_change_state), widget);
}

/*******************************************************************************
現在のウィンドウに環境設定を適用します。
*/ static void
paint_document_window_settings_apply (PaintDocumentWindow *settings)
{
	GtkWindow *window;
	window = GTK_WINDOW (settings);
	gtk_window_set_default_size (window, settings->width, settings->height);

	if (settings->maximized)
	{
		gtk_window_maximize (window);
	}
	if (settings->fullscreen)
	{
		gtk_window_fullscreen (window);
	}
}

/*******************************************************************************
現在の環境設定を読み込みます。
*/ static void
paint_document_window_settings_load (PaintDocumentWindow *window)
{
	GSettings *settings;
	settings = paint_document_window_settings_new (GTK_WINDOW (window));

	if (settings)
	{
		g_settings_get (settings, SETTINGS_SIZE, SETTINGS_SIZE_FORMAT, &window->width, &window->height);
		window->fullscreen = g_settings_get_boolean (settings, SETTINGS_FULLSCREEN);
		window->maximized = g_settings_get_boolean (settings, SETTINGS_MAXIMIZED);
		g_object_unref (settings);
	}
	else
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, LOG_SETTINGS_LOADED);
	}
}

/*******************************************************************************
現在の環境設定を読み込みます。
*/ static GSettings *
paint_document_window_settings_new (GtkWindow *window)
{
	GSettings *settings;
	GtkApplication *application;
	application = gtk_window_get_application (window);

	if (application)
	{
		settings = g_settings_new (g_application_get_application_id (G_APPLICATION (application)));
	}
	else
	{
		settings = NULL;
	}

	return settings;
}

/*******************************************************************************
現在の環境設定を保存します。
*/ static void
paint_document_window_settings_save (PaintDocumentWindow *window)
{
	GSettings *settings;
	settings = paint_document_window_settings_new (GTK_WINDOW (window));

	if (settings)
	{
		g_settings_set (settings, SETTINGS_SIZE, SETTINGS_SIZE_FORMAT, window->width, window->height);
		g_settings_set_boolean (settings, SETTINGS_FULLSCREEN, window->fullscreen);
		g_settings_set_boolean (settings, SETTINGS_MAXIMIZED, window->maximized);
		g_object_unref (settings);
	}
	else
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, LOG_SETTINGS_SAVED);
	}
}

/*******************************************************************************
現在のウィンドウの大きさを取得します。
*/ static void
paint_document_window_size_allocate (GtkWidget *widget, int width, int height, int baseline)
{
	GTK_WIDGET_CLASS (paint_document_window_parent_class)->size_allocate (widget, width, height, baseline);
	paint_document_window_size_update (PAINT_DOCUMENT_WINDOW (widget));
}

/*******************************************************************************
現在のウィンドウの大きさを取得します。
*/ static void
paint_document_window_size_update (PaintDocumentWindow *window)
{
	if (!window->maximized && !window->fullscreen)
	{
		gtk_window_get_default_size (GTK_WINDOW (window), &window->width, &window->height);
	}
}

/*******************************************************************************
ウィンドウ状態監視を終了します。
*/ static void
paint_document_window_unrealize (GtkWidget *widget)
{
	g_signal_handlers_disconnect_by_func (gtk_native_get_surface (GTK_NATIVE(widget)), paint_document_window_change_state, widget);
	GTK_WIDGET_CLASS (paint_document_window_parent_class)->unrealize (widget);
}
