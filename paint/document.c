/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "paint.h"
#define PROPERTY_APPLICATION  "application"
#define PROPERTY_SHOW_MENUBAR "show-menubar"
#define SETTINGS_HEIGHT       "window-height"
#define SETTINGS_MAXIMIZED    "window-maximized"
#define SETTINGS_WIDTH        "window-width"
#define SIGNAL_NOTIFY_STATE   "notify::state"
#define TITLE                 _("Paint")

/* クラスのインスタンス */
struct _PaintDocumentWindow
{
	GtkApplicationWindow parent_instance;
	int                  width;
	int                  height;
	int                  maximized;
};

static void paint_document_window_change_surface     (GdkSurface *surface, GParamSpec *pspec, gpointer user_data);
static void paint_document_window_class_init         (PaintDocumentWindowClass *this_class);
static void paint_document_window_class_init_object  (GObjectClass *this_class);
static void paint_document_window_class_init_widget  (GtkWidgetClass *this_class);
static void paint_document_window_connect_surface    (GtkWidget *self);
static void paint_document_window_constructed        (GObject *self);
static void paint_document_window_disconnect_surface (GtkWidget *self);
static void paint_document_window_dispose            (GObject *self);
static void paint_document_window_get_property       (GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static void paint_document_window_init               (PaintDocumentWindow *self);
static void paint_document_window_init_settings      (PaintDocumentWindow *self);
static void paint_document_window_load_settings      (PaintDocumentWindow *self);
static void paint_document_window_realize            (GtkWidget *self);
static void paint_document_window_save_settings      (PaintDocumentWindow *self);
static void paint_document_window_set_property       (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);
static void paint_document_window_size_allocate      (GtkWidget *self, int width, int height, int baseline);
static void paint_document_window_unrealize          (GtkWidget *self);
static void paint_document_window_update_size        (PaintDocumentWindow *self);

/*******************************************************************************
* Paint Document Window クラス。
* ドキュメントを格納するウィンドウを表します。
* ウィンドウ作成時はドキュメントを作成します。
* ウィンドウ破棄時はドキュメントを破棄します。
*/
G_DEFINE_FINAL_TYPE (PaintDocumentWindow, paint_document_window, GTK_TYPE_APPLICATION_WINDOW);

/*******************************************************************************
* @brief ウィンドウの大きさを更新します。
*/
static void
paint_document_window_change_surface (GdkSurface *surface, GParamSpec *pspec, gpointer user_data)
{
	PaintDocumentWindow *self;
	GdkToplevelState state;
	state = gdk_toplevel_get_state (GDK_TOPLEVEL (surface));
	self = PAINT_DOCUMENT_WINDOW (user_data);
	self->maximized = (state & GDK_TOPLEVEL_STATE_MAXIMIZED) != 0;
}

/*******************************************************************************
* @brief クラスを初期化します。
*/
static void
paint_document_window_class_init (PaintDocumentWindowClass *this_class)
{
	paint_document_window_class_init_object (G_OBJECT_CLASS (this_class));
	paint_document_window_class_init_widget (GTK_WIDGET_CLASS (this_class));
}

/*******************************************************************************
* @brief Object クラスを初期化します。
*/
static void
paint_document_window_class_init_object (GObjectClass *this_class)
{
	this_class->constructed = paint_document_window_constructed;
	this_class->dispose = paint_document_window_dispose;
	this_class->get_property = paint_document_window_get_property;
	this_class->set_property = paint_document_window_set_property;
}

/*******************************************************************************
* @brief Widget クラスを初期化します。
*/
static void
paint_document_window_class_init_widget (GtkWidgetClass *this_class)
{
	this_class->realize = paint_document_window_realize;
	this_class->size_allocate = paint_document_window_size_allocate;
	this_class->unrealize = paint_document_window_unrealize;
}

/*******************************************************************************
* @brief サーフィス通知を購読します。
*/
static void
paint_document_window_connect_surface (GtkWidget *self)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (GTK_NATIVE (self));
	g_signal_connect (surface, SIGNAL_NOTIFY_STATE, G_CALLBACK (paint_document_window_change_surface), self);
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
paint_document_window_constructed (GObject *self)
{
	paint_document_window_init_settings (PAINT_DOCUMENT_WINDOW (self));
	G_OBJECT_CLASS (paint_document_window_parent_class)->constructed (self);
}

/*******************************************************************************
* @brief サーフィス通知を解約します。
*/
static void
paint_document_window_disconnect_surface (GtkWidget *self)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (GTK_NATIVE (self));
	g_signal_handlers_disconnect_by_func (surface, paint_document_window_change_surface, self);
}

/*******************************************************************************
* @brief クラスのインスタンスを破棄します。
*/
static void
paint_document_window_dispose (GObject *self)
{
	paint_document_window_save_settings (PAINT_DOCUMENT_WINDOW (self));
	G_OBJECT_CLASS (paint_document_window_parent_class)->dispose (self);
}

/*******************************************************************************
* @brief プロパティを取得します。
*/
static void
paint_document_window_get_property (GObject *self, guint property_id, GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
paint_document_window_init (PaintDocumentWindow *self)
{
	gtk_window_set_title (GTK_WINDOW (self), TITLE);
}

/*******************************************************************************
* @brief 環境設定を適用します。
*/
static void
paint_document_window_init_settings (PaintDocumentWindow *self)
{
	GtkWindow *window;
	window = GTK_WINDOW (self);
	paint_document_window_load_settings (self);
	gtk_window_set_default_size (window, self->width, self->height);

	if (self->maximized)
	{
		gtk_window_maximize (window);
	}
}

/*******************************************************************************
* @brief 環境設定を読み込みます。
*/
static void
paint_document_window_load_settings (PaintDocumentWindow *self)
{
	GSettings *settings;
	settings = paint_get_settings ();
	self->width = g_settings_get_int (settings, SETTINGS_WIDTH);
	self->height = g_settings_get_int (settings, SETTINGS_HEIGHT);
	self->maximized = g_settings_get_boolean (settings, SETTINGS_MAXIMIZED);
	g_object_unref (settings);
}

/*******************************************************************************
* @brief クラスのインスタンスを作成します。
*/
GtkWidget *
paint_document_window_new (GApplication *application)
{
	return g_object_new (PAINT_TYPE_DOCUMENT_WINDOW,
		PROPERTY_APPLICATION, application,
		PROPERTY_SHOW_MENUBAR, TRUE,
		NULL);
}

/*******************************************************************************
* @brief ウィンドウを表示します。
*/
static void
paint_document_window_realize (GtkWidget *self)
{
	GTK_WIDGET_CLASS (paint_document_window_parent_class)->realize (self);
	paint_document_window_connect_surface (self);
}

/*******************************************************************************
* @brief 環境設定を書き込みます。
*/
static void
paint_document_window_save_settings (PaintDocumentWindow *self)
{
	GSettings *settings;
	settings = paint_get_settings ();
	g_settings_set_int (settings, SETTINGS_WIDTH, self->width);
	g_settings_set_int (settings, SETTINGS_HEIGHT, self->height);
	g_settings_set_boolean (settings, SETTINGS_MAXIMIZED, self->maximized);
	g_object_unref (settings);
}

/*******************************************************************************
* @brief プロパティを設定します。
*/
static void
paint_document_window_set_property (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
}

/*******************************************************************************
* @brief ウィンドウの大きさを更新します。
*/
static void
paint_document_window_size_allocate (GtkWidget *self, int width, int height, int baseline)
{
	GTK_WIDGET_CLASS (paint_document_window_parent_class)->size_allocate (self, width, height, baseline);
	paint_document_window_update_size (PAINT_DOCUMENT_WINDOW (self));
}

/*******************************************************************************
* @brief ウィンドウを隠蔽します。
*/
static void
paint_document_window_unrealize (GtkWidget *self)
{
	paint_document_window_disconnect_surface (self);
	GTK_WIDGET_CLASS (paint_document_window_parent_class)->unrealize (self);
}

/*******************************************************************************
* @brief ウィンドウの大きさを更新します。
*/
static void
paint_document_window_update_size (PaintDocumentWindow *self)
{
	if (!self->maximized)
	{
		gtk_window_get_default_size (GTK_WINDOW (self), &self->width, &self->height);
	}
}
