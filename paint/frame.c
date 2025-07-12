/* Copyright (C) 2025 Taichi Murakami.
フレーム ウィンドウ クラス。
現在のアプリケーションを起動した際に表示されるウィンドウ。
このクラスはドキュメント ウィンドウ、ツールバー、およびステータス バーを所有する。
*/
#include <gtk/gtk.h>
#include "paint.h"
#include "text.h"
#define PAINT_FRAME_DEFAULT_HEIGHT 480
#define PAINT_FRAME_DEFAULT_WIDTH 640

typedef void (*PaintFrameActivate) (GSimpleAction *, GVariant *, gpointer);
typedef struct _PaintFrameActionEntry PaintFrameActionEntry;

/* フレーム ウィンドウ クラスのプロパティ */
enum _PaintFrameProperty
{
	PAINT_FRAME_PROPERTY_0,
};

/* フレーム ウィンドウ クラスのインスタンス */
struct _PaintFrame
{
	GtkApplicationWindow parent_instance;
	GtkWidget           *content;
	GtkWidget           *document;
	GtkWidget           *statusbar;
	GtkWidget           *toolbar;
	GtkWidget           *viewport;
};

/* メニュー */
struct _PaintFrameActionEntry
{
	const char *name;
	PaintFrameActivate activate;
};

static void
paint_frame_activate_about (GSimpleAction *, GVariant *, gpointer);
static void
paint_frame_activate_open (GSimpleAction *, GVariant *, gpointer);
static void
paint_frame_activate_save (GSimpleAction *, GVariant *, gpointer);
static void
paint_frame_activate_save_as (GSimpleAction *, GVariant *, gpointer);
static void
paint_frame_class_init (PaintFrameClass *);
static void
paint_frame_class_init_object (GObjectClass *);
static void
paint_frame_class_init_window (GtkWindowClass *);
static gboolean
paint_frame_close (GtkWindow *);
static void
paint_frame_destroy (PaintFrame *);
static void
paint_frame_dispose (GObject *);
static void
paint_frame_init (PaintFrame *);
static void
paint_frame_init_action (GActionMap *);
static void
paint_frame_init_content (PaintFrame *);
static void
paint_frame_init_document (PaintFrame *);
static void
paint_frame_init_viewport (PaintFrame *);
static void
paint_frame_respond_open (GObject *, GAsyncResult *, gpointer);
static void
paint_frame_respond_save (GObject *, GAsyncResult *, gpointer);

/* メニュー */
static const PaintFrameActionEntry
paint_frame_action_entries [] =
{
	{ "about",   paint_frame_activate_about   },
	{ "open",    paint_frame_activate_open    },
	{ "save",    paint_frame_activate_save    },
	{ "save-as", paint_frame_activate_save_as },
};

G_DEFINE_FINAL_TYPE (PaintFrame, paint_frame, GTK_TYPE_APPLICATION_WINDOW);
#define PAINT_FRAME_ACTION_N_ENTRIES G_N_ELEMENTS (paint_frame_action_entries)

void
paint_frame_activate_about (GSimpleAction *, GVariant *, gpointer self)
{
	paint_about_dialog_show (GTK_WINDOW (self));
}

void
paint_frame_activate_open (GSimpleAction *, GVariant *, gpointer self)
{
	paint_file_dialog_open (GTK_WINDOW (self), NULL, paint_frame_respond_open, self);
}

void
paint_frame_activate_save (GSimpleAction *, GVariant *, gpointer self)
{
	paint_file_dialog_save (GTK_WINDOW (self), NULL, paint_frame_respond_save, self);
}

void
paint_frame_activate_save_as (GSimpleAction *, GVariant *, gpointer)
{
}

/*******************************************************************************
 * @brief フレーム ウィンドウ クラスを初期化する。
 ******************************************************************************/
void
paint_frame_class_init (PaintFrameClass *frame)
{
	paint_frame_class_init_object (G_OBJECT_CLASS (frame));
	paint_frame_class_init_window (GTK_WINDOW_CLASS (frame));
}

/*******************************************************************************
 * @brief オブジェクト クラスを初期化する。
 ******************************************************************************/
void
paint_frame_class_init_object (GObjectClass *object)
{
	object->dispose = paint_frame_dispose;
}

/*******************************************************************************
 * @brief ウィンドウ クラスを初期化する。
 ******************************************************************************/
void
paint_frame_class_init_window (GtkWindowClass *window)
{
	window->close_request = paint_frame_close;
}

/*******************************************************************************
 * @brief 現在のウィンドウを閉じる。
 * @return ウィンドウを閉じる場合は FALSE を返す。
 ******************************************************************************/
gboolean
paint_frame_close (GtkWindow *window)
{
	return FALSE;
}

void
paint_frame_destroy (PaintFrame *)
{
}

/*******************************************************************************
 * @brief インスタンスを破棄する。
 ******************************************************************************/
void
paint_frame_dispose (GObject *object)
{
	paint_frame_destroy (PAINT_FRAME (object));
	G_OBJECT_CLASS (paint_frame_parent_class)->dispose (object);
}

/*******************************************************************************
 * @brief クラスのインスタンスを初期化する。
 ******************************************************************************/
void
paint_frame_init (PaintFrame *frame)
{
	paint_frame_init_action (G_ACTION_MAP (frame));
	paint_frame_init_content (frame);
	paint_frame_init_viewport (frame);
	paint_frame_init_document (frame);
}

/*******************************************************************************
 * @brief メニュー項目のコールバック関数を登録する。
 ******************************************************************************/
void
paint_frame_init_action (GActionMap *actions)
{
	const PaintFrameActionEntry *entries;
	GSimpleAction *action;
	guint index;
	entries = paint_frame_action_entries;

	for (index = 0; index < PAINT_FRAME_ACTION_N_ENTRIES; index++)
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
 * @brief ボックスを作成する。
 ******************************************************************************/
void
paint_frame_init_content (PaintFrame *frame)
{
	frame->content = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	if (frame->content)
	{
		gtk_window_set_child (GTK_WINDOW (frame), frame->content);
	}
}

/*******************************************************************************
 * @brief ドキュメント ウィンドウを作成する。
 ******************************************************************************/
void
paint_frame_init_document (PaintFrame *frame)
{
	GtkScrollable *scrollable;

	if (frame->viewport)
	{
		scrollable = GTK_SCROLLABLE (frame->viewport);
		frame->document = paint_document_new (gtk_scrollable_get_hadjustment (scrollable), gtk_scrollable_get_vadjustment (scrollable));

		if (frame->document)
		{
			gtk_viewport_set_child (GTK_VIEWPORT (frame->viewport), frame->document);
		}
	}
}

/*******************************************************************************
 * @brief スクロール可能なウィンドウを作成する。
 ******************************************************************************/
void
paint_frame_init_viewport (PaintFrame *frame)
{
	if (frame->content)
	{
		frame->viewport = gtk_viewport_new (NULL, NULL);

		if (frame->viewport)
		{
			gtk_box_append (GTK_BOX (frame->content), frame->viewport);
		}
	}
}

/*******************************************************************************
 * @brief クラスのインスタンスを作成する。
 * @param application ウィンドウが属するアプリケーション。
 ******************************************************************************/
GtkWidget *
paint_frame_new (GApplication *application)
{
	return g_object_new (PAINT_TYPE_FRAME,
		"application",    application,
		"default-height", PAINT_FRAME_DEFAULT_HEIGHT,
		"default-width",  PAINT_FRAME_DEFAULT_WIDTH,
		"show-menubar",   TRUE,
		"title",          TEXT_TITLE,
		NULL);
}

void
paint_frame_respond_open (GObject *, GAsyncResult *, gpointer)
{
}

void
paint_frame_respond_save (GObject *, GAsyncResult *, gpointer)
{
}
