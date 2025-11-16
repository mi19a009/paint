/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include "share.h"
#define ABOUT_COPYRIGHT         "Copyright © 2025 Taichi Murakami."
#define ABOUT_WEBSITE           "https://mi19a009.github.io/paint/"
#define ALERT_FORMAT            "%s"
#define FILTER_ALL              "*.*"
#define FILTER_IMAGE            "image/*"
#define GETTEXT_CODESET         "UTF-8"
#define GETTEXT_DOMAIN          "msg"
#define GETTEXT_FORMAT          "%s"
#define GETTEXT_LINK            "/proc/self/exe"
#define GETTEXT_LOCALE          ""
#define GETTEXT_PATH            "locale"
#define PRINT_N_PAGES           1
#define PROPERTY_APPLICATION_ID "application-id"
#define PROPERTY_FLAGS          "flags"
#define SIGNAL_BEGIN_PRINT      "begin-print"
#define SIGNAL_DESTROY          "destroy"
#define SIGNAL_DRAW_PAGE        "draw-page"
#define SIGNAL_END_PRINT        "end-print"
#define TITLE_ALL               _("All Files")
#define TITLE_IMAGE             _("Image Files")
#define TITLE_OPEN              _("Open File")
#define TITLE_SAVE              _("Save As")

typedef struct _SharePrintData SharePrintData;
typedef void (*ShareFileFilterAdd) (GtkFileFilter *filter, const char *format);

/* 印刷 */
struct _SharePrintData
{
	GdkPixbuf       *pixbuf;
	cairo_surface_t *surface;
};

static void share_about_dialog_init        (GtkAboutDialog *dialog, const char *title, const char *logo_icon_name);
static void share_application_init         (void);
static void share_file_dialog_init         (GtkFileDialog *dialog, const char *title, GFile *initial_file);
static void share_file_dialog_init_filters (GtkFileDialog *dialog, ShareFileDialogFlag flags);
static void share_file_filters_add         (GListStore *filters, const char *name, const char *format, ShareFileFilterAdd add);
static void share_present                  (GtkWindow *window, GtkWindow *parent);
static void share_print_operation_begin    (GtkPrintOperation *operation, GtkPrintContext *context, SharePrintData *data);
static void share_print_operation_draw     (GtkPrintOperation *operation, GtkPrintContext *context, int page, SharePrintData *data);
static void share_print_operation_end      (GtkPrintOperation *operation, GtkPrintContext *context, SharePrintData *data);
static void share_print_operation_init     (GtkPrintContext *context, SharePrintData *data);
static const char *ABOUT_AUTHORS [] = { "Taichi Murakami", NULL };

/*******************************************************************************
* @brief バージョン情報ダイアログを初期化します。
*/
static void
share_about_dialog_init (GtkAboutDialog *dialog, const char *title, const char *logo_icon_name)
{
	gtk_about_dialog_set_authors (dialog, ABOUT_AUTHORS);
	gtk_about_dialog_set_copyright (dialog, ABOUT_COPYRIGHT);
	gtk_about_dialog_set_license_type (dialog, GTK_LICENSE_APACHE_2_0);
	gtk_about_dialog_set_logo_icon_name (dialog, logo_icon_name);
	gtk_about_dialog_set_program_name (dialog, title);
	gtk_about_dialog_set_website (dialog, ABOUT_WEBSITE);
}

/*******************************************************************************
* @brief バージョン情報ダイアログを表示します。
*/
void
share_about_dialog_show (GtkWindow *parent, const char *title, const char *logo_icon_name)
{
	GtkWidget *dialog;
	dialog = gtk_about_dialog_new ();
	g_signal_connect_swapped (dialog, SIGNAL_DESTROY, G_CALLBACK (gtk_window_destroy), dialog);
	share_about_dialog_init (GTK_ABOUT_DIALOG (dialog), title, logo_icon_name);
	share_present (GTK_WINDOW (dialog), parent);
}

/*******************************************************************************
* @brief 指定したエラーを説明する文字列を表示します。
*/
void
share_alert_dialog_show (GtkWindow *parent, const GError *error)
{
	GtkAlertDialog *dialog;
	dialog = gtk_alert_dialog_new (ALERT_FORMAT, error->message);
	gtk_alert_dialog_show (dialog, parent);
	g_object_unref (dialog);
}

/*******************************************************************************
* @brief メッセージ カタログを初期化します。
*/
static void
share_application_init (void)
{
	gchar *path;
	GError *error;
	GPathBuf buffer;
	error = NULL;
	setlocale (LC_ALL, GETTEXT_LOCALE);
	path = g_file_read_link (GETTEXT_LINK, &error);

	if (path)
	{
		g_path_buf_init_from_path (&buffer, path);
		g_free (path);
		g_path_buf_pop (&buffer);
		g_path_buf_pop (&buffer);
		g_path_buf_push (&buffer, GETTEXT_PATH);
		path = g_path_buf_clear_to_path (&buffer);
		bindtextdomain (GETTEXT_DOMAIN, path);
		g_free (path);
		bind_textdomain_codeset (GETTEXT_DOMAIN, GETTEXT_CODESET);
		textdomain (GETTEXT_DOMAIN);
	}
	if (error)
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_ERROR, GETTEXT_FORMAT, error->message);
		g_error_free (error);
	}
}

/*******************************************************************************
* @brief アプリケーションを作成します。
*/
int
share_application_run (int argc, char **argv, GType type, const char *application_id, GApplicationFlags flags)
{
	GApplication *application;
	int exitcode;
	share_application_init ();
	application = g_object_new (type, PROPERTY_APPLICATION_ID, application_id, PROPERTY_FLAGS, flags, NULL);
	exitcode = g_application_run (application, argc, argv);
	g_object_unref (application);
	return exitcode;
}

/*******************************************************************************
* @brief アクセラレーターを初期化します。
*/
void
share_application_set_accel_entries (GtkApplication *application, const ShareAccelEntry *entries, int n_entries)
{
	int n;

	for (n = 0; n < n_entries; n++)
	{
		gtk_application_set_accels_for_action (application, entries->detailed_action_name, entries->accels);
		entries++;
	}
}

/*******************************************************************************
* @brief ファイル ダイアログを初期化します。
* @param dialog ファイル ダイアログ。
*/
static void
share_file_dialog_init (GtkFileDialog *dialog, const char *title, GFile *initial_file)
{
	gtk_file_dialog_set_initial_file (dialog, initial_file);
	gtk_file_dialog_set_modal (dialog, TRUE);
	gtk_file_dialog_set_title (dialog, title);
}

/*******************************************************************************
* @brief ファイル ダイアログ フィルターを初期化します。
* @param dialog ファイル ダイアログ。
*/
static void
share_file_dialog_init_filters (GtkFileDialog *dialog, ShareFileDialogFlag flags)
{
	GListStore *filters;
	filters = g_list_store_new (GTK_TYPE_FILE_FILTER);

	if (flags & SHARE_FILE_FILTER_IMAGE)
	{
		share_file_filters_add (filters, TITLE_IMAGE, FILTER_IMAGE, gtk_file_filter_add_mime_type);
	}
	if (flags & SHARE_FILE_FILTER_ALL)
	{
		share_file_filters_add (filters, TITLE_ALL, FILTER_ALL, gtk_file_filter_add_pattern);
	}

	gtk_file_dialog_set_filters (dialog, G_LIST_MODEL (filters));
	g_object_unref (filters);
}

/*******************************************************************************
* @brief ファイルを開くダイアログを表示します。
* @param parent ファイル ダイアログの親ウィンドウ。
* @param initial_file ファイル。
*/
void
share_file_dialog_open (GtkWindow *parent, GFile *initial_file, GAsyncReadyCallback callback, gpointer user_data, ShareFileDialogFlag flags)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();
	share_file_dialog_init (dialog, TITLE_OPEN, initial_file);
	share_file_dialog_init_filters (dialog, flags);
	gtk_file_dialog_open (dialog, parent, NULL, callback, user_data);
	g_object_unref (dialog);
}

/*******************************************************************************
* @brief ファイルを保存ダイアログを表示します。
* @param parent ファイル ダイアログの親ウィンドウ。
* @param initial_file ファイル。
*/
void
share_file_dialog_save (GtkWindow *parent, GFile *initial_file, GAsyncReadyCallback callback, gpointer user_data, ShareFileDialogFlag flags)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();
	share_file_dialog_init (dialog, TITLE_OPEN, initial_file);
	share_file_dialog_init_filters (dialog, flags);
	gtk_file_dialog_save (dialog, parent, NULL, callback, user_data);
	g_object_unref (dialog);
}

/*******************************************************************************
* @brief ファイル フィルターを作成します。
* @param filters ファイル フィルターのコレクション。
* @param add ファイル フィルターを作成する方法。
* @param format ファイル フィルター。
*/
static void
share_file_filters_add (GListStore *filters, const char *name, const char *format, ShareFileFilterAdd add)
{
	GtkFileFilter *filter;
	filter = gtk_file_filter_new ();
	gtk_file_filter_set_name (filter, name);
	add (filter, format);
	g_list_store_append (filters, filter);
	g_object_unref (filter);
}

/*******************************************************************************
* @brief 画像ファイルを読み込みます。
* @param file 画像ファイル。
* @return 画像。
*/
GdkPixbuf *
share_pixbuf_create_from_file (GFile *file, GError **error)
{
	GdkPixbuf *pixbuf;
	GFileInputStream *stream;
	stream = g_file_read (file, NULL, error);

	if (stream)
	{
		pixbuf = gdk_pixbuf_new_from_stream (G_INPUT_STREAM (stream), NULL, error);
		g_object_unref (stream);
	}

	return pixbuf;
}

/*******************************************************************************
* @brief モーダル ウィンドウを表示します。
* @param window モーダル ウィンドウ。
* @param parent モーダル ウィンドウの親ウィンドウ。
*/
static void
share_present (GtkWindow *window, GtkWindow *parent)
{
	gtk_window_set_destroy_with_parent (window, TRUE);
	gtk_window_set_modal (window, TRUE);
	gtk_window_set_transient_for (window, parent);
	gtk_window_present (window);
}

/*******************************************************************************
* @brief 印刷を開始します。
*/
static void
share_print_operation_begin (GtkPrintOperation *operation, GtkPrintContext *context, SharePrintData *data)
{
	gtk_print_operation_set_n_pages (operation, PRINT_N_PAGES);
	share_print_operation_init (context, data);
	share_surface_load (data->surface, data->pixbuf);
}

/*******************************************************************************
* @brief 印刷を描画します。
*/
static void
share_print_operation_draw (GtkPrintOperation *operation, GtkPrintContext *context, int page, SharePrintData *data)
{
	cairo_t *cairo;

	if (data->surface)
	{
		cairo = gtk_print_context_get_cairo_context (context);
		cairo_set_source_surface (cairo, data->surface, 0, 0);
		cairo_paint (cairo);
	}
}

/*******************************************************************************
* @brief 印刷を終了します。
*/
static void
share_print_operation_end (GtkPrintOperation *operation, GtkPrintContext *context, SharePrintData *data)
{
	if (data->surface) cairo_surface_destroy (data->surface);
	if (data->pixbuf) g_object_unref (data->pixbuf);
	g_free (data);
}

/*******************************************************************************
* @brief サーフィスを作成します。
*/
static void
share_print_operation_init (GtkPrintContext *context, SharePrintData *data)
{
	cairo_t *cairo;
	cairo_surface_t *target;
	int width, height;

	if (!data->surface && data->pixbuf)
	{
		cairo = gtk_print_context_get_cairo_context (context);
		target = cairo_get_target (cairo);
		width = gdk_pixbuf_get_width (data->pixbuf);
		height = gdk_pixbuf_get_height (data->pixbuf);
		data->surface = cairo_surface_create_similar (target, CAIRO_CONTENT_COLOR_ALPHA, width, height);
	}
}

/*******************************************************************************
* @brief 印刷ダイアログを表示します。
* @param parent 印刷ダイアログの親ウィンドウ。
* @param pixbuf 印刷する画像。
*/
GtkPrintOperationResult
share_print_operation_run (GtkWindow *parent, GdkPixbuf *pixbuf, GError **error)
{
	GtkPrintOperation *operation;
	SharePrintData *data;
	GtkPrintOperationResult result;
	operation = gtk_print_operation_new ();
	data = g_malloc0 (sizeof (SharePrintData));
	data->pixbuf = g_object_ref (pixbuf);
	g_signal_connect (operation, SIGNAL_BEGIN_PRINT, G_CALLBACK (share_print_operation_begin), data);
	g_signal_connect (operation, SIGNAL_DRAW_PAGE, G_CALLBACK (share_print_operation_draw), data);
	g_signal_connect (operation, SIGNAL_END_PRINT, G_CALLBACK (share_print_operation_end), data);
	gtk_print_operation_set_embed_page_setup (operation, TRUE);
	gtk_print_operation_set_unit (operation, GTK_UNIT_PIXEL);
	gtk_print_operation_set_use_full_page (operation, FALSE);
	result = gtk_print_operation_run (operation, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, parent, error);
	g_object_unref (operation);
	return result;
}

/*******************************************************************************
* @brief 指定したサーフィスの内容を初期化します。
* @param surface 書き込み先。
* @param pixbuf 読み込み元。
*/
void
share_surface_load (cairo_surface_t *surface, GdkPixbuf *pixbuf)
{
	cairo_t *cairo;
	cairo = cairo_create (surface);

	if (cairo)
	{
		cairo_set_antialias (cairo, CAIRO_ANTIALIAS_NONE);
		cairo_set_operator (cairo, CAIRO_OPERATOR_SOURCE);
		gdk_cairo_set_source_pixbuf (cairo, pixbuf, 0, 0);
		cairo_paint (cairo);
		cairo_destroy (cairo);
	}
}
