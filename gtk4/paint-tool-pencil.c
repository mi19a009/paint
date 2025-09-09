/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define PAINT_TOOL_PENCIL_ICON_NAME "pencil-symbolic"
#define PAINT_TOOL_PENCIL_NAME      "pencil"
#define PAINT_TOOL_PENCIL_NICK      "Pencil"

/* Paint Tool Pencil クラスのインスタンス */
struct _PaintToolPencil
{
	PaintToolClass parent_class;
};

static void paint_tool_pencil_class_init      (PaintToolPencilClass *this_class);
static void paint_tool_pencil_class_init_tool (PaintToolClass *this_class);
static void paint_tool_pencil_init            (PaintToolPencil *self);

/*******************************************************************************
Paint Tool Pencil クラス:
えんぴつツールを表します。
*/ G_DEFINE_FINAL_TYPE (PaintToolPencil, paint_tool_pencil, PAINT_TYPE_TOOL);

/*******************************************************************************
クラスを初期化します。
*/ static void
paint_tool_pencil_class_init (PaintToolPencilClass *this_class)
{
	paint_tool_pencil_class_init_tool (PAINT_TOOL_CLASS (this_class));
}

/*******************************************************************************
Tool クラスを初期化します。
*/ static void
paint_tool_pencil_class_init_tool (PaintToolClass *this_class)
{
	this_class->icon_name = PAINT_TOOL_PENCIL_ICON_NAME;
	this_class->name = PAINT_TOOL_PENCIL_NAME;
	this_class->nick = PAINT_TOOL_PENCIL_NICK;
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/ static void
paint_tool_pencil_init (PaintToolPencil *self)
{
}

/*******************************************************************************
クラスのインスタンスを作成します。
*/ PaintTool *
paint_tool_pencil_new (void)
{
	return g_object_new (PAINT_TYPE_TOOL_PENCIL, NULL);
}
