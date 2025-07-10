/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "text.h"
#define PAINT_EDITOR_WINDOW_TITLE_CCH 256
#define PAINT_EDITOR_WINDOW_TITLE_FORMAT "%s - %s"

typedef void (*PaintEditorActivate) (GSimpleAction *, GVariant *, gpointer);
typedef struct _PaintEditorActionEntry PaintEditorActionEntry;

/* メニュー */
struct _PaintEditorActionEntry
{
	const char *name;
	PaintEditorActivate activate;
};

/* クラスのインスタンス */
struct _PaintEditorWindow
{
	GtkApplicationWindow parent_instance;
	char                *name; /* タイトル バー上に表示される名前 */
	cairo_surface_t     *surface; /* キャンバス上に描画される画像 */
	GFile               *file; /* 開いたファイル */
	GtkWidget           *content;
	GtkWidget           *canvas;
	GtkWidget           *client;
	gdouble              surface_x;
	gdouble              surface_y;
	gint                 content_width;
	gint                 content_height;
	gint                 zoom;
	guint8               closing;
	guint8               modified;
	guint8               surface_loading;
	guint8               canvas_pressed;
};

static void
paint_editor_window_activate_about (GSimpleAction *, GVariant *, gpointer);
static void
paint_editor_window_activate_open (GSimpleAction *, GVariant *, gpointer);
static void
paint_editor_window_activate_save (GSimpleAction *, GVariant *, gpointer);
static void
paint_editor_window_activate_save_as (GSimpleAction *, GVariant *, gpointer);
static void
paint_editor_window_canvas_move (GtkEventControllerMotion *, gdouble, gdouble, gpointer);
static void
paint_editor_window_canvas_press (GtkGestureClick *, gint, gdouble, gdouble, gpointer);
static void
paint_editor_window_canvas_release (GtkGestureClick *, gint, gdouble, gdouble, gpointer);
static void
paint_editor_window_class_init (PaintEditorWindowClass *);
static gboolean
paint_editor_window_close (GtkWindow *);
static void
paint_editor_window_destroy (PaintEditorWindow *);
static void
paint_editor_window_dispose (GObject *);
static void
paint_editor_window_draw_canvas (GtkDrawingArea *, cairo_t *, int, int, gpointer);
static void
paint_editor_window_init (PaintEditorWindow *);
static void
paint_editor_window_init_about_dialog (GtkAboutDialog *);
static void
paint_editor_window_init_actions (GActionMap *);
static void
paint_editor_window_init_canvas (PaintEditorWindow *);
static void
paint_editor_window_init_canvas_click (PaintEditorWindow *);
static void
paint_editor_window_init_canvas_motion (PaintEditorWindow *);
static void
paint_editor_window_init_client (PaintEditorWindow *);
static void
paint_editor_window_init_content (PaintEditorWindow *);
static void
paint_editor_window_init_file_dialog (PaintEditorWindow *, GtkFileDialog *);
static void
paint_editor_window_init_window (GtkWindow *);
static void
paint_editor_window_load_surface (PaintEditorWindow *, GFile *, cairo_t *);
static void
paint_editor_window_present_dialog (GtkWindow *, GtkWindow *);
//static void
//paint_editor_window_present_tool (GtkWindow *, GtkWindow *);
static void
paint_editor_window_respond_closing (GObject *, GAsyncResult *, gpointer);
static void
paint_editor_window_respond_open (GObject *, GAsyncResult *, gpointer);
static void
paint_editor_window_respond_save (GObject *, GAsyncResult *, gpointer);
static void
paint_editor_window_set_file (PaintEditorWindow *, GFile *);
static void
paint_editor_window_set_name (PaintEditorWindow *, const char *);
static void
paint_editor_window_set_surface (PaintEditorWindow *, cairo_surface_t *);
static void
paint_editor_window_show_about_dialog (PaintEditorWindow *);
static void
paint_editor_window_show_closing_dialog (PaintEditorWindow *);
static void
paint_editor_window_show_error_dialog (PaintEditorWindow *, GError *);
static void
paint_editor_window_show_open_dialog (PaintEditorWindow *);
static void
paint_editor_window_show_save_dialog (PaintEditorWindow *);
static void
paint_editor_window_update_canvas (PaintEditorWindow *);
static void
paint_editor_window_update_content (PaintEditorWindow *);
static void
paint_editor_window_update_name (PaintEditorWindow *);
static void
paint_editor_window_update_title (PaintEditorWindow *);

G_DEFINE_FINAL_TYPE (PaintEditorWindow, paint_editor_window, GTK_TYPE_APPLICATION_WINDOW);

/* メニュー */
static const PaintEditorActionEntry paint_editor_action_entries [] =
{
	{ "about",   paint_editor_window_activate_about   },
	{ "open",    paint_editor_window_activate_open    },
	{ "save",    paint_editor_window_activate_save    },
	{ "save-as", paint_editor_window_activate_save_as },
};

/*******************************************************************************
 * @brief メニュー: バージョン情報。
 ******************************************************************************/
void
paint_editor_window_activate_about (GSimpleAction *, GVariant *, gpointer self)
{
	paint_editor_window_show_about_dialog (PAINT_EDITOR_WINDOW (self));
}

/*******************************************************************************
 * @brief メニュー: ファイルを開く。
 ******************************************************************************/
void
paint_editor_window_activate_open (GSimpleAction *, GVariant *, gpointer self)
{
	paint_editor_window_show_open_dialog (PAINT_EDITOR_WINDOW (self));
}

/*******************************************************************************
 * @brief メニュー: 上書き保存。
 ******************************************************************************/
void
paint_editor_window_activate_save (GSimpleAction *, GVariant *, gpointer self)
{
	PaintEditorWindow *editor;
	editor = PAINT_EDITOR_WINDOW (self);

	if (editor->modified)
	{
		paint_editor_window_show_save_dialog (editor);
	}
	else
	{
	}
}

/*******************************************************************************
 * @brief メニュー: 名前を付けて保存。
 ******************************************************************************/
void
paint_editor_window_activate_save_as (GSimpleAction *, GVariant *, gpointer self)
{
	paint_editor_window_show_save_dialog (PAINT_EDITOR_WINDOW (self));
}

void
paint_editor_window_canvas_move (GtkEventControllerMotion *, gdouble canvas_x, gdouble canvas_y, gpointer self)
{
	PaintEditorWindow *editor;
	cairo_t *context;
	editor = PAINT_EDITOR_WINDOW (self);

	if (editor->surface && editor->canvas_pressed)
	{
		context = cairo_create (editor->surface);

		if (context)
		{
			cairo_move_to (context, editor->surface_x, editor->surface_y);
			cairo_line_to (context, canvas_x, canvas_y);
			cairo_stroke (context);
			cairo_destroy (context);
		}

		gtk_widget_queue_draw (editor->canvas);
	}

	editor->surface_x = canvas_x;
	editor->surface_y = canvas_y;
}

void
paint_editor_window_canvas_press (GtkGestureClick *click, gint, gdouble canvas_x, gdouble canvas_y, gpointer self)
{
	PaintEditorWindow *editor;
	editor = PAINT_EDITOR_WINDOW (self);

	if (gtk_gesture_single_get_current_button (GTK_GESTURE_SINGLE (click)) == GDK_BUTTON_PRIMARY)
	{
		editor->canvas_pressed = TRUE;
		editor->surface_x = canvas_x;
		editor->surface_y = canvas_y;
	}
}

void
paint_editor_window_canvas_release (GtkGestureClick *click, gint, gdouble canvas_x, gdouble canvas_y, gpointer self)
{
	PaintEditorWindow *editor;
	editor = PAINT_EDITOR_WINDOW (self);

	if (gtk_gesture_single_get_current_button (GTK_GESTURE_SINGLE (click)) == GDK_BUTTON_PRIMARY)
	{
		editor->canvas_pressed = FALSE;
		editor->surface_x = canvas_x;
		editor->surface_y = canvas_y;
	}
}

/*******************************************************************************
 * @brief クラスのコールバック関数を登録する。
 ******************************************************************************/
void
paint_editor_window_class_init (PaintEditorWindowClass *editor)
{
	GObjectClass *object;
	GtkWindowClass *window;
	object = G_OBJECT_CLASS (editor);
	window = GTK_WINDOW_CLASS (editor);
	object->dispose       = paint_editor_window_dispose;
	window->close_request = paint_editor_window_close;
}

/*******************************************************************************
 * @brief 現在のウィンドウを閉じる。
 * @return ウィンドウを閉じる場合は FALSE を返す。
 ******************************************************************************/
gboolean
paint_editor_window_close (GtkWindow *window)
{
	PaintEditorWindow *editor;
	gboolean result;
	editor = PAINT_EDITOR_WINDOW (window);
	result = editor->modified;

	if (result)
	{
		paint_editor_window_show_closing_dialog (editor);
	}

	return result;
}

/*******************************************************************************
 * @brief メンバー変数を破棄する。
 ******************************************************************************/
void
paint_editor_window_destroy (PaintEditorWindow *editor)
{
	g_clear_pointer (&editor->name, g_free);
	g_clear_pointer (&editor->surface, cairo_surface_destroy);
	g_clear_object (&editor->file);
}

/*******************************************************************************
 * @brief クラスのインスタンスを破棄する。
 ******************************************************************************/
void
paint_editor_window_dispose (GObject *object)
{
	paint_editor_window_destroy (PAINT_EDITOR_WINDOW (object));
	G_OBJECT_CLASS (paint_editor_window_parent_class)->dispose (object);
}

/*******************************************************************************
 * @brief キャンバス上にサーフィスを描画する。
 ******************************************************************************/
void
paint_editor_window_draw_canvas (GtkDrawingArea *canvas, cairo_t *context, int, int, gpointer self)
{
	PaintEditorWindow *editor;
	editor = PAINT_EDITOR_WINDOW (self);
	// cairo_set_source_rgb (context, 255, 255, 255);
	// cairo_paint (context);

	if (editor->surface_loading)
	{
		paint_editor_window_load_surface (editor, editor->file, context);
		editor->surface_loading = FALSE;
	}
	if (editor->surface)
	{
		cairo_set_source_surface (context, editor->surface, 0, 0);
		cairo_paint (context);
	}
}

/*******************************************************************************
 * @brief クラスのインスタンスを初期化する。
 ******************************************************************************/
void
paint_editor_window_init (PaintEditorWindow *editor)
{
	editor->zoom = PAINT_ZOOM_DEFAULT_VALUE;
	paint_editor_window_init_actions (G_ACTION_MAP (editor));
	paint_editor_window_init_window  (GTK_WINDOW (editor));
	paint_editor_window_init_content (editor);
	paint_editor_window_init_client  (editor);
	paint_editor_window_init_canvas  (editor);
	paint_editor_window_init_canvas_click (editor);
	paint_editor_window_init_canvas_motion (editor);
}

/*******************************************************************************
 * @brief バージョン情報ダイアログ ボックスを初期化する。
 * @param dialog ダイアログ ボックス。
 ******************************************************************************/
void
paint_editor_window_init_about_dialog (GtkAboutDialog *dialog)
{
	GdkTexture *logo;
	char path [PAINT_RESOURCE_PATH_CCH];
	gtk_about_dialog_set_authors        (dialog, TEXT_AUTHORS);
	gtk_about_dialog_set_copyright      (dialog, TEXT_COPYRIGHT);
	gtk_about_dialog_set_license_type   (dialog, GTK_LICENSE_APACHE_2_0);
	gtk_about_dialog_set_program_name   (dialog, TEXT_TITLE);
	gtk_about_dialog_set_version        (dialog, TEXT_VERSION);
	gtk_about_dialog_set_website        (dialog, TEXT_WEBSITE);
	paint_resource_format_path (path, PAINT_RESOURCE_PATH_CCH, "pencil.png");
	logo = gdk_texture_new_from_resource (path);

	if (logo)
	{
		gtk_about_dialog_set_logo (dialog, GDK_PAINTABLE (logo));
		g_object_unref (logo);
	}
}

/*******************************************************************************
 * @brief メニュー項目のコールバック関数を登録する。
 ******************************************************************************/
void
paint_editor_window_init_actions (GActionMap *actions)
{
	const PaintEditorActionEntry *entries;
	GSimpleAction *action;
	unsigned index;
	entries = paint_editor_action_entries;

	for (index = 0; index < G_N_ELEMENTS (paint_editor_action_entries); index++)
	{
		action = g_simple_action_new (entries->name, NULL);

		if (action)
		{
			g_signal_connect (action, "activate", G_CALLBACK (entries->activate), actions);
			g_action_map_add_action (actions, G_ACTION (action));
			g_object_unref (action);
		}

		entries++;
	}
}

/*******************************************************************************
 * @brief 描画領域を作成する。
 ******************************************************************************/
void
paint_editor_window_init_canvas (PaintEditorWindow *editor)
{
	if (editor->client)
	{
		editor->canvas = gtk_drawing_area_new ();

		if (editor->canvas)
		{
			gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (editor->canvas), paint_editor_window_draw_canvas, editor, NULL);
			gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (editor->client), editor->canvas);
		}
	}
}

void
paint_editor_window_init_canvas_click (PaintEditorWindow *editor)
{
	GtkGesture *gesture;

	if (editor->canvas)
	{
		gesture = gtk_gesture_click_new ();

		if (gesture)
		{
			g_signal_connect (gesture, "pressed", G_CALLBACK (paint_editor_window_canvas_press), editor);
			g_signal_connect (gesture, "released", G_CALLBACK (paint_editor_window_canvas_release), editor);
			gtk_widget_add_controller (editor->canvas, GTK_EVENT_CONTROLLER (gesture));
		}
	}
}

void
paint_editor_window_init_canvas_motion (PaintEditorWindow *editor)
{
	GtkEventController *controller;

	if (editor->canvas)
	{
		controller = gtk_event_controller_motion_new ();

		if (controller)
		{
			g_signal_connect (controller, "motion", G_CALLBACK (paint_editor_window_canvas_move), editor);
			gtk_widget_add_controller (editor->canvas, controller);
		}
	}
}

/*******************************************************************************
 * @brief スルロール可能ウィンドウを作成する。
 ******************************************************************************/
void
paint_editor_window_init_client (PaintEditorWindow *editor)
{
	if (editor->content)
	{
		editor->client = gtk_scrolled_window_new ();

		if (editor->client)
		{
			gtk_widget_set_vexpand (editor->client, TRUE);
			gtk_box_append (GTK_BOX (editor->content), editor->client);
		}
	}
}

/*******************************************************************************
 * @brief ボックスを作成する。
 ******************************************************************************/
void
paint_editor_window_init_content (PaintEditorWindow *editor)
{
	editor->content = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	if (editor->content)
	{
		gtk_window_set_child (GTK_WINDOW (editor), editor->content);
	}
}

/*******************************************************************************
 * @brief ファイル ダイアログ ボックスを初期化する。
 * @param dialog ダイアログ ボックス。
 ******************************************************************************/
void
paint_editor_window_init_file_dialog (PaintEditorWindow *editor, GtkFileDialog *dialog)
{
	GListModel *filters;
	filters = paint_file_filter_list_new ();

	if (filters)
	{
		gtk_file_dialog_set_filters (dialog, filters);
		g_object_unref (filters);
	}
	if (editor->file)
	{
		gtk_file_dialog_set_initial_file (dialog, editor->file);
	}
}

/*******************************************************************************
 * @brief 現在のウィンドウを初期化する。
 ******************************************************************************/
void
paint_editor_window_init_window (GtkWindow *window)
{
	gtk_window_set_default_size (window, 640, 480);
	gtk_window_set_title (window, TEXT_TITLE);
}

/*******************************************************************************
 * @brief 指定したファイルを開く。
 * 画像ファイルが開いた場合は現在のサーフィスを更新する。
 * @param file 開くファイル。
 ******************************************************************************/
void
paint_editor_window_load_surface (PaintEditorWindow *editor, GFile *file, cairo_t *context)
{
	GError *error;
	cairo_surface_t *surface;
	error = NULL;
	surface = paint_surface_new_from_file (file, context, &error);

	if (surface)
	{
		paint_editor_window_set_surface (editor, surface);
		paint_editor_window_update_content (editor);
		paint_editor_window_update_canvas (editor);
		cairo_surface_destroy (surface);
	}
	if (error)
	{
		paint_editor_window_show_error_dialog (editor, error);
		g_clear_error (&error);
	}
}

/*******************************************************************************
 * @brief クラスのインスタンスを作成する。
 * @param application ウィンドウが属するアプリケーション。
 * @return 作成したインスタンスを返す。
 ******************************************************************************/
GtkWidget *
paint_editor_window_new (GApplication *application)
{
	return g_object_new (PAINT_TYPE_EDITOR_WINDOW,
		"application", application,
		"show-menubar", TRUE,
		NULL);
}

/*******************************************************************************
 * @brief ダイアログ ウィンドウを表示します。
 * @param parent 親ウィンドウ。
 * @param modal モーダル ウィンドウ。
 ******************************************************************************/
void
paint_editor_window_present_dialog (GtkWindow *parent, GtkWindow *modal)
{
	gtk_window_set_destroy_with_parent (modal, TRUE);
	gtk_window_set_modal               (modal, TRUE);
	gtk_window_set_transient_for       (modal, parent);
	gtk_window_present                 (modal);
}

/*******************************************************************************
 * @brief ファイルを保存するかどうかを選択した。
 * @param object Alert Dialog クラスのインスタンス。
 ******************************************************************************/
void
paint_editor_window_respond_closing (GObject *object, GAsyncResult *result, gpointer self)
{
	int response;
	response = gtk_alert_dialog_choose_finish (GTK_ALERT_DIALOG (object), result, NULL);

	switch (response)
	{
	case PAINT_CLOSING_RESPONSE_CANCEL:
		break;
	case PAINT_CLOSING_RESPONSE_DESTROY:
		g_idle_add_once ((GSourceOnceFunc) gtk_window_destroy, self);
		break;
	case PAINT_CLOSING_RESPONSE_SAVE:
		paint_editor_window_show_save_dialog (PAINT_EDITOR_WINDOW (self));
		break;
	default:
		g_assert_not_reached ();
		break;
	}
}

/*******************************************************************************
 * @brief 開くファイルを選択した。
 * @param object File Dialog クラスのインスタンス。
 ******************************************************************************/
void
paint_editor_window_respond_open (GObject *object, GAsyncResult *result, gpointer self)
{
	GFile *file;
	PaintEditorWindow *editor;
	file = gtk_file_dialog_open_finish (GTK_FILE_DIALOG (object), result, NULL);

	if (file)
	{
		editor = PAINT_EDITOR_WINDOW (self);
		editor->surface_loading = TRUE;
		paint_editor_window_set_file (editor, file);
		paint_editor_window_update_name (editor);
		paint_editor_window_update_title (editor);
		gtk_widget_queue_draw (GTK_WIDGET (editor->canvas));
		g_object_unref (file);
	}
}

/*******************************************************************************
 * @brief 保存するファイルを選択した。
 * @param object File Dialog クラスのインスタンス。
 ******************************************************************************/
void
paint_editor_window_respond_save (GObject *object, GAsyncResult *result, gpointer self)
{
	GFile *file;
	file = gtk_file_dialog_save_finish (GTK_FILE_DIALOG (object), result, NULL);

	if (file)
	{
		g_object_unref (file);
	}
}

/*******************************************************************************
 * @brief 現在のファイルの参照数を減らす。指定したファイルの参照数を増やす。
 ******************************************************************************/
void
paint_editor_window_set_file (PaintEditorWindow *editor, GFile *file)
{
	if (editor->file)
	{
		g_object_unref (editor->file);
	}
	if (file)
	{
		editor->file = g_object_ref (file);
	}
	else
	{
		editor->file = NULL;
	}
}

/*******************************************************************************
 * @brief ドキュメントの名前を設定する。
 ******************************************************************************/
void
paint_editor_window_set_name (PaintEditorWindow *editor, const char *name)
{
	char *buffer;
	size_t size;

	if (name)
	{
		size = strlen (name) + 1;

		if (editor->name)
		{
			buffer = g_realloc (editor->name, size);
		}
		else
		{
			buffer = g_malloc (size);
		}
		if (buffer)
		{
			memcpy (buffer, name, size);
			editor->name = buffer;
		}
	}
	else if (editor->name)
	{
		g_free (editor->name);
		editor->name = NULL;
	}
}

/*******************************************************************************
 * @brief 現在のサーフィスの参照数を減らす。指定したサーフィスの参照数を増やす。
 ******************************************************************************/
void
paint_editor_window_set_surface (PaintEditorWindow *editor, cairo_surface_t *surface)
{
	if (editor->surface != surface)
	{
		if (editor->surface)
		{
			cairo_surface_destroy (editor->surface);
		}
		if (surface)
		{
			editor->surface = cairo_surface_reference (surface);
		}
		else
		{
			editor->surface = NULL;
		}
	}
}

/*******************************************************************************
 * @brief バージョン情報ダイアログ ボックスを表示する。
 ******************************************************************************/
void
paint_editor_window_show_about_dialog (PaintEditorWindow *editor)
{
	GtkWidget *dialog;
	dialog = gtk_about_dialog_new ();

	if (dialog)
	{
		g_signal_connect_swapped (dialog, "destroy", G_CALLBACK (gtk_window_destroy), dialog);
		paint_editor_window_init_about_dialog (GTK_ABOUT_DIALOG (dialog));
		paint_editor_window_present_dialog (GTK_WINDOW (editor), GTK_WINDOW (dialog));
	}
}

/*******************************************************************************
 * @brief ドキュメントを閉じるダイアログ ボックスを表示する。
 ******************************************************************************/
void
paint_editor_window_show_closing_dialog (PaintEditorWindow *editor)
{
	const char *labels [PAINT_CLOSING_RESPONSE_MAX];
	GtkAlertDialog *dialog;
	dialog = gtk_alert_dialog_new ("Alert Dialog");

	if (dialog)
	{
		labels [PAINT_CLOSING_RESPONSE_CANCEL]  = "_Cancel";
		labels [PAINT_CLOSING_RESPONSE_DESTROY] = "_Destroy";
		labels [PAINT_CLOSING_RESPONSE_SAVE]    = "_Save";
		gtk_alert_dialog_set_buttons        (dialog, labels);
		gtk_alert_dialog_set_cancel_button  (dialog, PAINT_CLOSING_RESPONSE_CANCEL);
		gtk_alert_dialog_set_default_button (dialog, PAINT_CLOSING_RESPONSE_SAVE);
		gtk_alert_dialog_set_detail         (dialog, "Details");
		gtk_alert_dialog_set_message        (dialog, "Message");
		gtk_alert_dialog_set_modal          (dialog, TRUE);
		gtk_alert_dialog_choose             (dialog, GTK_WINDOW (editor), NULL, paint_editor_window_respond_closing, editor);
		g_object_unref                      (dialog);
	}
}

/*******************************************************************************
 * @brief 指定したエラーを説明する。
 ******************************************************************************/
void
paint_editor_window_show_error_dialog (PaintEditorWindow *editor, GError *error)
{
	GtkAlertDialog *dialog;
	dialog = gtk_alert_dialog_new ("%s", error->message);

	if (dialog)
	{
		gtk_alert_dialog_set_modal (dialog, TRUE);
		gtk_alert_dialog_show (dialog, GTK_WINDOW (editor));
		g_object_unref (dialog);
	}
}

/*******************************************************************************
 * @brief ファイルを開くダイアログ ボックスを表示する。
 ******************************************************************************/
void
paint_editor_window_show_open_dialog (PaintEditorWindow *editor)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		paint_editor_window_init_file_dialog (editor, dialog);
		gtk_file_dialog_set_title (dialog, "Open File");
		gtk_file_dialog_open (dialog, GTK_WINDOW (editor), NULL, paint_editor_window_respond_open, editor);
		g_object_unref (dialog);
	}
}

/*******************************************************************************
 * @brief ファイルを保存ダイアログ ボックスを表示する。
 ******************************************************************************/
void
paint_editor_window_show_save_dialog (PaintEditorWindow *editor)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		paint_editor_window_init_file_dialog (editor, dialog);
		gtk_file_dialog_set_title (dialog, "Save File");
		gtk_file_dialog_save (dialog, GTK_WINDOW (editor), NULL, paint_editor_window_respond_save, editor);
		g_object_unref (dialog);
	}
}

/*******************************************************************************
 * @brief キャンバスの大きさに拡大率を掛ける。
 ******************************************************************************/
void
paint_editor_window_update_canvas (PaintEditorWindow *editor)
{
	GtkDrawingArea *canvas;
	int value;

	if (editor->canvas)
	{
		canvas = GTK_DRAWING_AREA (editor->canvas);
		value = muldiv (editor->content_width, editor->zoom, PAINT_ZOOM_DEFAULT_VALUE);
		gtk_drawing_area_set_content_width (canvas, value);
		value = muldiv (editor->content_height, editor->zoom, PAINT_ZOOM_DEFAULT_VALUE);
		gtk_drawing_area_set_content_height (canvas, value);
	}
}

/*******************************************************************************
 * @brief サーフィスの大きさでキャンバスの大きさを更新する。
 ******************************************************************************/
void
paint_editor_window_update_content (PaintEditorWindow *editor)
{
	cairo_surface_t *image;
	int value;

	if (editor->surface)
	{
		image = cairo_surface_map_to_image (editor->surface, NULL);
		value = cairo_image_surface_get_width (image);

		if (editor->content_width < value)
		{
			editor->content_width = value;
		}

		value = cairo_image_surface_get_height (image);

		if (editor->content_height < value)
		{
			editor->content_height = value;
		}

		cairo_surface_unmap_image (editor->surface, image);
	}
}

/*******************************************************************************
 * @brief 現在のファイルでドキュメントの名前を更新する。
 ******************************************************************************/
void
paint_editor_window_update_name (PaintEditorWindow *editor)
{
	char *name;

	if (editor->file)
	{
		name = g_file_get_basename (editor->file);

		if (name)
		{
			paint_editor_window_set_name (editor, name);
			g_free (name);
		}
	}
}

/*******************************************************************************
 * @brief キュメントの名前でウィンドウ タイトルを更新する。
 ******************************************************************************/
void
paint_editor_window_update_title (PaintEditorWindow *editor)
{
	char title [PAINT_EDITOR_WINDOW_TITLE_CCH];

	if (editor->name)
	{
		snprintf (title, PAINT_EDITOR_WINDOW_TITLE_CCH, PAINT_EDITOR_WINDOW_TITLE_FORMAT, editor->name, TEXT_TITLE);
		gtk_window_set_title (GTK_WINDOW (editor), title);
	}
}
