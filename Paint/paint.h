/**
	@file paint.h
	@author Taichi Murakami
	@date 2025
	@brief アプリケーションを宣言します。
*/
#include <gtk/gtk.h>

/** レイヤーの可視性 */
enum _PaintLayerVisibility
{
	/** 表示可編集可 */
	PAINT_LAYER_VISIBILITY_EDIT,
	/** 表示可 */
	PAINT_LAYER_VISIBILITY_VISIBLE,
	/** 非表示 */
	PAINT_LAYER_VISIBILITY_HIDDEN,
};

/** Paint Application クラス */
struct _PaintApplicationClass
{
	GtkApplicationClass parent_class;
};

/** Paint Application Window クラス */
struct _PaintApplicationWindowClass
{
	GtkApplicationWindowClass parent_class;
};

extern const char *PAINT_APPLICATION_ID;
extern const char *PAINT_APPLICATION_WINDOW_CLASS_TEMPLATE;
extern const char *PAINT_RESOURCE_PATH_DIALOG;
extern const char *PAINT_RESOURCE_PATH_MENUS;

/** Paint Layer Visibility 列挙体 */
GType paint_layer_visibility_get_type (void);
#define PAINT_TYPE_LAYER_VISIBILITY paint_layer_visibility_get_type ()

/**
	@class PaintApplication
	@extends GtkApplication
	@brief アプリケーションを表します。
*/
G_DECLARE_DERIVABLE_TYPE (PaintApplication, paint_application, PAINT, APPLICATION, GtkApplication);
#define PAINT_TYPE_APPLICATION paint_application_get_type ()

/**
	@class PaintApplicationWindow
	@extends GtkApplicationWindow
	@brief アプリケーションが最初に表示するウィンドウを表します。
*/
G_DECLARE_DERIVABLE_TYPE (PaintApplicationWindow, paint_application_window, PAINT, APPLICATION_WINDOW, GtkApplicationWindow);
#define PAINT_TYPE_APPLICATION_WINDOW paint_application_window_get_type ()

/** @name Paint Application クラス */
/** @{ */
GApplication *paint_application_new (void);

/** @} */
/** @name Paint Application Window クラス */
/** @{ */
GtkWidget *paint_application_window_new (GApplication *application);

/** @} */
