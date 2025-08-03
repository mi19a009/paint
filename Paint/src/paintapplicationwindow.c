/**
	@file paintapplicationwindow.c
	@author Taichi Murakami
	@date 2025
	@brief Paint Application Window クラスを定義します。
*/
#include "paint.h"
#define PAINT_APPLICATION_WINDOW_DEFAULT_HEIGHT 480
#define PAINT_APPLICATION_WINDOW_DEFAULT_WIDTH 640

static void paint_application_window_class_init        (PaintApplicationWindowClass *);
static void paint_application_window_class_init_object (GObjectClass *);
static void paint_application_window_class_init_widget (GtkWidgetClass *);
static void paint_application_window_dispose           (GObject *);
static void paint_application_window_init              (PaintApplicationWindow *);
static void paint_application_window_on_constructed    (GObject *);
static void paint_application_window_on_size_allocate  (GtkWidget *, int, int, int);
static void paint_application_window_realize           (GtkWidget *);
static void paint_application_window_unrealize         (GtkWidget *);

/** Paint Application Window クラス */
G_DEFINE_TYPE (PaintApplicationWindow, paint_application_window, GTK_TYPE_APPLICATION_WINDOW);

/**
	クラスを初期化します。
	@memberof PaintApplicationWindow
*/
void paint_application_window_class_init (PaintApplicationWindowClass *window)
{
	paint_application_window_class_init_object (G_OBJECT_CLASS (window));
	paint_application_window_class_init_widget (GTK_WIDGET_CLASS (window));
}

/**
	Object クラスを初期化します。
	@memberof PaintApplicationWindow
*/
void paint_application_window_class_init_object (GObjectClass *object)
{
	object->constructed = paint_application_window_on_constructed;
	object->dispose = paint_application_window_dispose;
}

/**
	Widget クラスを初期化します。
	@memberof PaintApplicationWindow
*/
void paint_application_window_class_init_widget (GtkWidgetClass *widget)
{
	widget->size_allocate = paint_application_window_on_size_allocate;
	widget->realize = paint_application_window_realize;
	widget->unrealize = paint_application_window_unrealize;
	gtk_widget_class_set_template_from_resource (widget, PAINT_APPLICATION_WINDOW_CLASS_TEMPLATE);
}

/**
	オブジェクトを破棄します。
	@memberof PaintApplicationWindow
*/
void paint_application_window_dispose (GObject *object)
{
	gtk_widget_dispose_template (GTK_WIDGET (object), PAINT_TYPE_APPLICATION_WINDOW);
	G_OBJECT_CLASS (paint_application_window_parent_class)->dispose (object);
}

/**
	クラスのインスタンスを初期化します。
	@memberof PaintApplicationWindow
*/
void paint_application_window_init (PaintApplicationWindow *window)
{
	gtk_widget_init_template (GTK_WIDGET (window));
}

/**
	クラスのインスタンスを作成します。
	@param application ウィンドウが属するアプリケーション。
	@memberof PaintApplicationWindow
*/
GtkWidget *paint_application_window_new (GApplication *application)
{
	return g_object_new (PAINT_TYPE_APPLICATION_WINDOW,
		"application", application,
		"show-menubar", TRUE,
		NULL);
}

/**
	オブジェクトを作成します。
	@memberof PaintApplicationWindow
*/
void paint_application_window_on_constructed (GObject *object)
{
	G_OBJECT_CLASS (paint_application_window_parent_class)->constructed (object);
}

/**
	ウィジェットの大きさを変更します。
	@param width ウィジェットの幅。
	@param height ウィジェットの高さ。
	@param baseline ウィジェットの位置。
	@memberof PaintApplicationWindow
*/
void paint_application_window_on_size_allocate (GtkWidget *widget, int width, int height, int baseline)
{
	GTK_WIDGET_CLASS (paint_application_window_parent_class)->size_allocate (widget, width, height, baseline);
}

/**
	ウィジェットを表示します。
	@memberof PaintApplicationWindow
*/
void paint_application_window_realize (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (paint_application_window_parent_class)->realize (widget);
}

/**
	ウィジェットを消去します。
	@memberof PaintApplicationWindow
*/
void paint_application_window_unrealize (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (paint_application_window_parent_class)->unrealize (widget);
}

