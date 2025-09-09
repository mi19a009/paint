/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define PAINT_TOOL_ERASER_ICON_NAME "eraser-symbolic"
#define PAINT_TOOL_ERASER_NAME      "eraser"
#define PAINT_TOOL_ERASER_NICK      "Eraser"

/* Paint Tool Eraser クラスのインスタンス */
struct _PaintToolEraser
{
	PaintToolClass parent_class;
};

static void paint_tool_eraser_class_init      (PaintToolEraserClass *this_class);
static void paint_tool_eraser_class_init_tool (PaintToolClass *this_class);
static void paint_tool_eraser_init            (PaintToolEraser *self);

/*******************************************************************************
Paint Tool Eraser クラス:
えんぴつツールを表します。
*/ G_DEFINE_FINAL_TYPE (PaintToolEraser, paint_tool_eraser, PAINT_TYPE_TOOL);

/*******************************************************************************
クラスを初期化します。
*/ static void
paint_tool_eraser_class_init (PaintToolEraserClass *this_class)
{
	paint_tool_eraser_class_init_tool (PAINT_TOOL_CLASS (this_class));
}

/*******************************************************************************
Tool クラスを初期化します。
*/ static void
paint_tool_eraser_class_init_tool (PaintToolClass *this_class)
{
	this_class->icon_name = PAINT_TOOL_ERASER_ICON_NAME;
	this_class->name = PAINT_TOOL_ERASER_NAME;
	this_class->nick = PAINT_TOOL_ERASER_NICK;
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/ static void
paint_tool_eraser_init (PaintToolEraser *self)
{
}

/*******************************************************************************
クラスのインスタンスを作成します。
*/ PaintTool *
paint_tool_eraser_new (void)
{
	return g_object_new (PAINT_TYPE_TOOL_ERASER, NULL);
}
