/* Copyright (C) 2025 Taichi Murakami. */
#include "paint.h"
#include "text.h"
#define PAINT_TOOLBAR_PROPERTY_ACTIONS_NAME          "actions"
#define PAINT_TOOLBAR_PROPERTY_ACTIONS_NICK          "Actions"
#define PAINT_TOOLBAR_PROPERTY_ACTIONS_BLURB         "Actions"
#define PAINT_TOOLBAR_PROPERTY_ACTIONS_DEFAULT_VALUE NULL
#define PAINT_TOOLBAR_PROPERTY_ACTIONS_FLAGS         G_PARAM_READWRITE | G_PARAM_CONSTRUCT

/* ツールバー クラスのプロパティ */
enum _PaintToolbarProperty
{
	PAINT_TOOLBAR_PROPERTY_0,
	PAINT_TOOLBAR_PROPERTY_ACTIONS,
	PAINT_TOOLBAR_N_PROPERTIES,
};

/* ツールバー クラスのインスタンス */
struct _PaintToolbar
{
	GtkBox        parent_instance;
	GActionGroup *actions;
};

static void
paint_toolbar_class_init (PaintToolbarClass *);
static void
paint_toolbar_class_init_object (GObjectClass *);
static void
paint_toolbar_click_button (GtkButton *, gpointer);
static void
paint_toolbar_destroy (PaintToolbar *);
static void
paint_toolbar_dispose (GObject *);
static void
paint_toolbar_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
paint_toolbar_init (PaintToolbar *);
static void
paint_toolbar_set_property (GObject *, guint, const GValue *, GParamSpec *);

G_DEFINE_FINAL_TYPE (PaintToolbar, paint_toolbar, GTK_TYPE_BOX);

/*******************************************************************************
 * @brief ツールバー クラスを初期化する。
 ******************************************************************************/
void
paint_toolbar_class_init (PaintToolbarClass *toolbar)
{
	paint_toolbar_class_init_object (G_OBJECT_CLASS (toolbar));
}

/*******************************************************************************
 * @brief オブジェクト クラスを初期化する。
 ******************************************************************************/
void
paint_toolbar_class_init_object (GObjectClass *object)
{
	object->dispose      = paint_toolbar_dispose;
	object->get_property = paint_toolbar_get_property;
	object->set_property = paint_toolbar_set_property;
	g_object_class_install_property (object, PAINT_TOOLBAR_PROPERTY_ACTIONS, PAINT_PARAM_SPEC_OBJECT (PAINT_TOOLBAR_PROPERTY_ACTIONS, G_TYPE_ACTION_GROUP));
}

/*******************************************************************************
 * @brief アクションを起こす。
 ******************************************************************************/
void
paint_toolbar_click_button (GtkButton *, gpointer self)
{
	PaintToolbar *toolbar;
	toolbar = PAINT_TOOLBAR (self);

	if (toolbar->actions)
	{
		g_action_group_activate_action (toolbar->actions, "new", NULL);
	}
}

/*******************************************************************************
 * @brief ツールバーを破棄する。
 ******************************************************************************/
void
paint_toolbar_destroy (PaintToolbar *toolbar)
{
	g_clear_object (&toolbar->actions);
}

/*******************************************************************************
 * @brief オブジェクトを破棄する。
 ******************************************************************************/
void
paint_toolbar_dispose (GObject *object)
{
	paint_toolbar_destroy (PAINT_TOOLBAR (object));
	G_OBJECT_CLASS (paint_toolbar_parent_class)->dispose (object);
}

/*******************************************************************************
 * @brief アクション グループを取得する。参照数を増やす。
 ******************************************************************************/
GActionGroup *
paint_toolbar_get_actions (PaintToolbar *toolbar)
{
	GActionGroup *actions;

	if (toolbar->actions)
	{
		actions = g_object_ref (toolbar->actions);
	}
	else
	{
		actions = NULL;
	}

	return actions;
}

/*******************************************************************************
 * @brief プロパティを取得する。
 ******************************************************************************/
void
paint_toolbar_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	switch (property_id)
	{
	case PAINT_TOOLBAR_PROPERTY_ACTIONS:
		g_value_set_object (value, PAINT_TOOLBAR (object)->actions);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

/*******************************************************************************
 * @brief ボタンを作成する。
 ******************************************************************************/
void
paint_toolbar_init (PaintToolbar *toolbar)
{
	GtkWidget *child;
	child = gtk_button_new ();

	if (child)
	{
		g_signal_connect (child, "clicked", G_CALLBACK (paint_toolbar_click_button), toolbar);
		gtk_box_append (GTK_BOX (toolbar), child);
	}
}

/*******************************************************************************
 * @brief ツールバー クラスのインスタンスを作成する。
 ******************************************************************************/
GtkWidget *
paint_toolbar_new (GActionGroup *actions)
{
	return g_object_new (PAINT_TYPE_TOOLBAR,
		"actions",     actions,
		"orientation", GTK_ORIENTATION_HORIZONTAL,
		NULL);
}

/*******************************************************************************
 * @brief アクション グループを設定する。参照数を増やす。
 ******************************************************************************/
void
paint_toolbar_set_actions (PaintToolbar *toolbar, GActionGroup *actions)
{
	if (toolbar->actions != actions)
	{
		if (toolbar->actions)
		{
			g_object_unref (toolbar->actions);
		}
		if (actions)
		{
			toolbar->actions = g_object_ref (actions);
		}
		else
		{
			toolbar->actions = NULL;
		}
	}
}

/*******************************************************************************
 * @brief プロパティを設定する。
 ******************************************************************************/
void
paint_toolbar_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	switch (property_id)
	{
	case PAINT_TOOLBAR_PROPERTY_ACTIONS:
		paint_toolbar_set_actions (PAINT_TOOLBAR (object), g_value_get_object (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}
