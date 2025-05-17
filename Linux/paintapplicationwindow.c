/*
Copyright 2025 Taichi Murakami.
ドキュメントを含むウィンドウを実装します。
*/

#include "paint.h"
#define ALERT_RESPONSE_CANCEL   0
#define ALERT_RESPONSE_DESTROY  1
#define ALERT_RESPONSE_SAVE     2
#define CCH_TITLE               256
#define DEFAULT_HEIGHT          480
#define DEFAULT_WIDTH           640
#define FORMAT_ALERT            "%s"
#define FORMAT_TITLE            "%s%s - %s"
#define ID_TOOLBAR              "toolbar"
#define MODE_BRUSH              "brush"
#define MODE_ERASER             "eraser"
#define MODE_PENCIL             "pencil"
#define PROPERTY_APPLICATION    "application"
#define PROPERTY_SHOW_MENUBAR   "show-menubar"
#define SIGNAL_CLICKED          "clicked"
#define SIGNAL_DOWN             "down"
#define SIGNAL_MOTION           "motion"
#define SIGNAL_PROXIMITY        "proximity"
#define SIGNAL_UP               "up"
#define UI_TOOLBAR              "toolbar.ui"

static void     paint_application_window_activate_about        (GSimpleAction *action, GVariant *parameter, gpointer window);
static void     paint_application_window_activate_flip_horizontal (GSimpleAction *action, GVariant *parameter, gpointer window);
static void     paint_application_window_activate_flip_vertical (GSimpleAction *action, GVariant *parameter, gpointer window);
static void     paint_application_window_activate_mode         (GSimpleAction *action, GVariant *parameter, gpointer window);
static void     paint_application_window_activate_open         (GSimpleAction *action, GVariant *parameter, gpointer window);
static void     paint_application_window_activate_print        (GSimpleAction *action, GVariant *parameter, gpointer window);
static void     paint_application_window_activate_save         (GSimpleAction *action, GVariant *parameter, gpointer window);
static void     paint_application_window_activate_save_as      (GSimpleAction *action, GVariant *parameter, gpointer window);
static void     paint_application_window_activate_undo         (GSimpleAction *action, GVariant *parameter, gpointer window);
static void     paint_application_window_class_init            (PaintApplicationWindowClass *window);
static void     paint_application_window_class_init_object     (GObjectClass *object);
static void     paint_application_window_class_init_window     (GtkWindowClass *window);
static gboolean paint_application_window_close_request         (GtkWindow *window);
static gboolean paint_application_window_close_request_self    (PaintApplicationWindow *window);
static void     paint_application_window_constructed           (GObject *object);
static void     paint_application_window_dispose               (GObject *object);
static void     paint_application_window_dispose_self          (PaintApplicationWindow *window);
static void     paint_application_window_draw                  (PaintApplicationWindow *window, cairo_t *cairo, int width, int height);
static void     paint_application_window_draw_canvas           (GtkDrawingArea *canvas, cairo_t *cairo, int width, int height, gpointer window);
static void     paint_application_window_draw_surface          (PaintApplicationWindow *window, double x, double y);
static void     paint_application_window_init                  (PaintApplicationWindow *window);
static void     paint_application_window_init_canvas           (PaintApplicationWindow *window);
static void     paint_application_window_init_content          (PaintApplicationWindow *window);
static void     paint_application_window_init_surface          (PaintApplicationWindow *window);
static void     paint_application_window_init_surface_from_file (PaintApplicationWindow *window);
static void     paint_application_window_init_toolbar          (PaintApplicationWindow *window);
static void     paint_application_window_move                  (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer window);
static void     paint_application_window_move_down             (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer window);
static void     paint_application_window_move_proximity        (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer window);
static void     paint_application_window_move_up               (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer window);
static void     paint_application_window_respond_alert         (GObject *dialog, GAsyncResult *result, gpointer window);
static void     paint_application_window_respond_open          (GObject *dialog, GAsyncResult *result, gpointer window);
static void     paint_application_window_respond_save          (GObject *dialog, GAsyncResult *result, gpointer window);
static void     paint_application_window_set_filename          (PaintApplicationWindow *window, const char *filename);
static void     paint_application_window_set_modification      (PaintApplicationWindow *window, gboolean modified);
static void     paint_application_window_show_about_dialog     (PaintApplicationWindow *window);
static void     paint_application_window_show_alert_dialog     (PaintApplicationWindow *window);
static void     paint_application_window_show_file_dialog_open (PaintApplicationWindow *window);
static void     paint_application_window_show_file_dialog_save (PaintApplicationWindow *window);
static void     paint_application_window_update_cursor_pos     (PaintApplicationWindow *window, double x, double y);
static void     paint_application_window_update_title          (PaintApplicationWindow *window);

/**
 * Paint Application Window クラス。
 */
struct _PaintApplicationWindow
{
	GtkApplicationWindow super;
	GtkWidget *canvas;
	GtkWidget *client;
	GtkWidget *content;
	GtkWidget *dock;
	GtkWidget *toolbar;
	cairo_surface_t *surface;
	char *filename;
	size_t basename;
	double x;
	double y;
	unsigned char modified;
};

G_DEFINE_FINAL_TYPE (PaintApplicationWindow, paint_application_window, GTK_TYPE_APPLICATION_WINDOW);

/**
 * メニュー項目とコールバック関数。
 */
static const PaintActionEntry paint_application_window_action_entries [] =
{
	{ PAINT_ACTION_ABOUT,           paint_application_window_activate_about                                        },
	{ PAINT_ACTION_FLIP_HORIZONTAL, paint_application_window_activate_flip_horizontal, PAINT_ACTION_ENTRY_STATEFUL },
	{ PAINT_ACTION_FLIP_VERTICAL,   paint_application_window_activate_flip_vertical,   PAINT_ACTION_ENTRY_STATEFUL },
	{ PAINT_ACTION_MODE,            paint_application_window_activate_mode,            PAINT_MODE_PENCIL           },
	{ PAINT_ACTION_OPEN,            paint_application_window_activate_open                                         },
	{ PAINT_ACTION_PRINT,           paint_application_window_activate_print                                        },
	{ PAINT_ACTION_SAVE,            paint_application_window_activate_save                                         },
	{ PAINT_ACTION_SAVE_AS,         paint_application_window_activate_save_as,                                     },
	{ PAINT_ACTION_UNDO,            paint_application_window_activate_undo                                         },
};

static const char *paint_file_patterns_all [] = { "*", NULL };
static const char *paint_file_patterns_image [] = { "*.png", NULL };

static const PaintFileFilter paint_file_filters [] =
{
	{ "All Files", paint_file_patterns_all },
	{ "Image Files", paint_file_patterns_image },
};

/**
 * メニュー: バージョン情報。
 */
void paint_application_window_activate_about (GSimpleAction *action, GVariant *parameter, gpointer window)
{
	paint_application_window_show_about_dialog (window);
}

/**
 * メニュー: Flip Horizontal。
 */
void paint_application_window_activate_flip_horizontal (GSimpleAction *action, GVariant *parameter, gpointer window)
{
	g_action_change_state (G_ACTION (action), g_variant_new_boolean (!g_variant_get_boolean (g_action_get_state (G_ACTION (action)))));
}

/**
 * メニュー: Flip Vertical。
 */
void paint_application_window_activate_flip_vertical (GSimpleAction *action, GVariant *parameter, gpointer window)
{
	g_action_change_state (G_ACTION (action), g_variant_new_boolean (!g_variant_get_boolean (g_action_get_state (G_ACTION (action)))));
}

/**
 * メニュー: モード。
 */
void paint_application_window_activate_mode (GSimpleAction *action, GVariant *parameter, gpointer window)
{
	g_action_change_state (G_ACTION (action), parameter);
}

/**
 * メニュー: ファイルを開く。
 */
void paint_application_window_activate_open (GSimpleAction *action, GVariant *parameter, gpointer window)
{
	paint_application_window_show_file_dialog_open (window);
}

/**
 * メニュー: 印刷。
 */
void paint_application_window_activate_print (GSimpleAction *action, GVariant *parameter, gpointer window)
{
}

/**
 * メニュー: 保存。
 */
void paint_application_window_activate_save (GSimpleAction *action, GVariant *parameter, gpointer window)
{
	paint_application_window_set_modification (window, FALSE);
}

/**
 * メニュー: 名前を付けて保存。
 */
void paint_application_window_activate_save_as (GSimpleAction *action, GVariant *parameter, gpointer window)
{
	paint_application_window_show_file_dialog_save (window);
}

/**
 * メニュー: 元に戻す。
 */
void paint_application_window_activate_undo (GSimpleAction *action, GVariant *parameter, gpointer window)
{
}

/**
 * クラスを初期化します。
 */
void paint_application_window_class_init (PaintApplicationWindowClass *window)
{
	paint_application_window_class_init_object (G_OBJECT_CLASS (window));
	paint_application_window_class_init_window (GTK_WINDOW_CLASS (window));
}

/**
 * Object クラスを初期化します。
 */
void paint_application_window_class_init_object (GObjectClass *object)
{
	object->constructed = paint_application_window_constructed;
	object->dispose = paint_application_window_dispose;
}

/**
 * Window クラスを初期化します。
 */
void paint_application_window_class_init_window (GtkWindowClass *window)
{
	window->close_request = paint_application_window_close_request;
}

/**
 * この関数はウィンドウが閉じられる場合に呼び出されます。
 */
gboolean paint_application_window_close_request (GtkWindow *window)
{
	return paint_application_window_close_request_self (PAINT_APPLICATION_WINDOW (window));
}

/**
 * 現在のドキュメントが更新された場合はダイアログを表示します。
 */
gboolean paint_application_window_close_request_self (PaintApplicationWindow *window)
{
	if (window->modified)
	{
		paint_application_window_show_alert_dialog (window);
	}

	return window->modified;
}

/**
 * この関数はインスタンスが作成された場合に呼び出されます。
 */
void paint_application_window_constructed (GObject *object)
{
	G_OBJECT_CLASS (paint_application_window_parent_class)->constructed (object);
	gtk_window_set_default_size (GTK_WINDOW (object), 640, 480);
}

/**
 * この関数は現在のインスタンスが破棄される場合に呼び出されます。
 */
void paint_application_window_dispose (GObject *object)
{
	paint_application_window_dispose_self (PAINT_APPLICATION_WINDOW (object));
	//gtk_widget_dispose_template (GTK_WIDGET(object), PAINT_TYPE_APPLICATION_WINDOW);
	G_OBJECT_CLASS (paint_application_window_parent_class)->dispose (object);
}

/**
 * 自身のプロパティを開放します。
 */
void paint_application_window_dispose_self (PaintApplicationWindow *window)
{
	g_clear_pointer (&window->filename, free);
	g_clear_pointer (&window->surface, cairo_surface_destroy);
}

/**
 * 現在のウィンドウ上に描画します。
 */
void paint_application_window_draw (PaintApplicationWindow *window, cairo_t *cairo, int width, int height)
{
	if (window->surface)
	{
		cairo_set_source_surface (cairo, window->surface, 0, 0);
		cairo_paint (cairo);
	}
}

/**
 * 現在のウィンドウ上に描画します。
 */
void paint_application_window_draw_canvas (GtkDrawingArea *canvas, cairo_t *cairo, int width, int height, gpointer window)
{
	paint_application_window_draw (window, cairo, width, height);
}

/**
 * 現在のサーフィス上に描画します。
 */
void paint_application_window_draw_surface (PaintApplicationWindow *window, double x, double y)
{
	cairo_t *cairo;

	if (window->surface)
	{
		cairo = cairo_create (window->surface);

		if (cairo)
		{
			cairo_set_antialias (cairo, CAIRO_ANTIALIAS_NONE);
			cairo_set_line_width (cairo, 10);
			cairo_set_line_cap (cairo, CAIRO_LINE_CAP_ROUND);
			cairo_move_to (cairo, window->x, window->y);
			cairo_line_to (cairo, x, y);
			cairo_stroke (cairo);
			cairo_destroy (cairo);

			if (window->canvas)
			{
				gtk_widget_queue_draw (GTK_WIDGET (window->canvas));
			}
		}
	}
}

/**
 * クラスの新しいインスタンスを初期化します。
 */
void paint_application_window_init (PaintApplicationWindow *window)
{
	//gtk_widget_init_template (GTK_WIDGET (window));
	paint_action_map_add_action_entries (G_ACTION_MAP (window), paint_application_window_action_entries, G_N_ELEMENTS (paint_application_window_action_entries), window);
	paint_application_window_init_content (window);
	paint_application_window_init_surface (window);
	paint_application_window_init_canvas (window);
	paint_application_window_init_toolbar (window);
	paint_application_window_update_title (window);
}

/**
 * 新しいジェスチャを作成します。
 */
void paint_application_window_init_canvas (PaintApplicationWindow *window)
{
	GtkGesture *gesture;

	if (window->canvas)
	{
		gesture = gtk_gesture_stylus_new ();
		gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (window->canvas), paint_application_window_draw_canvas, window, NULL);

		if (gesture)
		{
			gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (gesture), GTK_PHASE_CAPTURE);
			gtk_gesture_stylus_set_stylus_only (GTK_GESTURE_STYLUS (gesture), FALSE);
			g_signal_connect (gesture, SIGNAL_DOWN, G_CALLBACK (paint_application_window_move_down), window);
			g_signal_connect (gesture, SIGNAL_MOTION, G_CALLBACK (paint_application_window_move), window);
			g_signal_connect (gesture, SIGNAL_PROXIMITY, G_CALLBACK (paint_application_window_move_proximity), window);
			g_signal_connect (gesture, SIGNAL_UP, G_CALLBACK (paint_application_window_move_up), window);
			gtk_widget_add_controller (GTK_WIDGET (window->canvas), GTK_EVENT_CONTROLLER (gesture));
		}
	}
}

void paint_application_window_init_content (PaintApplicationWindow *window)
{
	window->content = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	if (window->content)
	{
		gtk_window_set_child (GTK_WINDOW (window), window->content);
		window->client = gtk_scrolled_window_new ();

		if (window->client)
		{
			gtk_widget_set_vexpand (window->client, TRUE);
			gtk_box_append (GTK_BOX (window->content), window->client);
			window->canvas = gtk_drawing_area_new ();

			if (window->canvas)
			{
				gtk_drawing_area_set_content_height (GTK_DRAWING_AREA (window->canvas), 1000);
				gtk_drawing_area_set_content_width (GTK_DRAWING_AREA (window->canvas), 1000);
				gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (window->client), window->canvas);
			}
		}
	}
}

/**
 * 新しいサーフィスを作成します。
 */
void paint_application_window_init_surface (PaintApplicationWindow *window)
{
	if (window->surface)
	{
		cairo_surface_destroy (window->surface);
	}

	window->surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, DEFAULT_WIDTH, DEFAULT_HEIGHT);

	if (window->canvas)
	{
		gtk_widget_queue_draw (GTK_WIDGET (window->canvas));
	}
}

/**
 * 新しいサーフィスを作成します。
 */
void paint_application_window_init_surface_from_file (PaintApplicationWindow *window)
{
	if (window->surface)
	{
		cairo_surface_destroy (window->surface);
	}

	window->surface = cairo_image_surface_create_from_png (window->filename);

	if (window->canvas)
	{
		gtk_widget_queue_draw (GTK_WIDGET (window->canvas));
	}
}

void paint_application_window_init_toolbar (PaintApplicationWindow *window)
{
	if (window->content)
	{
		window->toolbar = paint_toolbar_new ();

		if (window->toolbar)
		{
			gtk_box_insert_child_after (GTK_BOX (window->content), window->toolbar, NULL);
		}
	}
}

void paint_application_window_move (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer window)
{
	paint_application_window_draw_surface (window, x, y);
	paint_application_window_update_cursor_pos (window, x ,y);
}

void paint_application_window_move_down (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer window)
{
	paint_application_window_update_cursor_pos (window, x ,y);
}

void paint_application_window_move_proximity (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer window)
{
	paint_application_window_update_cursor_pos (window, x ,y);
}

void paint_application_window_move_up (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer window)
{
	paint_application_window_set_modification (window, TRUE);
}

/**
 * クラスの新しいインスタンスを初期化します。
 */
GtkWidget *paint_application_window_new (GApplication *application)
{
	return g_object_new (PAINT_TYPE_APPLICATION_WINDOW, PROPERTY_APPLICATION, application, PROPERTY_SHOW_MENUBAR, TRUE, NULL);
}

/**
 * クラスの新しいインスタンスを初期化します。
 */
GtkWidget *paint_application_window_new_from_file (GApplication *application, const char *filename)
{
	return paint_application_window_new (application);
}

void paint_application_window_respond_alert (GObject *dialog, GAsyncResult *result, gpointer window)
{
	int response;
	response = gtk_alert_dialog_choose_finish (GTK_ALERT_DIALOG (dialog), result, NULL);

	switch (response)
	{
	case ALERT_RESPONSE_CANCEL:
		break;
	case ALERT_RESPONSE_DESTROY:
		g_idle_add_once ((GSourceOnceFunc) gtk_window_destroy, window);
		break;
	case ALERT_RESPONSE_SAVE:
		paint_application_window_show_file_dialog_save (window);
		break;
	default:
		g_assert_not_reached ();
		break;
	}
}

/**
 * この関数はファイルを開くダイアログを終了した場合に呼び出されます。
 */
void paint_application_window_respond_open (GObject *dialog, GAsyncResult *result, gpointer window)
{
	GFile *file;
	char *path;
	file = gtk_file_dialog_open_finish (GTK_FILE_DIALOG (dialog), result, NULL);

	if (file)
	{
		path = g_file_get_path (file);

		if (path)
		{
			paint_application_window_set_filename (window, path);
			paint_application_window_init_surface_from_file (window);
			g_free (path);
		}

		g_object_unref (file);
	}
}

/**
 * この関数は保存ダイアログを終了した場合に呼び出されます。
 */
void paint_application_window_respond_save (GObject *dialog, GAsyncResult *result, gpointer window)
{
	GFile *file;
	char *path;
	file = gtk_file_dialog_save_finish (GTK_FILE_DIALOG (dialog), result, NULL);

	if (file)
	{
		path = g_file_get_path (file);

		if (path)
		{
			paint_application_window_set_filename (window, path);
			g_free (path);
		}

		g_object_unref (file);
	}
}

void paint_application_window_set_filename (PaintApplicationWindow *window, const char *filename)
{
	size_t length;

	if (window->filename)
	{
		free (window->filename);
		window->filename = NULL;
		window->basename = 0;
	}
	if (filename)
	{
		length = strlen (filename) + 1;

		if (length)
		{
			window->filename = malloc (length);

			if (window->filename)
			{
				memcpy (window->filename, filename, length);
				window->modified = FALSE;
				filename = strrchr (window->filename, G_DIR_SEPARATOR);

				if (filename)
				{
					window->basename = (size_t) (filename - window->filename) + 1;
				}
			}
		}
	}

	paint_application_window_update_title (window);
}

void paint_application_window_set_modification (PaintApplicationWindow *window, gboolean modified)
{
	if (window->modified != modified)
	{
		window->modified = modified;
		paint_application_window_update_title (window);
	}
}

void paint_application_window_show_about_dialog (PaintApplicationWindow *window)
{
	GtkWidget *dialog;
	dialog = paint_about_dialog_new (GTK_WINDOW (window));

	if (dialog)
	{
		gtk_window_present (GTK_WINDOW (dialog));
	}
}

void paint_application_window_show_alert_dialog (PaintApplicationWindow *window)
{
	const char *labels [] = { "_Cancel", "_Destroy", "_Save", NULL };
	GtkAlertDialog *dialog;
	dialog = gtk_alert_dialog_new (FORMAT_ALERT, window->filename);

	if (dialog)
	{
		gtk_alert_dialog_set_buttons (dialog, labels);
		gtk_alert_dialog_set_cancel_button (dialog, 0);
		gtk_alert_dialog_set_default_button (dialog, 2);
		gtk_alert_dialog_set_modal (dialog, TRUE);
		gtk_alert_dialog_choose (dialog, GTK_WINDOW (window), NULL, paint_application_window_respond_alert, window);
		g_object_unref (dialog);
	}
}

/**
 * ファイルを開くダイアログを表示します。
 */
void paint_application_window_show_file_dialog_open (PaintApplicationWindow *window)
{
	GtkFileDialog *dialog;
	GListModel *filters;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		filters = paint_file_filter_list_new (paint_file_filters, G_N_ELEMENTS (paint_file_filters));

		if (filters)
		{
			gtk_file_dialog_set_filters (dialog, filters);
			//gtk_file_dialog_set_default_filter (dialog, GTK_FILE_FILTER (gtk_filter_list_model_get_filter (filters)));
			g_object_unref (filters);
		}

		gtk_file_dialog_set_title (dialog, "Open File");
		gtk_file_dialog_open (dialog, GTK_WINDOW (window), NULL, paint_application_window_respond_open, window);
		g_object_unref (dialog);
	}
}

/**
 * 保存ダイアログを表示します。
 */
void paint_application_window_show_file_dialog_save (PaintApplicationWindow *window)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		gtk_file_dialog_set_title (dialog, "Save File");
		gtk_file_dialog_save (dialog, GTK_WINDOW (window), NULL, paint_application_window_respond_save, window);
		g_object_unref (dialog);
	}
}

void paint_application_window_update_cursor_pos (PaintApplicationWindow *window, double x, double y)
{
	window->x = x;
	window->y = y;
}

void paint_application_window_update_title (PaintApplicationWindow *window)
{
	const char *basename;
	const char *modified;
	char title [CCH_TITLE];

	if (window->filename)
	{
		basename = window->filename + window->basename;
	}
	else
	{
		basename = "(Untitled)";
	}
	if (window->modified)
	{
		modified = "*";
	}
	else
	{
		modified = "";
	}

	snprintf (title, CCH_TITLE, FORMAT_TITLE, modified, basename, paint_application_name);
	gtk_window_set_title (GTK_WINDOW (window), title);
}
