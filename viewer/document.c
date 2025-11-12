/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#include "share.h"
#define LOGO_ICON_NAME        "viewer"
#define PROPERTY_APPLICATION  "application"
#define PROPERTY_SHOW_MENUBAR "show-menubar"
#define SETTINGS_HEIGHT       "window-height"
#define SETTINGS_MAXIMIZED    "window-maximized"
#define SETTINGS_WIDTH        "window-width"
#define SIGNAL_NOTIFY_STATE   "notify::state"
#define TITLE                 _("Picture Viewer")
#define TITLE_CCH             256
#define TITLE_FORMAT          "%s - %s"

/* クラスのインスタンス */
struct _ViewerDocumentWindow
{
	GtkApplicationWindow parent_instance;
	GFile               *file;
	GtkWidget           *canvas;
	int                  width;
	int                  height;
	int                  maximized;
};

static void     viewer_document_window_activate_about     (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_document_window_activate_open      (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_document_window_change_surface     (GdkSurface *surface, GParamSpec *pspec, gpointer user_data);
static void     viewer_document_window_class_init         (ViewerDocumentWindowClass *this_class);
static void     viewer_document_window_class_init_object  (GObjectClass *this_class);
static void     viewer_document_window_class_init_widget  (GtkWidgetClass *this_class);
static void     viewer_document_window_clear_file         (ViewerDocumentWindow *self);
static void     viewer_document_window_connect_surface    (GtkWidget *self);
static void     viewer_document_window_constructed        (GObject *self);
static void     viewer_document_window_destroy            (ViewerDocumentWindow *self);
static void     viewer_document_window_disconnect_surface (GtkWidget *self);
static void     viewer_document_window_dispose            (GObject *self);
static void     viewer_document_window_get_property       (GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static void     viewer_document_window_init               (ViewerDocumentWindow *self);
static void     viewer_document_window_init_canvas        (ViewerDocumentWindow *self);
static void     viewer_document_window_init_settings      (ViewerDocumentWindow *self);
static gboolean viewer_document_window_load               (ViewerDocumentWindow *self, GFile *file);
static void     viewer_document_window_load_settings      (ViewerDocumentWindow *self);
static void     viewer_document_window_realize            (GtkWidget *self);
static void     viewer_document_window_respond_open       (GObject *dialog, GAsyncResult *result, gpointer user_data);
static void     viewer_document_window_save_settings      (ViewerDocumentWindow *self);
static void     viewer_document_window_set_property       (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);
static void     viewer_document_window_size_allocate      (GtkWidget *self, int width, int height, int baseline);
static void     viewer_document_window_unrealize          (GtkWidget *self);
static void     viewer_document_window_update_size        (ViewerDocumentWindow *self);
static void     viewer_document_window_update_title       (ViewerDocumentWindow *self);

/*******************************************************************************
* Viewer Document Window クラス。
* ドキュメントを格納するウィンドウを表します。
* ウィンドウ作成時はドキュメントを作成します。
* ウィンドウ破棄時はドキュメントを破棄します。
*/
G_DEFINE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, GTK_TYPE_APPLICATION_WINDOW);

/* メニュー アクション */
static const GActionEntry
ACTION_ENTRIES [] =
{
	{ "show-about", viewer_document_window_activate_about, NULL, NULL, NULL },
	{ "open",       viewer_document_window_activate_open,  NULL, NULL, NULL },
};

/*******************************************************************************
* @brief バージョン情報ダイアログを表示します。
*/
static void
viewer_document_window_activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	about (GTK_WINDOW (user_data), TITLE, LOGO_ICON_NAME);
}

/*******************************************************************************
* @brief ファイルを開くダイアログを表示します。
*/
static void
viewer_document_window_activate_open (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	viewer_choose_file (GTK_WINDOW (user_data), NULL, viewer_document_window_respond_open, user_data);
}

/*******************************************************************************
* @brief ウィンドウの大きさを更新します。
*/
static void
viewer_document_window_change_surface (GdkSurface *surface, GParamSpec *pspec, gpointer user_data)
{
	ViewerDocumentWindow *self;
	GdkToplevelState state;
	state = gdk_toplevel_get_state (GDK_TOPLEVEL (surface));
	self = VIEWER_DOCUMENT_WINDOW (user_data);
	self->maximized = (state & GDK_TOPLEVEL_STATE_MAXIMIZED) != 0;
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
* @brief ファイルを破棄します。
*/
static void
viewer_document_window_clear_file (ViewerDocumentWindow *self)
{
	g_clear_object (&self->file);
}

/*******************************************************************************
* @brief サーフィス通知を購読します。
*/
static void
viewer_document_window_connect_surface (GtkWidget *self)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (GTK_NATIVE (self));
	g_signal_connect (surface, SIGNAL_NOTIFY_STATE, G_CALLBACK (viewer_document_window_change_surface), self);
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
	viewer_document_window_clear_file (self);
	viewer_document_window_save_settings (self);
}

/*******************************************************************************
* @brief サーフィス通知を解約します。
*/
static void
viewer_document_window_disconnect_surface (GtkWidget *self)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (GTK_NATIVE (self));
	g_signal_handlers_disconnect_by_func (surface, viewer_document_window_change_surface, self);
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
* @brief ファイルを取得します。
*/
GFile *
viewer_document_window_get_file (ViewerDocumentWindow *self)
{
	return OBJECT_REF (self->file);
}

/*******************************************************************************
* @brief プロパティを取得します。
*/
static void
viewer_document_window_get_property (GObject *self, guint property_id, GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
}

/*******************************************************************************
* @brief クラスのインスタンスを初期化します。
*/
static void
viewer_document_window_init (ViewerDocumentWindow *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
	viewer_document_window_init_canvas (self);
	viewer_document_window_update_title (self);
}

/*******************************************************************************
* @brief キャンバスを作成します。
*/
static void
viewer_document_window_init_canvas (ViewerDocumentWindow *self)
{
	self->canvas = viewer_canvas_new ();
	gtk_widget_set_hexpand (self->canvas, TRUE);
	gtk_widget_set_vexpand (self->canvas, TRUE);
	gtk_window_set_child (GTK_WINDOW (self), self->canvas);
}

/*******************************************************************************
* @brief 環境設定を適用します。
*/
static void
viewer_document_window_init_settings (ViewerDocumentWindow *self)
{
	GtkWindow *window;
	window = GTK_WINDOW (self);
	viewer_document_window_load_settings (self);
	gtk_window_set_default_size (window, self->width, self->height);

	if (self->maximized)
	{
		gtk_window_maximize (window);
	}
}

static gboolean
viewer_document_window_load (ViewerDocumentWindow *self, GFile *file)
{
	GError *error;
	GdkPixbuf *pixbuf;
	gboolean result;
	error = NULL;
	pixbuf = pixload (file, &error);

	if (pixbuf)
	{
		viewer_canvas_set_pixbuf (VIEWER_CANVAS (self->canvas), pixbuf);
		g_object_unref (pixbuf);
		result = TRUE;
	}
	else
	{
		result = FALSE;
	}
	if (error)
	{
		alert (GTK_WINDOW (self), error);
		g_clear_error (&error);
	}

	return result;
}

/*******************************************************************************
* @brief 環境設定を読み込みます。
*/
static void
viewer_document_window_load_settings (ViewerDocumentWindow *self)
{
	GSettings *settings;
	settings = viewer_get_settings ();
	self->width = g_settings_get_int (settings, SETTINGS_WIDTH);
	self->height = g_settings_get_int (settings, SETTINGS_HEIGHT);
	self->maximized = g_settings_get_boolean (settings, SETTINGS_MAXIMIZED);
	g_object_unref (settings);
}

/*******************************************************************************
* @brief クラスのインスタンスを作成します。
*/
GtkWidget *
viewer_document_window_new (GApplication *application)
{
	return g_object_new (VIEWER_TYPE_DOCUMENT_WINDOW,
		PROPERTY_APPLICATION, application,
		PROPERTY_SHOW_MENUBAR, TRUE,
		NULL);
}

/*******************************************************************************
* @brief ウィンドウを表示します。
*/
static void
viewer_document_window_realize (GtkWidget *self)
{
	GTK_WIDGET_CLASS (viewer_document_window_parent_class)->realize (self);
	viewer_document_window_connect_surface (self);
}

/*******************************************************************************
* @brief 指定したファイルを開きます。
*/
static void
viewer_document_window_respond_open (GObject *dialog, GAsyncResult *result, gpointer user_data)
{
	GFile *file;
	ViewerDocumentWindow *self;
	file = gtk_file_dialog_open_finish (GTK_FILE_DIALOG (dialog), result, NULL);

	if (file)
	{
		self = VIEWER_DOCUMENT_WINDOW (user_data);

		if (viewer_document_window_load (self, file))
		{
			viewer_document_window_set_file (self, file);
		}

		g_object_unref (file);
	}
}

/*******************************************************************************
* @brief 環境設定を書き込みます。
*/
static void
viewer_document_window_save_settings (ViewerDocumentWindow *self)
{
	GSettings *settings;
	settings = viewer_get_settings ();
	g_settings_set_int (settings, SETTINGS_WIDTH, self->width);
	g_settings_set_int (settings, SETTINGS_HEIGHT, self->height);
	g_settings_set_boolean (settings, SETTINGS_MAXIMIZED, self->maximized);
	g_object_unref (settings);
}

/*******************************************************************************
* @brief ファイルを設定します。
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
		if (file)
		{
			self->file = g_object_ref (file);
		}
		else
		{
			self->file = NULL;
		}

		viewer_document_window_update_title (self);
	}
}

/*******************************************************************************
* @brief プロパティを設定します。
*/
static void
viewer_document_window_set_property (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
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
* @brief ウィンドウを隠蔽します。
*/
static void
viewer_document_window_unrealize (GtkWidget *self)
{
	viewer_document_window_disconnect_surface (self);
	GTK_WIDGET_CLASS (viewer_document_window_parent_class)->unrealize (self);
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
		g_snprintf (title, TITLE_CCH, TITLE_FORMAT, name, TITLE);
		gtk_window_set_title (GTK_WINDOW (self), title);
		g_free (name);
	}
	else
	{
		gtk_window_set_title (GTK_WINDOW (self), TITLE);
	}
}