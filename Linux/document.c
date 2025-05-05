/*
Copyright 2025 Taichi Murakami.
ドキュメントを含むウィンドウを実装します。
*/

#include <gtk/gtk.h>
#include "paint.h"

#define ACTION_ABOUT            "about"
#define ACTION_FLIP_HORIZONTAL  "flip-horizontal"
#define ACTION_FLIP_VERTICAL    "flip-vertical"
#define ACTION_MODE             "mode"
#define ACTION_OPEN             "open"
#define ACTION_SAVE             "save"
#define ACTION_UNDO             "undo"
#define ALERT_RESPONSE_CANCEL   0
#define ALERT_RESPONSE_DESTROY  1
#define ALERT_RESPONSE_SAVE     2
#define CCH_TITLE               256
#define DEFAULT_HEIGHT          480
#define DEFAULT_WIDTH           640
#define FLAGS_MODIFIED          0x0001
#define FORMAT_ALERT            "%s"
#define FORMAT_TITLE            "%s%s - %s"
#define MODE_BRUSH              "brush"
#define MODE_ERASER             "eraser"
#define MODE_PENCIL             "pencil"
#define PROPERTY_APPLICATION    "application"
#define PROPERTY_SHOW_MENUBAR   "show-menubar"
#define SIGNAL_DOWN             "down"
#define SIGNAL_MOTION           "motion"
#define SIGNAL_PROXIMITY        "proximity"
#define SIGNAL_UP               "up"
#define UI_TEMPLATE             "document.ui"

typedef struct _PaintDocumentWindow
{
	GtkApplicationWindow super;
	GtkDrawingArea *canvas;
	cairo_surface_t *surface;
	char *filename;
	size_t basename;
	double x;
	double y;
	unsigned flags;
} Self;

static void paint_document_window_class_init (PaintDocumentWindowClass *self);
static void paint_document_window_init (PaintDocumentWindow *self);

G_DEFINE_FINAL_TYPE (PaintDocumentWindow, paint_document_window, GTK_TYPE_APPLICATION_WINDOW);
#define SELF                    PAINT_DOCUMENT_WINDOW
#define SUPER_CLASS             paint_document_window_parent_class
#define THIS_TYPE               paint_document_window_get_type ()

static void activate_about (GSimpleAction *action, GVariant *parameter, gpointer self);
static void activate_flip_horizontal (GSimpleAction *action, GVariant *parameter, gpointer self);
static void activate_flip_vertical (GSimpleAction *action, GVariant *parameter, gpointer self);
static void activate_mode (GSimpleAction *action, GVariant *parameter, gpointer self);
static void activate_open (GSimpleAction *action, GVariant *parameter, gpointer self);
static void activate_save (GSimpleAction *action, GVariant *parameter, gpointer self);
static void activate_undo (GSimpleAction *action, GVariant *parameter, gpointer self);
static void clear_modified (Self *self);
static gboolean close_request (GtkWindow *window);
static void constructed (GObject *object);
static void dispose (GObject *object);
static void dispose_self (Self *self);
static void draw_canvas (GtkDrawingArea *canvas, cairo_t *cairo, int width, int height, gpointer self);
static void draw_surface (Self *self, double x, double y);
static void init_canvas (Self *self);
static void init_object_class (GObjectClass *object);
static void init_surface (Self *self);
static void init_surface_from_file (Self *self);
static void init_widget_class (GtkWidgetClass *widget);
static void init_window_class (GtkWindowClass *window);
static void modify (Self *self);
static void move_down (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer self);
static void move_motion (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer self);
static void move_proximity (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer self);
static void move_up (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer self);
static void response_alert_dialog (GObject *dialog, GAsyncResult *result, gpointer self);
static void response_file_dialog_open (GObject *dialog, GAsyncResult *result, gpointer self);
static void response_file_dialog_save (GObject *dialog, GAsyncResult *result, gpointer self);
static void set_filename (Self *self, const char *filename);
static void show_alert_dialog (Self *self);
static void show_file_dialog_open (Self *self);
static void show_file_dialog_save (Self *self);
static void update_cursor_pos (Self *self, double x, double y);
static void update_title (Self *self);

static const PaintActionEntry action_entries [] =
{
	{ ACTION_ABOUT, activate_about },
	{ ACTION_FLIP_HORIZONTAL, activate_flip_horizontal, PAINT_ACTION_ENTRY_STATEFUL },
	{ ACTION_FLIP_VERTICAL, activate_flip_vertical, PAINT_ACTION_ENTRY_STATEFUL },
	{ ACTION_MODE, activate_mode, MODE_PENCIL },
	{ ACTION_OPEN, activate_open },
	{ ACTION_SAVE, activate_save },
	{ ACTION_UNDO, activate_undo },
	{ NULL },
};

/* クラスの新しいインスタンスを初期化します。 */
GtkWidget *paint_document_window_new (GApplication *application)
{
	return g_object_new (THIS_TYPE, PROPERTY_APPLICATION, application, PROPERTY_SHOW_MENUBAR, TRUE, NULL);
}

/* クラスの新しいインスタンスを初期化します。 */
GtkWidget *paint_document_window_new_from_file (GApplication *application, const char *filename)
{
	return paint_document_window_new (application);
}

/* クラスを初期化します。 */
static void paint_document_window_class_init (PaintDocumentWindowClass *self)
{
	init_object_class (G_OBJECT_CLASS (self));
	init_widget_class (GTK_WIDGET_CLASS (self));
	init_window_class (GTK_WINDOW_CLASS (self));
}

/* クラスの新しいインスタンスを初期化します。 */
static void paint_document_window_init (PaintDocumentWindow *self)
{
	gtk_widget_init_template (GTK_WIDGET (self));
	paint_action_map_add_action_entries (G_ACTION_MAP (self), action_entries);
	init_surface (self);
	init_canvas (self);
	update_title (self);
}

/* メニュー項目: バージョン情報。 */
static void activate_about (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	paint_about_dialog_show (self);
}

static void activate_flip_horizontal (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	g_action_change_state (G_ACTION (action), g_variant_new_boolean (!g_variant_get_boolean (g_action_get_state (G_ACTION (action)))));
}

static void activate_flip_vertical (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	g_action_change_state (G_ACTION (action), g_variant_new_boolean (!g_variant_get_boolean (g_action_get_state (G_ACTION (action)))));
}

/* メニュー項目: モード。 */
static void activate_mode (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	g_action_change_state (G_ACTION (action), parameter);
}

/* メニュー項目: ファイルを開く。 */
static void activate_open (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	show_file_dialog_open (self);
}

/* メニュー項目: 保存。 */
static void activate_save (GSimpleAction *action, GVariant *parameter, gpointer self)
{
	show_file_dialog_save (self);
}

static void activate_undo (GSimpleAction *action, GVariant *parameter, gpointer self)
{
}

static void clear_modified (Self *self)
{
	if (self->flags & FLAGS_MODIFIED)
	{
		self->flags &= ~FLAGS_MODIFIED;
		update_title (self);
	}
}

/* この関数はウィンドウが閉じられる場合に呼び出されます。 */
static gboolean close_request (GtkWindow *window)
{
	Self *self;
	gboolean result;
	self = SELF (window);
	result = self && (self->flags & FLAGS_MODIFIED);

	if (result)
	{
		show_alert_dialog (self);
	}

	return result;
}

static void constructed (GObject *object)
{
	G_OBJECT_CLASS (SUPER_CLASS)->constructed (object);
}

/* この関数は現在のインスタンスが破棄される場合に呼び出されます。 */
static void dispose (GObject *object)
{
	dispose_self (PAINT_DOCUMENT_WINDOW (object));
	gtk_widget_dispose_template (GTK_WIDGET(object), THIS_TYPE);
	G_OBJECT_CLASS (SUPER_CLASS)->dispose (object);
}

/* 自身のプロパティを開放します。 */
static void dispose_self (Self *self)
{
	g_clear_pointer (&self->filename, free);
	g_clear_pointer (&self->surface, cairo_surface_destroy);
}

/* 現在のウィンドウ上に描画します。 */
static void draw_canvas (GtkDrawingArea *canvas, cairo_t *cairo, int width, int height, gpointer self)
{
	if (((Self *) self)->surface)
	{
		cairo_set_source_surface (cairo, ((Self *) self)->surface, 0, 0);
		cairo_paint (cairo);
	}
}

/* 現在のサーフィス上に描画します。 */
static void draw_surface (Self *self, double x, double y)
{
	cairo_t *cairo;

	if (self->surface)
	{
		cairo = cairo_create (self->surface);

		if (cairo)
		{
			cairo_set_antialias (cairo, CAIRO_ANTIALIAS_NONE);
			cairo_set_line_width (cairo, 10);
			cairo_set_line_cap (cairo, CAIRO_LINE_CAP_ROUND);
			cairo_move_to (cairo, self->x, self->y);
			cairo_line_to (cairo, x, y);
			cairo_stroke (cairo);
			cairo_destroy (cairo);

			if (self->canvas)
			{
				gtk_widget_queue_draw (GTK_WIDGET (self->canvas));
			}
		}
	}
}

/* 新しいジェスチャを作成します。 */
static void init_canvas (Self *self)
{
	GtkGesture *gesture;

	if (self->canvas)
	{
		gesture = gtk_gesture_stylus_new ();
		gtk_drawing_area_set_draw_func (self->canvas, draw_canvas, self, NULL);

		if (gesture)
		{
			gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (gesture), GTK_PHASE_CAPTURE);
			gtk_gesture_stylus_set_stylus_only (GTK_GESTURE_STYLUS (gesture), FALSE);
			g_signal_connect (gesture, SIGNAL_DOWN, G_CALLBACK (move_down), self);
			g_signal_connect (gesture, SIGNAL_MOTION, G_CALLBACK (move_motion), self);
			g_signal_connect (gesture, SIGNAL_PROXIMITY, G_CALLBACK (move_proximity), self);
			g_signal_connect (gesture, SIGNAL_UP, G_CALLBACK (move_up), self);
			gtk_widget_add_controller (GTK_WIDGET (self->canvas), GTK_EVENT_CONTROLLER (gesture));
		}
	}
}

static void init_object_class (GObjectClass *object)
{
	object->constructed = constructed;
	object->dispose = dispose;
}

/* 新しいサーフィスを作成します。 */
static void init_surface (Self *self)
{
	if (self->surface)
	{
		cairo_surface_destroy (self->surface);
	}

	self->surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	gtk_widget_queue_draw (GTK_WIDGET (self->canvas));
}

/* 新しいサーフィスを作成します。 */
static void init_surface_from_file (Self *self)
{
	if (self->surface)
	{
		cairo_surface_destroy (self->surface);
	}

	self->surface = cairo_image_surface_create_from_png (self->filename);
	gtk_widget_queue_draw (GTK_WIDGET (self->canvas));
}

static void init_widget_class (GtkWidgetClass *widget)
{
	char name [CCH_RESOURCE_NAME];
	format_resource_name (name, CCH_RESOURCE_NAME, UI_TEMPLATE);
	gtk_widget_class_set_template_from_resource (widget, name);
	gtk_widget_class_bind_template_child (widget, Self, canvas);
}

static void init_window_class (GtkWindowClass *window)
{
	window->close_request = close_request;
}

static void modify (Self *self)
{
	if (!(self->flags & FLAGS_MODIFIED))
	{
		self->flags |= FLAGS_MODIFIED;
		update_title (self);
	}
}

static void move_down (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer self)
{
	update_cursor_pos (self, x ,y);
}

static void move_motion (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer self)
{
	draw_surface (self, x, y);
	update_cursor_pos (self, x ,y);
}

static void move_proximity (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer self)
{
	update_cursor_pos (self, x ,y);
}

static void move_up (GtkGestureStylus *stylus, gdouble x, gdouble y, gpointer self)
{
	modify (self);
}

static void response_alert_dialog (GObject *dialog, GAsyncResult *result, gpointer self)
{
	int response;
	response = gtk_alert_dialog_choose_finish (GTK_ALERT_DIALOG (dialog), result, NULL);

	switch (response)
	{
	case ALERT_RESPONSE_CANCEL:
		break;
	case ALERT_RESPONSE_DESTROY:
		g_idle_add_once ((GSourceOnceFunc) gtk_window_destroy, self);
		break;
	case ALERT_RESPONSE_SAVE:
		show_file_dialog_save (self);
		break;
	default:
		g_assert_not_reached ();
		break;
	}
}

/* この関数はファイルを開くダイアログを終了した場合に呼び出されます。 */
static void response_file_dialog_open (GObject *dialog, GAsyncResult *result, gpointer self)
{
	GFile *file;
	char *path;
	file = gtk_file_dialog_open_finish (GTK_FILE_DIALOG (dialog), result, NULL);

	if (file)
	{
		path = g_file_get_path (file);

		if (path)
		{
			clear_modified (self);
			set_filename (self, path);
			init_surface_from_file (self);
			g_free (path);
		}

		g_object_unref (file);
	}
}

/* この関数は保存ダイアログを終了した場合に呼び出されます。 */
static void response_file_dialog_save (GObject *dialog, GAsyncResult *result, gpointer self)
{
	GFile *file;
	char *path;
	file = gtk_file_dialog_save_finish (GTK_FILE_DIALOG (dialog), result, NULL);

	if (file)
	{
		path = g_file_get_path (file);

		if (path)
		{
			clear_modified (self);
			set_filename (self, path);
			g_free (path);
		}

		g_object_unref (file);
	}
}

static void set_filename (Self *self, const char *filename)
{
	size_t length;

	if (self->filename)
	{
		free (self->filename);
		self->filename = NULL;
		self->basename = 0;
	}
	if (filename)
	{
		length = strlen (filename) + 1;

		if (length)
		{
			self->filename = malloc (length);

			if (self->filename)
			{
				memcpy (self->filename, filename, length);
				filename = strrchr (self->filename, G_DIR_SEPARATOR);

				if (filename)
				{
					self->basename = (size_t) (filename - self->filename) + 1;
				}
			}
		}
	}

	update_title (self);
}

static void show_alert_dialog (Self *self)
{
	const char *labels [] = { "_Cancel", "_Destroy", "_Save", NULL };
	GtkAlertDialog *dialog;
	dialog = gtk_alert_dialog_new (FORMAT_ALERT, self->filename);

	if (dialog)
	{
		gtk_alert_dialog_set_buttons (dialog, labels);
		gtk_alert_dialog_set_cancel_button (dialog, 0);
		gtk_alert_dialog_set_default_button (dialog, 2);
		gtk_alert_dialog_set_modal (dialog, TRUE);
		gtk_alert_dialog_choose (dialog, GTK_WINDOW (self), NULL, response_alert_dialog, self);
		g_object_unref (dialog);
	}
}

/* ファイルを開くダイアログを表示します。 */
static void show_file_dialog_open (Self *self)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		gtk_file_dialog_set_title (dialog, "Open File");
		gtk_file_dialog_open (dialog, GTK_WINDOW (self), NULL, response_file_dialog_open, self);
		g_object_unref (dialog);
	}
}

/* 保存ダイアログを表示します。 */
static void show_file_dialog_save (Self *self)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		gtk_file_dialog_set_title (dialog, "Save File");
		gtk_file_dialog_save (dialog, GTK_WINDOW (self), NULL, response_file_dialog_save, self);
		g_object_unref (dialog);
	}
}

static void update_cursor_pos (Self *self, double x, double y)
{
	self->x = x;
	self->y = y;
}

static void update_title (Self *self)
{
	const char *basename;
	const char *modified;
	char title [CCH_TITLE];

	if (self->filename)
	{
		basename = self->filename + self->basename;
	}
	else
	{
		basename = "(Untitled)";
	}
	if (self->flags & FLAGS_MODIFIED)
	{
		modified = "*";
	}
	else
	{
		modified = "";
	}

	snprintf (title, CCH_TITLE, FORMAT_TITLE, modified, basename, paint_application_name);
	gtk_window_set_title (GTK_WINDOW (self), title);
}
