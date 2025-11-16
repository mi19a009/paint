/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#include "share.h"
#define ACTION_FULLSCREEN           "fullscreen"
#define FILE_PROPERTY_NAME          "file"
#define FILE_PROPERTY_NICK          "Picture File"
#define FILE_PROPERTY_BLURB         "Picture File"
#define FILE_PROPERTY_OBJECT_TYPE   G_TYPE_FILE
#define FILE_PROPERTY_FLAGS         G_PARAM_READWRITE
#define LOGO_ICON_NAME              "viewer"
#define PIXBUF_PROPERTY_NAME        "pixbuf"
#define PIXBUF_PROPERTY_NICK        "Picture Pixbuf"
#define PIXBUF_PROPERTY_BLURB       "Picture Pixbuf"
#define PIXBUF_PROPERTY_OBJECT_TYPE GDK_TYPE_PIXBUF
#define PIXBUF_PROPERTY_FLAGS       G_PARAM_READWRITE
#define PROPERTY_APPLICATION        "application"
#define PROPERTY_SHOW_MENUBAR       "show-menubar"
#define SETTINGS_HEIGHT             "window-height"
#define SETTINGS_MAXIMIZED          "window-maximized"
#define SETTINGS_WIDTH              "window-width"
#define SIGNAL_NOTIFY_STATE         "notify::state"
#define TITLE                       _("Picture Viewer")
#define TITLE_CCH                   256
#define TITLE_FORMAT                "%s - %s"
#define TITLE_FORMAT_ZOOM           "%d%% %s - %s"
#define ZOOM_PROPERTY_NAME          "zoom"
#define ZOOM_PROPERTY_NICK          "Zoom"
#define ZOOM_PROPERTY_BLURB         "Zoom"
#define ZOOM_PROPERTY_MINIMUM       5
#define ZOOM_PROPERTY_MAXIMUM       2000
#define ZOOM_PROPERTY_DEFAULT_VALUE 100
#define ZOOM_PROPERTY_FLAGS         G_PARAM_READWRITE
#define ZOOM_RATIO                  1.25F

/* クラスのプロパティ */
enum _ViewerDocumentWindowProperties
{
	NULL_PROPERTY_ID,
	FILE_PROPERTY_ID,
	PIXBUF_PROPERTY_ID,
	ZOOM_PROPERTY_ID,
};

/* クラスのインスタンス */
struct _ViewerDocumentWindow
{
	GtkApplicationWindow parent_instance;
	GFile               *file;
	GdkPixbuf           *pixbuf;
	GtkWidget           *area;
	int                  zoom;
	int                  width;
	int                  height;
	unsigned char        maximized;
	unsigned char        fullscreen;
};

static void viewer_document_window_activate_about        (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_document_window_activate_fullscreen   (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_document_window_activate_open         (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_document_window_activate_print        (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_document_window_activate_restore_zoom (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_document_window_activate_unfullscreen (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_document_window_activate_zoom_in      (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_document_window_activate_zoom_out     (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_document_window_check_action          (GActionMap *self, const char *name, gboolean checked);
static void viewer_document_window_class_init            (ViewerDocumentWindowClass *this_class);
static void viewer_document_window_class_init_object     (GObjectClass *this_class);
static void viewer_document_window_class_init_widget     (GtkWidgetClass *this_class);
static void viewer_document_window_constructed           (GObject *self);
static void viewer_document_window_destroy               (ViewerDocumentWindow *self);
static void viewer_document_window_dispose               (GObject *self);
static void viewer_document_window_draw                  (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data);
static void viewer_document_window_enable_actions        (GActionMap *self, gboolean enabled);
static void viewer_document_window_get_property          (GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static void viewer_document_window_init                  (ViewerDocumentWindow *self);
static void viewer_document_window_init_actions          (GActionMap *self);
static void viewer_document_window_init_area             (ViewerDocumentWindow *self);
static void viewer_document_window_init_content          (ViewerDocumentWindow *self);
static void viewer_document_window_init_settings         (ViewerDocumentWindow *self);
static void viewer_document_window_realize               (GtkWidget *self);
static void viewer_document_window_respond_open          (GObject *dialog, GAsyncResult *result, gpointer user_data);
static void viewer_document_window_set_fullscreen        (ViewerDocumentWindow *self, gboolean fullscreen);
static void viewer_document_window_set_maximized         (ViewerDocumentWindow *self, gboolean maximized);
static void viewer_document_window_set_property          (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);
static void viewer_document_window_settings_apply        (ViewerDocumentWindow *self);
static void viewer_document_window_settings_load         (ViewerDocumentWindow *self);
static void viewer_document_window_settings_save         (ViewerDocumentWindow *self);
static void viewer_document_window_size_allocate         (GtkWidget *self, int width, int height, int baseline);
static void viewer_document_window_surface_changed       (GdkSurface *surface, GParamSpec *pspec, gpointer user_data);
static void viewer_document_window_surface_connect       (GtkWidget *self);
static void viewer_document_window_surface_disconnect    (GtkWidget *self);
static void viewer_document_window_unrealize             (GtkWidget *self);
static void viewer_document_window_update_area           (ViewerDocumentWindow *self);
static void viewer_document_window_update_size           (ViewerDocumentWindow *self);
static void viewer_document_window_update_title          (ViewerDocumentWindow *self);

/*******************************************************************************
* Viewer Document Window クラス:
* ドキュメントを格納するウィンドウを表します。
* ウィンドウ作成時はドキュメントを作成します。
* ウィンドウ破棄時はドキュメントを破棄します。
*/
G_DEFINE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, GTK_TYPE_APPLICATION_WINDOW);
static const char *DISABLED_ACTIONS [] = { "print", "restore-zoom", "zoom-in", "zoom-out" };

/* メニュー アクション */
static const GActionEntry
ACTION_ENTRIES [] =
{
	{ "show-about",   viewer_document_window_activate_about,        NULL, NULL,    NULL },
	{ "fullscreen",   viewer_document_window_activate_fullscreen,   NULL, "false", NULL },
	{ "open",         viewer_document_window_activate_open,         NULL, NULL,    NULL },
	{ "print",        viewer_document_window_activate_print,        NULL, NULL,    NULL },
	{ "restore-zoom", viewer_document_window_activate_restore_zoom, NULL, NULL,    NULL },
	{ "unfullscreen", viewer_document_window_activate_unfullscreen, NULL, NULL,    NULL },
	{ "zoom-in",      viewer_document_window_activate_zoom_in,      NULL, NULL,    NULL },
	{ "zoom-out",     viewer_document_window_activate_zoom_out,     NULL, NULL,    NULL },
};

/*******************************************************************************
* @brief バージョン情報ダイアログを表示します。
*/
static void
viewer_document_window_activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	share_about_dialog_show (GTK_WINDOW (user_data), TITLE, LOGO_ICON_NAME);
}

/*******************************************************************************
* @brief 全画面表示します。
*/
static void
viewer_document_window_activate_fullscreen (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	ViewerDocumentWindow *self;
	self = VIEWER_DOCUMENT_WINDOW (user_data);

	if (self->fullscreen)
	{
		gtk_window_unfullscreen (GTK_WINDOW (self));
	}
	else
	{
		gtk_window_fullscreen (GTK_WINDOW (self));
	}
}

/*******************************************************************************
* @brief ファイルを開くダイアログを表示します。
*/
static void
viewer_document_window_activate_open (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	ViewerDocumentWindow *self;
	self = VIEWER_DOCUMENT_WINDOW (user_data);
	share_file_dialog_open (GTK_WINDOW (user_data), self->file, viewer_document_window_respond_open, user_data, SHARE_FILE_FILTER_ALL | SHARE_FILE_FILTER_IMAGE);
}

/*******************************************************************************
* @brief 印刷ダイアログを表示します。
*/
static void
viewer_document_window_activate_print (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	ViewerDocumentWindow *self;
	GError *error;
	self = VIEWER_DOCUMENT_WINDOW (user_data);

	if (self->pixbuf)
	{
		error = NULL;
		share_print_operation_run (GTK_WINDOW (self), self->pixbuf, &error);

		if (error)
		{
			share_alert_dialog_show (GTK_WINDOW (self), error);
			g_error_free (error);
		}
	}
}

/*******************************************************************************
* @brief 既定の拡大率に戻します。
*/
static void
viewer_document_window_activate_restore_zoom (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	viewer_document_window_set_zoom (VIEWER_DOCUMENT_WINDOW (user_data), ZOOM_PROPERTY_DEFAULT_VALUE);
}

/*******************************************************************************
* @brief ウィンドウ表示します。
*/
static void
viewer_document_window_activate_unfullscreen (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	gtk_window_unfullscreen (GTK_WINDOW (user_data));
}

/*******************************************************************************
* @brief 詳細表示します。
*/
static void
viewer_document_window_activate_zoom_in (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	ViewerDocumentWindow *self;
	float zoom;
	self = VIEWER_DOCUMENT_WINDOW (user_data);
	zoom = viewer_document_window_get_zoom_percent (self) * ZOOM_RATIO;
	viewer_document_window_set_zoom_percent (self, zoom);
}

/*******************************************************************************
* @brief 広域表示します。
*/
static void
viewer_document_window_activate_zoom_out (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	ViewerDocumentWindow *self;
	float zoom;
	self = VIEWER_DOCUMENT_WINDOW (user_data);
	zoom = viewer_document_window_get_zoom_percent (self) / ZOOM_RATIO;
	viewer_document_window_set_zoom_percent (self, zoom);
}

/*******************************************************************************
* @brief 指定したメニュー項目にチェックを付けます。
*/
static void
viewer_document_window_check_action (GActionMap *self, const char *name, gboolean checked)
{
	GAction *action;
	action = g_action_map_lookup_action (self, name);

	if (G_IS_SIMPLE_ACTION (action))
	{
		g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (checked));
	}
}

/*******************************************************************************
* @brief クラスを初期化します。
*/
static void
viewer_document_window_class_init (ViewerDocumentWindowClass *this_class)
{
	viewer_document_window_class_init_object (G_OBJECT_CLASS (this_class));
	viewer_document_window_class_init_widget (GTK_WIDGET_CLASS (this_class));
}

/*******************************************************************************
* @brief Object クラスを初期化します。
*/
static void
viewer_document_window_class_init_object (GObjectClass *this_class)
{
	this_class->constructed = viewer_document_window_constructed;
	this_class->dispose = viewer_document_window_dispose;
	this_class->get_property = viewer_document_window_get_property;
	this_class->set_property = viewer_document_window_set_property;
	OBJECT_CLASS_INSTALL_PROPERTY_OBJECT (this_class, FILE_PROPERTY);
	OBJECT_CLASS_INSTALL_PROPERTY_OBJECT (this_class, PIXBUF_PROPERTY);
	OBJECT_CLASS_INSTALL_PROPERTY_INT (this_class, ZOOM_PROPERTY);
}

/*******************************************************************************
* @brief Widget クラスを初期化します。
*/
static void
viewer_document_window_class_init_widget (GtkWidgetClass *this_class)
{
	this_class->realize = viewer_document_window_realize;
	this_class->size_allocate = viewer_document_window_size_allocate;
	this_class->unrealize = viewer_document_window_unrealize;
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
viewer_document_window_constructed (GObject *self)
{
	viewer_document_window_init_settings (VIEWER_DOCUMENT_WINDOW (self));
	G_OBJECT_CLASS (viewer_document_window_parent_class)->constructed (self);
}

/*******************************************************************************
* @brief プロパティを破棄します。
*/
static void
viewer_document_window_destroy (ViewerDocumentWindow *self)
{
	g_clear_object (&self->file);
	g_clear_object (&self->pixbuf);
	viewer_document_window_settings_save (self);
}

/*******************************************************************************
* @brief クラスのインスタンスを破棄します。
*/
static void
viewer_document_window_dispose (GObject *self)
{
	viewer_document_window_destroy (VIEWER_DOCUMENT_WINDOW (self));
	G_OBJECT_CLASS (viewer_document_window_parent_class)->dispose (self);
}

/*******************************************************************************
* @brief 領域を描画します。
*/
static void
viewer_document_window_draw (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data)
{
	ViewerDocumentWindow *self;
	double zoom;
	self = VIEWER_DOCUMENT_WINDOW (user_data);

	if (self->pixbuf)
	{
		if (self->zoom != ZOOM_PROPERTY_DEFAULT_VALUE)
		{
			zoom = viewer_document_window_get_zoom_percent (self);
			cairo_scale (cairo, zoom, zoom);
		}

		gdk_cairo_set_source_pixbuf (cairo, self->pixbuf, 0, 0);
		cairo_paint (cairo);
	}
}

/*******************************************************************************
* @brief アクションを有効化します。
*/
static void
viewer_document_window_enable_actions (GActionMap *self, gboolean enabled)
{
	GAction *action;
	int n;

	for (n = 0; n < G_N_ELEMENTS (DISABLED_ACTIONS); n++)
	{
		action = g_action_map_lookup_action (self, DISABLED_ACTIONS [n]);

		if (G_IS_SIMPLE_ACTION (action))
		{
			g_simple_action_set_enabled (G_SIMPLE_ACTION (action), enabled);
		}
	}
}

/*******************************************************************************
* @brief 画像ファイルを取得します。
*/
GFile *
viewer_document_window_get_file (ViewerDocumentWindow *self)
{
	GFile *file;
	file = self->file;
	return file ? g_object_ref (file) : NULL;
}

/*******************************************************************************
* @brief 画像を取得します。
*/
GdkPixbuf *
viewer_document_window_get_pixbuf (ViewerDocumentWindow *self)
{
	GdkPixbuf *pixbuf;
	pixbuf = self->pixbuf;
	return pixbuf ? g_object_ref (pixbuf) : NULL;
}

/*******************************************************************************
* @brief プロパティを取得します。
*/
static void
viewer_document_window_get_property (GObject *self, guint property_id, GValue *value, GParamSpec *pspec)
{
	ViewerDocumentWindow *properties;
	properties = VIEWER_DOCUMENT_WINDOW (self);

	switch (property_id)
	{
	case FILE_PROPERTY_ID:
		g_value_set_object (value, properties->file);
		break;
	case PIXBUF_PROPERTY_ID:
		g_value_set_object (value, properties->pixbuf);
		break;
	case ZOOM_PROPERTY_ID:
		g_value_set_int (value, properties->zoom);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

/*******************************************************************************
* @brief 拡大率を取得します。
*/
int
viewer_document_window_get_zoom (ViewerDocumentWindow *self)
{
	return self->zoom;
}

/*******************************************************************************
* @brief 拡大率を取得します。
*/
float
viewer_document_window_get_zoom_percent (ViewerDocumentWindow *self)
{
	return self->zoom / (float) ZOOM_PROPERTY_DEFAULT_VALUE;
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
viewer_document_window_init (ViewerDocumentWindow *self)
{
	self->zoom = ZOOM_PROPERTY_DEFAULT_VALUE;
	viewer_document_window_init_actions (G_ACTION_MAP (self));
	viewer_document_window_init_content (self);
	viewer_document_window_init_area (self);
	viewer_document_window_update_title (self);
	viewer_document_window_update_area (self);
}

/*******************************************************************************
* @brief アクションを追加します。
*/
static void
viewer_document_window_init_actions (GActionMap *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
	viewer_document_window_enable_actions (self, FALSE);
}

/*******************************************************************************
* @brief 描画領域を作成します。
*/
static void
viewer_document_window_init_area (ViewerDocumentWindow *self)
{
	GtkWidget *parent, *widget;
	parent = gtk_window_get_child (GTK_WINDOW (self));
	widget = gtk_drawing_area_new ();
	self->area = widget;
	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (widget), viewer_document_window_draw, self, NULL);
	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (parent), widget);
}

/*******************************************************************************
* @brief 内容を作成します。
*/
static void
viewer_document_window_init_content (ViewerDocumentWindow *self)
{
	GtkWidget *widget;
	widget = gtk_scrolled_window_new ();
	gtk_widget_set_hexpand (widget, TRUE);
	gtk_widget_set_vexpand (widget, TRUE);
	gtk_window_set_child (GTK_WINDOW (self), widget);
}

/*******************************************************************************
* @brief 環境設定を適用します。
*/
static void
viewer_document_window_init_settings (ViewerDocumentWindow *self)
{
	viewer_document_window_settings_load (self);
	viewer_document_window_settings_apply (self);
}

/*******************************************************************************
* @brief クラスのインスタンスを作成します。
*/
GtkWidget *
viewer_document_window_new (GApplication *application)
{
	return g_object_new (VIEWER_TYPE_DOCUMENT_WINDOW, PROPERTY_APPLICATION, application, PROPERTY_SHOW_MENUBAR, TRUE, NULL);
}

/*******************************************************************************
* @brief ウィンドウを表示します。
*/
static void
viewer_document_window_realize (GtkWidget *self)
{
	GTK_WIDGET_CLASS (viewer_document_window_parent_class)->realize (self);
	viewer_document_window_surface_connect (self);
}

/*******************************************************************************
* @brief 指定したファイルを開きます。
*/
static void
viewer_document_window_respond_open (GObject *dialog, GAsyncResult *result, gpointer user_data)
{
	GError *error;
	GFile *file;
	GdkPixbuf *pixbuf;
	ViewerDocumentWindow *self;
	file = gtk_file_dialog_open_finish (GTK_FILE_DIALOG (dialog), result, NULL);

	if (file)
	{
		error = NULL;
		self = VIEWER_DOCUMENT_WINDOW (user_data);
		pixbuf = share_pixbuf_create_from_file (file, &error);
		viewer_document_window_set_file (self, pixbuf ? file : NULL);
		viewer_document_window_set_pixbuf (self, pixbuf);
		g_object_unref (file);

		if (pixbuf)
		{
			g_object_unref (pixbuf);
		}
		if (error)
		{
			share_alert_dialog_show (GTK_WINDOW (self), error);
			g_error_free (error);
		}
	}
}

/*******************************************************************************
* @brief 画像ファイルを設定します。
* ウィンドウ タイトルを更新します。
*/
void
viewer_document_window_set_file (ViewerDocumentWindow *self, GFile *file)
{
	if (self->file != file)
	{
		if (self->file)
		{
			g_object_unref (self->file);
		}

		self->file = file ? g_object_ref (file) : NULL;
		viewer_document_window_update_title (self);
	}
}

/*******************************************************************************
* @brief 全画面表示を設定します。
*/
static void
viewer_document_window_set_fullscreen (ViewerDocumentWindow *self, gboolean fullscreen)
{
	fullscreen = fullscreen != 0;

	if (self->fullscreen != fullscreen)
	{
		self->fullscreen = fullscreen;
		viewer_document_window_check_action (G_ACTION_MAP (self), ACTION_FULLSCREEN, fullscreen);
	}
}

/*******************************************************************************
* @brief 最大化表示を設定します。
*/
static void
viewer_document_window_set_maximized (ViewerDocumentWindow *self, gboolean maximized)
{
	self->maximized = maximized != 0;
}

/*******************************************************************************
* @brief 画像を設定します。
* 描画領域を更新します。
*/
void
viewer_document_window_set_pixbuf (ViewerDocumentWindow *self, GdkPixbuf *pixbuf)
{
	gboolean enabled;

	if (self->pixbuf != pixbuf)
	{
		if (self->pixbuf)
		{
			g_object_unref (self->pixbuf);
		}
		if (pixbuf)
		{
			self->pixbuf = g_object_ref (pixbuf);
			enabled = TRUE;
		}
		else
		{
			self->pixbuf = NULL;
			enabled = FALSE;
		}

		viewer_document_window_enable_actions (G_ACTION_MAP (self), enabled);
		viewer_document_window_update_area (self);
		gtk_widget_queue_draw (self->area);
	}
}

/*******************************************************************************
* @brief プロパティを設定します。
*/
static void
viewer_document_window_set_property (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec)
{
	ViewerDocumentWindow *properties;
	properties = VIEWER_DOCUMENT_WINDOW (self);

	switch (property_id)
	{
	case FILE_PROPERTY_ID:
		viewer_document_window_set_file (properties, g_value_get_object (value));
		break;
	case PIXBUF_PROPERTY_ID:
		viewer_document_window_set_pixbuf (properties, g_value_get_object (value));
		break;
	case ZOOM_PROPERTY_ID:
		viewer_document_window_set_zoom (properties, g_value_get_int (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

/*******************************************************************************
* @brief 拡大率を設定します。
*/
void
viewer_document_window_set_zoom (ViewerDocumentWindow *self, int zoom)
{
	zoom = CLAMP (zoom, ZOOM_PROPERTY_MINIMUM, ZOOM_PROPERTY_MAXIMUM);

	if (self->zoom != zoom)
	{
		self->zoom = zoom;
		viewer_document_window_update_title (self);
		viewer_document_window_update_area (self);
		gtk_widget_queue_draw (self->area);
	}
}

/*******************************************************************************
* @brief 拡大率を設定します。
*/
void
viewer_document_window_set_zoom_percent (ViewerDocumentWindow *self, float zoom)
{
	viewer_document_window_set_zoom (self, lroundf (zoom * ZOOM_PROPERTY_DEFAULT_VALUE));
}

/*******************************************************************************
* @brief 環境設定を適用します。
*/
static void
viewer_document_window_settings_apply (ViewerDocumentWindow *self)
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
viewer_document_window_settings_load (ViewerDocumentWindow *self)
{
	GSettings *settings;
	settings = viewer_get_settings ();
	self->width = g_settings_get_int (settings, SETTINGS_WIDTH);
	self->height = g_settings_get_int (settings, SETTINGS_HEIGHT);
	self->maximized = g_settings_get_boolean (settings, SETTINGS_MAXIMIZED);
	g_object_unref (settings);
}

/*******************************************************************************
* @brief 環境設定を書き込みます。
*/
static void
viewer_document_window_settings_save (ViewerDocumentWindow *self)
{
	GSettings *settings;
	settings = viewer_get_settings ();
	g_settings_set_int (settings, SETTINGS_WIDTH, self->width);
	g_settings_set_int (settings, SETTINGS_HEIGHT, self->height);
	g_settings_set_boolean (settings, SETTINGS_MAXIMIZED, self->maximized);
	g_object_unref (settings);
}

/*******************************************************************************
* @brief ウィンドウの大きさを更新します。
*/
static void
viewer_document_window_size_allocate (GtkWidget *self, int width, int height, int baseline)
{
	GTK_WIDGET_CLASS (viewer_document_window_parent_class)->size_allocate (self, width, height, baseline);
	viewer_document_window_update_size (VIEWER_DOCUMENT_WINDOW (self));
}

/*******************************************************************************
* @brief ウィンドウの大きさを更新します。
*/
static void
viewer_document_window_surface_changed (GdkSurface *surface, GParamSpec *pspec, gpointer user_data)
{
	ViewerDocumentWindow *self;
	GdkToplevelState state;
	state = gdk_toplevel_get_state (GDK_TOPLEVEL (surface));
	self = VIEWER_DOCUMENT_WINDOW (user_data);
	viewer_document_window_set_maximized (self, state & GDK_TOPLEVEL_STATE_MAXIMIZED);
	viewer_document_window_set_fullscreen (self, state & GDK_TOPLEVEL_STATE_FULLSCREEN);
}

/*******************************************************************************
* @brief サーフィス通知を購読します。
*/
static void
viewer_document_window_surface_connect (GtkWidget *self)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (GTK_NATIVE (self));
	g_signal_connect (surface, SIGNAL_NOTIFY_STATE, G_CALLBACK (viewer_document_window_surface_changed), self);
}

/*******************************************************************************
* @brief サーフィス通知を解約します。
*/
static void
viewer_document_window_surface_disconnect (GtkWidget *self)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (GTK_NATIVE (self));
	g_signal_handlers_disconnect_by_func (surface, viewer_document_window_surface_changed, self);
}

/*******************************************************************************
* @brief ウィンドウを隠蔽します。
*/
static void
viewer_document_window_unrealize (GtkWidget *self)
{
	viewer_document_window_surface_disconnect (self);
	GTK_WIDGET_CLASS (viewer_document_window_parent_class)->unrealize (self);
}

/*******************************************************************************
* @brief 描画領域の大きさを更新します。
*/
static void
viewer_document_window_update_area (ViewerDocumentWindow *self)
{
	GtkDrawingArea *area;
	int width, height;
	float zoom;

	if (self->pixbuf)
	{
		width = gdk_pixbuf_get_width (self->pixbuf);
		height = gdk_pixbuf_get_height (self->pixbuf);

		if (self->zoom != ZOOM_PROPERTY_DEFAULT_VALUE)
		{
			zoom = viewer_document_window_get_zoom_percent (self);
			width *= zoom;
			height *= zoom;
		}
	}
	else
	{
		width = 0;
		height = 0;
	}

	area = GTK_DRAWING_AREA (self->area);
	gtk_drawing_area_set_content_width (area, width);
	gtk_drawing_area_set_content_height (area, height);
}

/*******************************************************************************
* @brief ウィンドウの大きさを更新します。
*/
static void
viewer_document_window_update_size (ViewerDocumentWindow *self)
{
	if (!self->maximized)
	{
		gtk_window_get_default_size (GTK_WINDOW (self), &self->width, &self->height);
	}
}

/*******************************************************************************
* @brief ウィンドウ タイトルを更新します。
*/
static void
viewer_document_window_update_title (ViewerDocumentWindow *self)
{
	char *name;
	char title [TITLE_CCH];

	if (self->file)
	{
		name = g_file_get_basename (self->file);

		if (self->zoom == ZOOM_PROPERTY_DEFAULT_VALUE)
		{
			g_snprintf (title, TITLE_CCH, TITLE_FORMAT, name, TITLE);
		}
		else
		{
			g_snprintf (title, TITLE_CCH, TITLE_FORMAT_ZOOM, self->zoom, name, TITLE);
		}

		gtk_window_set_title (GTK_WINDOW (self), title);
		g_free (name);
	}
	else
	{
		gtk_window_set_title (GTK_WINDOW (self), TITLE);
	}
}