/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "text.h"

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
	PaintLayer *layer;
	GtkDrawingArea *canvas;
	GtkScrolledWindow *client;
};

static void
paint_editor_window_activate_about (GSimpleAction *, GVariant *, gpointer);
static void
paint_editor_window_activate_open (GSimpleAction *, GVariant *, gpointer);
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
paint_editor_window_init_actions (GActionMap *);
void
paint_editor_window_init_canvas (PaintEditorWindow *);
static void
paint_editor_window_init_client (PaintEditorWindow *);
static void
paint_editor_window_init_layer (PaintEditorWindow *editor);
static void
paint_editor_window_init_window (GtkWindow *);
static void
paint_editor_window_respond_open (GObject *, GAsyncResult *, gpointer);

G_DEFINE_FINAL_TYPE (PaintEditorWindow, paint_editor_window, GTK_TYPE_APPLICATION_WINDOW);

/* メニュー */
static const PaintEditorActionEntry paint_editor_action_entries [] =
{
	{ "about", paint_editor_window_activate_about },
	{ "open",  paint_editor_window_activate_open  },
};

/*******************************************************************************
 * @brief メニュー: バージョン情報。
 ******************************************************************************/
void
paint_editor_window_activate_about (GSimpleAction *, GVariant *, gpointer self)
{
	paint_about_dialog_show (GTK_WINDOW (self));
}

/*******************************************************************************
 * @brief メニュー: ファイルを開く。
 ******************************************************************************/
void
paint_editor_window_activate_open (GSimpleAction *, GVariant *, gpointer self)
{
	paint_file_dialog_show_open (GTK_WINDOW (self), paint_editor_window_respond_open, self);
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
	return GTK_WINDOW_CLASS (paint_editor_window_parent_class)->close_request (window);
}

/*******************************************************************************
 * @brief メンバー変数を破棄する。
 ******************************************************************************/
void
paint_editor_window_destroy (PaintEditorWindow *editor)
{
	g_clear_object (&editor->layer);
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
paint_editor_window_draw_canvas (GtkDrawingArea *, cairo_t *, int, int, gpointer)
{
}

/*******************************************************************************
 * @brief クラスのインスタンスを初期化する。
 ******************************************************************************/
void
paint_editor_window_init (PaintEditorWindow *editor)
{
	paint_editor_window_init_actions (G_ACTION_MAP (editor));
	paint_editor_window_init_window (GTK_WINDOW (editor));
	paint_editor_window_init_client (editor);
	paint_editor_window_init_canvas (editor);
	paint_editor_window_init_layer (editor);
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

void
paint_editor_window_respond_open (GObject *object, GAsyncResult *result, gpointer self)
{
	GFile *file;
	char *path;
	file = gtk_file_dialog_open_finish (GTK_FILE_DIALOG (object), result, NULL);

	if (file)
	{
		path = g_file_get_path (file);

		if (path)
		{
			g_free (path);
		}

		g_object_unref (file);
	}
}
