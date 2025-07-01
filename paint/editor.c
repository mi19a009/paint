/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "text.h"

/* クラスのインスタンス */
struct _PaintEditorWindow
{
	GtkApplicationWindow parent_instance;
};

static void
paint_editor_window_class_init (PaintEditorWindowClass *);
static gboolean
paint_editor_window_close (GtkWindow *);
static void
paint_editor_window_init (PaintEditorWindow *);
static void
paint_editor_window_init_actions (GActionMap *);
static void
paint_editor_window_init_window (GtkWindow *);

G_DEFINE_FINAL_TYPE (PaintEditorWindow, paint_editor_window, GTK_TYPE_APPLICATION_WINDOW);

/*******************************************************************************
 * @brief クラスのコールバック関数を登録する。
 ******************************************************************************/
void
paint_editor_window_class_init (PaintEditorWindowClass *editor)
{
	GtkWindowClass *window;
	window = GTK_WINDOW_CLASS (editor);
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
 * @brief クラスのインスタンスを初期化する。
 ******************************************************************************/
void
paint_editor_window_init (PaintEditorWindow *editor)
{
	paint_editor_window_init_actions (G_ACTION_MAP (editor));
	paint_editor_window_init_window (GTK_WINDOW (editor));
}

/*******************************************************************************
 * @brief メニュー項目のコールバック関数を登録する。
 ******************************************************************************/
void
paint_editor_window_init_actions (GActionMap *actions)
{
	GSimpleAction *action;
	action = g_simple_action_new ("about", NULL);

	if (action)
	{
		g_signal_connect (action, "activate", G_CALLBACK (paint_activate_about), actions);
		g_action_map_add_action (actions, G_ACTION (action));
		g_object_unref (action);
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
