/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "text.h"
#define PAINT_EDITOR_WINDOW_CCH_TITLE 256
#define PAINT_EDITOR_WINDOW_FORMAT_TITLE "%s - %s"

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
	GFile               *file;
	GtkDrawingArea      *canvas;
	GtkScrolledWindow   *client;
	PaintLayer          *layer;
	guint8               closing;
	guint8               modified;
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
void
paint_editor_window_init_canvas (PaintEditorWindow *);
static void
paint_editor_window_init_client (PaintEditorWindow *);
static void
paint_editor_window_init_file_dialog (PaintEditorWindow *, GtkFileDialog *);
static void
paint_editor_window_init_layer (PaintEditorWindow *editor);
static void
paint_editor_window_init_window (GtkWindow *);
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
paint_editor_window_show_about_dialog (PaintEditorWindow *);
static void
paint_editor_window_show_closing_dialog (PaintEditorWindow *);
static void
paint_editor_window_show_open_dialog (PaintEditorWindow *);
static void
paint_editor_window_show_save_dialog (PaintEditorWindow *);
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
	g_clear_object  (&editor->file);
	g_clear_object  (&editor->layer);
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

void
paint_editor_window_draw_canvas (GtkDrawingArea *, cairo_t *cairo, int, int, gpointer self)
{
	PaintEditorWindow *editor;
	editor = PAINT_EDITOR_WINDOW (self);
	cairo_set_source_surface (cairo, paint_layer_get_surface (editor->layer), 0, 0);
	cairo_paint (cairo);
}

/*******************************************************************************
 * @brief 現在のファイルを取得する。
 ******************************************************************************/
GFile *
paint_editor_window_get_file (PaintEditorWindow *editor)
{
	return g_object_ref (editor->file);
}

/*******************************************************************************
 * @brief クラスのインスタンスを初期化する。
 ******************************************************************************/
void
paint_editor_window_init (PaintEditorWindow *editor)
{
	paint_editor_window_init_actions (G_ACTION_MAP (editor));
	paint_editor_window_init_window  (GTK_WINDOW (editor));
	paint_editor_window_init_client  (editor);
	paint_editor_window_init_canvas  (editor);
	paint_editor_window_init_layer   (editor);
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
	GtkWidget *canvas;

	if (!editor->canvas && editor->client)
	{
		canvas = gtk_drawing_area_new ();

		if (canvas)
		{
			editor->canvas = GTK_DRAWING_AREA (canvas);
			gtk_drawing_area_set_draw_func (editor->canvas, paint_editor_window_draw_canvas, editor, NULL);
			gtk_scrolled_window_set_child (editor->client, canvas);
		}
	}
}

/*******************************************************************************
 * @brief スルロール可能ウィンドウを作成する。
 ******************************************************************************/
void
paint_editor_window_init_client (PaintEditorWindow *editor)
{
	GtkWidget *client;

	if (!editor->client)
	{
		client = gtk_scrolled_window_new ();

		if (client)
		{
			editor->client = GTK_SCROLLED_WINDOW (client);
			gtk_window_set_child (GTK_WINDOW (editor), client);
		}
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
 * @brief レイヤーを初期化する。
 ******************************************************************************/
void
paint_editor_window_init_layer (PaintEditorWindow *editor)
{
	GtkDrawingArea *canvas;
	int width, height, n_planes;
	width = 16;
	height = 16;
	n_planes = 4;
	canvas = GTK_DRAWING_AREA (editor->canvas);
	editor->layer = paint_layer_new (width, height, n_planes);
	gtk_drawing_area_set_content_width (canvas, width);
	gtk_drawing_area_set_content_height (canvas, height);
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

void
paint_editor_window_load (PaintEditorWindow *editor, GFile *file)
{
	if (editor->layer)
	{
		g_object_unref (editor->layer);
	}

	editor->layer = paint_layer_new_from_file (file, 0);
	gtk_widget_queue_draw (GTK_WIDGET (editor->canvas));
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
		paint_editor_window_set_file (editor, file);
		paint_editor_window_load (editor, file);
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
 * @brief 現在のファイルを設定する。
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

	paint_editor_window_update_title (editor);
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
		gtk_alert_dialog_set_detail         (dialog, "Details");;
		gtk_alert_dialog_set_message        (dialog, "Message");
		gtk_alert_dialog_set_modal          (dialog, TRUE);
		gtk_alert_dialog_choose             (dialog, GTK_WINDOW (editor), NULL, paint_editor_window_respond_closing, editor);
		g_object_unref                      (dialog);
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
 * @brief ウィンドウ タイトルを更新する。
 ******************************************************************************/
void
paint_editor_window_update_title (PaintEditorWindow *editor)
{
	char *name;
	char title [PAINT_EDITOR_WINDOW_CCH_TITLE];

	if (editor->file)
	{
		name = g_file_get_basename (editor->file);

		if (name)
		{
			snprintf (title, PAINT_EDITOR_WINDOW_CCH_TITLE, PAINT_EDITOR_WINDOW_FORMAT_TITLE, name, TEXT_TITLE);
			g_free (name);
			gtk_window_set_title (GTK_WINDOW (editor), title);
		}
	}
}
