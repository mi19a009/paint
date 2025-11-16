/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "draw.h"
#include "share.h"
#define LOGO_ICON_NAME        "draw"
#define PROPERTY_APPLICATION  "application"
#define PROPERTY_SHOW_MENUBAR "show-menubar"
#define SETTINGS_HEIGHT       "window-height"
#define SETTINGS_MAXIMIZED    "window-maximized"
#define SETTINGS_WIDTH        "window-width"
#define SIGNAL_NOTIFY_STATE   "notify::state"
#define TITLE                 _("Draw")

/* クラスのインスタンス */
struct _DrawDocumentWindow
{
	GtkApplicationWindow parent_instance;
	int                  width;
	int                  height;
	int                  maximized;
};

static void draw_document_window_activate_about     (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void draw_document_window_class_init         (DrawDocumentWindowClass *this_class);
static void draw_document_window_class_init_object  (GObjectClass *this_class);
static void draw_document_window_class_init_widget  (GtkWidgetClass *this_class);
static void draw_document_window_constructed        (GObject *self);
static void draw_document_window_dispose            (GObject *self);
static void draw_document_window_get_property       (GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static void draw_document_window_init               (DrawDocumentWindow *self);
static void draw_document_window_init_settings      (DrawDocumentWindow *self);
static void draw_document_window_realize            (GtkWidget *self);
static void draw_document_window_set_property       (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);
static void draw_document_window_settings_apply     (DrawDocumentWindow *self);
static void draw_document_window_settings_load      (DrawDocumentWindow *self);
static void draw_document_window_settings_save      (DrawDocumentWindow *self);
static void draw_document_window_size_allocate      (GtkWidget *self, int width, int height, int baseline);
static void draw_document_window_surface_changed    (GdkSurface *surface, GParamSpec *pspec, gpointer user_data);
static void draw_document_window_surface_connect    (GtkWidget *self);
static void draw_document_window_surface_disconnect (GtkWidget *self);
static void draw_document_window_unrealize          (GtkWidget *self);
static void draw_document_window_update_size        (DrawDocumentWindow *self);

/*******************************************************************************
* Draw Document Window クラス:
* ドキュメントを格納するウィンドウを表します。
* ウィンドウ作成時はドキュメントを作成します。
* ウィンドウ破棄時はドキュメントを破棄します。
*/
G_DEFINE_FINAL_TYPE (DrawDocumentWindow, draw_document_window, GTK_TYPE_APPLICATION_WINDOW);

/* メニュー アクション */
static const GActionEntry
ACTION_ENTRIES [] =
{
	{ "show-about", draw_document_window_activate_about, NULL, NULL, NULL },
};

/*******************************************************************************
* @brief バージョン情報ダイアログを表示します。
*/
static void
draw_document_window_activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	share_about_dialog_show (GTK_WINDOW (user_data), TITLE, LOGO_ICON_NAME);
}

/*******************************************************************************
* @brief クラスを初期化します。
*/
static void
draw_document_window_class_init (DrawDocumentWindowClass *this_class)
{
	draw_document_window_class_init_object (G_OBJECT_CLASS (this_class));
	draw_document_window_class_init_widget (GTK_WIDGET_CLASS (this_class));
}

/*******************************************************************************
* @brief Object クラスを初期化します。
*/
static void
draw_document_window_class_init_object (GObjectClass *this_class)
{
	this_class->constructed = draw_document_window_constructed;
	this_class->dispose = draw_document_window_dispose;
	this_class->get_property = draw_document_window_get_property;
	this_class->set_property = draw_document_window_set_property;
}

/*******************************************************************************
* @brief Widget クラスを初期化します。
*/
static void
draw_document_window_class_init_widget (GtkWidgetClass *this_class)
{
	this_class->realize = draw_document_window_realize;
	this_class->size_allocate = draw_document_window_size_allocate;
	this_class->unrealize = draw_document_window_unrealize;
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
draw_document_window_constructed (GObject *self)
{
	draw_document_window_init_settings (DRAW_DOCUMENT_WINDOW (self));
	G_OBJECT_CLASS (draw_document_window_parent_class)->constructed (self);
}

/*******************************************************************************
* @brief クラスのインスタンスを破棄します。
*/
static void
draw_document_window_dispose (GObject *self)
{
	draw_document_window_settings_save (DRAW_DOCUMENT_WINDOW (self));
	G_OBJECT_CLASS (draw_document_window_parent_class)->dispose (self);
}

/*******************************************************************************
* @brief プロパティを取得します。
*/
static void
draw_document_window_get_property (GObject *self, guint property_id, GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
draw_document_window_init (DrawDocumentWindow *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
	gtk_window_set_title (GTK_WINDOW (self), TITLE);
}

/*******************************************************************************
* @brief 環境設定を適用します。
*/
static void
draw_document_window_init_settings (DrawDocumentWindow *self)
{
	draw_document_window_settings_load (self);
	draw_document_window_settings_apply (self);
}

/*******************************************************************************
* @brief クラスのインスタンスを作成します。
*/
GtkWidget *
draw_document_window_new (GApplication *application)
{
	return g_object_new (DRAW_TYPE_DOCUMENT_WINDOW, PROPERTY_APPLICATION, application, PROPERTY_SHOW_MENUBAR, TRUE, NULL);
}

/*******************************************************************************
* @brief ウィンドウを表示します。
*/
static void
draw_document_window_realize (GtkWidget *self)
{
	GTK_WIDGET_CLASS (draw_document_window_parent_class)->realize (self);
	draw_document_window_surface_connect (self);
}

/*******************************************************************************
* @brief プロパティを設定します。
*/
static void
draw_document_window_set_property (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
}

/*******************************************************************************
* @brief 環境設定を適用します。
*/
static void
draw_document_window_settings_apply (DrawDocumentWindow *self)
{
	GtkWindow *window;
	window = GTK_WINDOW (self);
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
draw_document_window_settings_load (DrawDocumentWindow *self)
{
	GSettings *settings;
	settings = draw_get_settings ();
	self->width = g_settings_get_int (settings, SETTINGS_WIDTH);
	self->height = g_settings_get_int (settings, SETTINGS_HEIGHT);
	self->maximized = g_settings_get_boolean (settings, SETTINGS_MAXIMIZED);
	g_object_unref (settings);
}

/*******************************************************************************
* @brief 環境設定を書き込みます。
*/
static void
draw_document_window_settings_save (DrawDocumentWindow *self)
{
	GSettings *settings;
	settings = draw_get_settings ();
	g_settings_set_int (settings, SETTINGS_WIDTH, self->width);
	g_settings_set_int (settings, SETTINGS_HEIGHT, self->height);
	g_settings_set_boolean (settings, SETTINGS_MAXIMIZED, self->maximized);
	g_object_unref (settings);
}

/*******************************************************************************
* @brief ウィンドウの大きさを更新します。
*/
static void
draw_document_window_size_allocate (GtkWidget *self, int width, int height, int baseline)
{
	GTK_WIDGET_CLASS (draw_document_window_parent_class)->size_allocate (self, width, height, baseline);
	draw_document_window_update_size (DRAW_DOCUMENT_WINDOW (self));
}

/*******************************************************************************
* @brief ウィンドウの大きさを更新します。
*/
static void
draw_document_window_surface_changed (GdkSurface *surface, GParamSpec *pspec, gpointer user_data)
{
	DrawDocumentWindow *self;
	GdkToplevelState state;
	state = gdk_toplevel_get_state (GDK_TOPLEVEL (surface));
	self = DRAW_DOCUMENT_WINDOW (user_data);
	self->maximized = (state & GDK_TOPLEVEL_STATE_MAXIMIZED) != 0;
}

/*******************************************************************************
* @brief サーフィス通知を購読します。
*/
static void
draw_document_window_surface_connect (GtkWidget *self)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (GTK_NATIVE (self));
	g_signal_connect (surface, SIGNAL_NOTIFY_STATE, G_CALLBACK (draw_document_window_surface_changed), self);
}

/*******************************************************************************
* @brief サーフィス通知を解約します。
*/
static void
draw_document_window_surface_disconnect (GtkWidget *self)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (GTK_NATIVE (self));
	g_signal_handlers_disconnect_by_func (surface, draw_document_window_surface_changed, self);
}

/*******************************************************************************
* @brief ウィンドウを隠蔽します。
*/
static void
draw_document_window_unrealize (GtkWidget *self)
{
	draw_document_window_surface_disconnect (self);
	GTK_WIDGET_CLASS (draw_document_window_parent_class)->unrealize (self);
}

/*******************************************************************************
* @brief ウィンドウの大きさを更新します。
*/
static void
draw_document_window_update_size (DrawDocumentWindow *self)
{
	if (!self->maximized)
	{
		gtk_window_get_default_size (GTK_WINDOW (self), &self->width, &self->height);
	}
}
