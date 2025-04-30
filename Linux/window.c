/*
Copyright 2025 Taichi Murakami.
Paint.ApplicationWindow class.
*/

#include "stdafx.h"
#define ACTION_ABOUT            "about"
#define ACTION_CLOSE            "close"
#define ACTION_COPY             "copy"
#define ACTION_CUT              "cut"
#define ACTION_NEW              "new"
#define ACTION_OPEN             "open"
#define ACTION_PAGE_SETUP       "page-setup"
#define ACTION_PASTE            "paste"
#define ACTION_PRINT            "print"
#define ACTION_REDO             "redo"
#define ACTION_SAVE             "save"
#define ACTION_SAVE_AS          "save-as"
#define ACTION_SELECT_ALL       "select-all"
#define ACTION_UNDO             "undo"
#define ACTION_VIEW             "view"
#define CCH_LABEL               32
#define DEFAULT_HEIGHT          480
#define DEFAULT_WIDTH           640
#define LOGO_ICON_NAME          "application-x-executable"
#define PROPERTY_APPLICATION    "application"
#define PROPERTY_SHOW_MENUBAR   "show-menubar"
#define SIGNAL_ACTIVATE         "activate"
#define SIGNAL_DESTROY          "destroy"
#define SIGNAL_ENTER            "enter"
#define SIGNAL_LEAVE            "leave"
#define SIGNAL_MOTION           "motion"
#define SIGNAL_PRESSED          "pressed"
#define SIGNAL_RELEASED         "released"
#define TEMPLATE_ABOUT          "about"
#define TEMPLATE_CLOSE          "new"
#define TEMPLATE_WINDOW         "window"

struct _PaintApplicationWindow
{
	GtkApplicationWindow parent;
	GtkDrawingArea *canvas;
	GtkGesture *gesture;
	GtkLabel *header_position;
	GtkLabel *header_size;
	GtkLabel *subtitle_label;
	GtkLabel *title_label;
	GtkEventController *motion;
	cairo_surface_t *surface;
	double motion_x;
	double motion_y;
	int pressed;
	int modified;
};

static void paint_application_window_class_init (PaintApplicationWindowClass *window);
static void paint_application_window_init (PaintApplicationWindow *window);
static void activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_close (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_copy (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_cut (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_new (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_open (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_page_setup (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_paste (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_print (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_redo (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_save (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_save_as (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_select_all (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void activate_undo (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void alert (GtkWindow *window);
static void clear (PaintApplicationWindow *window);
static gboolean close_request (GtkWindow *window);
static void constructed (GObject *object);
static void create_about_dialog (PaintApplicationWindow *window);
static void create_window (PaintApplicationWindow *window, GFile *file);
static void dispose (GObject *object);
static void draw_canvas (GtkDrawingArea *canvas, cairo_t *cairo, int width, int height, gpointer user_data);
static void draw_surface (PaintApplicationWindow *window, double x, double y);
static void enter (GtkEventControllerMotion *motion, double x, double y, gpointer user_data);
static void init_canvas (PaintApplicationWindow *window);
static void init_object_class (GObjectClass *class);
static void init_surface (PaintApplicationWindow *window, int width, int height);
static void init_widget_class (GtkWidgetClass *class);
static void init_window_class (GtkWindowClass *class);
static void leave (GtkEventControllerMotion *motion, double x, double y, gpointer user_data);
static void move (GtkEventControllerMotion *motion, double x, double y, gpointer user_data);
static void press (GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data);
static void release (GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data);
static void response_close (GObject *source, GAsyncResult *result, gpointer user_data);
static void response_open (GObject *source, GAsyncResult *result, gpointer user_data);
static void response_save (GObject *source, GAsyncResult *result, gpointer user_data);
static void update_header_position (GtkLabel *label, double x, double y);
static void update_header_size (GtkLabel *label, int width, int height);

G_DEFINE_FINAL_TYPE (PaintApplicationWindow, paint_application_window, GTK_TYPE_APPLICATION_WINDOW);
#define SELF    PAINT_APPLICATION_WINDOW
#define SUPER   paint_application_window_parent_class
#define TYPE    paint_application_window_get_type ()

static const GActionEntry action_entries[] =
{
	{ ACTION_ABOUT, activate_about },
	{ ACTION_CLOSE, activate_close },
	{ ACTION_COPY, activate_copy },
	{ ACTION_CUT, activate_cut },
	{ ACTION_NEW, activate_new },
	{ ACTION_OPEN, activate_open },
	{ ACTION_PAGE_SETUP, activate_page_setup },
	{ ACTION_PASTE, activate_paste },
	{ ACTION_PRINT, activate_print },
	{ ACTION_REDO, activate_redo },
	{ ACTION_SAVE, activate_save },
	{ ACTION_SAVE_AS, activate_save_as },
	{ ACTION_SELECT_ALL, activate_select_all },
	{ ACTION_UNDO, activate_undo },
};

GtkWidget *paint_application_window_new (GApplication *application)
{
	return (GtkWidget *) g_object_new (TYPE, PROPERTY_APPLICATION, application, NULL);
}

static void paint_application_window_class_init (PaintApplicationWindowClass *window)
{
	init_object_class (G_OBJECT_CLASS (window));
	init_widget_class (GTK_WIDGET_CLASS (window));
	init_window_class (GTK_WINDOW_CLASS (window));
}

static void paint_application_window_init (PaintApplicationWindow *window)
{
	gtk_widget_init_template (GTK_WIDGET (window));
	g_action_map_add_action_entries (G_ACTION_MAP (window), action_entries, G_N_ELEMENTS (action_entries), window);
	init_surface (window, 640, 480);
	init_canvas (window);
}

static void activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	create_about_dialog (user_data);
}

static void activate_close (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	PaintApplicationWindow *window = user_data;
	gtk_window_close (GTK_WINDOW (window));
}

static void activate_copy (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
}

static void activate_cut (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
}

static void activate_new (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	create_window (user_data, NULL);
}

static void activate_open (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	PaintApplicationWindow *window = user_data;
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		gtk_file_dialog_set_title (dialog, "Open");
		gtk_file_dialog_open (dialog, GTK_WINDOW (window), NULL, response_open, window);
		g_object_unref (dialog);
	}
}

static void activate_page_setup (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
}

static void activate_paste (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
}

static void activate_print (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
}

static void activate_redo (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
}

static void activate_save (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
}

static void activate_save_as (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	PaintApplicationWindow *window = user_data;
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();

	if (dialog)
	{
		gtk_file_dialog_set_title (dialog, "Save");
		gtk_file_dialog_save (dialog, GTK_WINDOW (window), NULL, response_save, window);
		g_object_unref (dialog);
	}
}

static void activate_select_all (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
}

static void activate_undo (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
}

static void alert (GtkWindow *window)
{
	const char *buttons[] = { "_Save and exit", "E_xit without saving", "_Cancel", NULL };
	GtkAlertDialog *dialog;
	dialog = gtk_alert_dialog_new ("Close requested.");

	if (dialog)
	{
		gtk_alert_dialog_set_buttons (dialog, buttons);
		gtk_alert_dialog_set_cancel_button (dialog, 2);
		gtk_alert_dialog_set_default_button (dialog, 0);
		gtk_alert_dialog_choose (dialog, window, NULL, response_close, window);
		g_object_unref (dialog);
	}
}

static void clear (PaintApplicationWindow *window)
{
	cairo_t *cairo;
	cairo_pattern_t *pattern;

	if (window->surface)
	{
		cairo = cairo_create (window->surface);
	
		if (cairo)
		{
			pattern = cairo_pattern_create_rgb (1.0, 1.0, 1.0);

			if (pattern)
			{
				cairo_set_source (cairo, pattern);
				cairo_paint (cairo);
				cairo_pattern_destroy (pattern);
			}

			cairo_destroy (cairo);
		}
	}
}

static gboolean close_request (GtkWindow *window)
{
	PaintApplicationWindow *paint;
	gboolean result;
	paint = PAINT_APPLICATION_WINDOW (window);
	result = paint && paint->modified;

	if (result)
	{
		alert (window);
	}

	return result;
}

static void constructed (GObject *object)
{
	gtk_window_set_default_size (GTK_WINDOW (object), DEFAULT_WIDTH, DEFAULT_HEIGHT);
	G_OBJECT_CLASS (SUPER)->constructed (object);
}

/* バージョン情報ダイアログ ボックスを表示します。 */
static void create_about_dialog (PaintApplicationWindow *window)
{
	GtkWidget *dialog;
	char text [CCH_LABEL];
	dialog = gtk_about_dialog_new ();

	if (dialog)
	{
		snprintf (text, CCH_LABEL, "%u.%u.%u", gtk_get_major_version (), gtk_get_minor_version (), gtk_get_micro_version ());
		gtk_window_set_destroy_with_parent (GTK_WINDOW (dialog), TRUE);
		gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
		gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));
		gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), paint_application_authors);
		gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), paint_application_copyright);
		gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG (dialog), GTK_LICENSE_APACHE_2_0);
		gtk_about_dialog_set_logo_icon_name (GTK_ABOUT_DIALOG (dialog), LOGO_ICON_NAME);
		gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (dialog), paint_application_name);
		gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), text);
		gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), paint_application_website);
		g_signal_connect_swapped(dialog, SIGNAL_DESTROY, G_CALLBACK(gtk_window_destroy), dialog);
		gtk_window_present (GTK_WINDOW (dialog));
	}
}

/* 新しいウィンドウを作成します。 */
static void create_window (PaintApplicationWindow *window, GFile *file)
{
	GtkApplication *application;
	GtkWidget *widget;
	application = gtk_window_get_application (GTK_WINDOW (window));

	if (application)
	{
		widget = paint_application_window_new (G_APPLICATION (application));
	
		if (widget)
		{
			gtk_window_present (GTK_WINDOW (widget));
		}
	}
}

static void dispose (GObject *object)
{
	init_surface (PAINT_APPLICATION_WINDOW (object), 0, 0);
	gtk_widget_dispose_template (GTK_WIDGET(object), TYPE);
	G_OBJECT_CLASS (SUPER)->dispose (object);
}

static void draw_canvas (GtkDrawingArea *canvas, cairo_t *cairo, int width, int height, gpointer user_data)
{
	PaintApplicationWindow *window = user_data;

	if (window->surface)
	{
		cairo_set_source_surface (cairo, window->surface, 0, 0);
		cairo_paint (cairo);
	}
}

static void draw_surface (PaintApplicationWindow *window, double x, double y)
{
	cairo_t *cairo;

	if (window->surface)
	{
		cairo = cairo_create (window->surface);

		if (cairo)
		{
			cairo_set_antialias (cairo, CAIRO_ANTIALIAS_NONE);
			cairo_set_line_width (cairo, 10);
			cairo_set_line_cap (cairo, CAIRO_LINE_CAP_ROUND);
			cairo_move_to (cairo, window->motion_x, window->motion_y);
			cairo_line_to (cairo, x, y);
			cairo_stroke (cairo);
			cairo_destroy (cairo);
		}
	}
}

static void enter (GtkEventControllerMotion *motion, double x, double y, gpointer user_data)
{
}

static void init_canvas (PaintApplicationWindow *window)
{
	if (window->canvas)
	{
		gtk_drawing_area_set_draw_func (window->canvas, draw_canvas, window, NULL);
		window->motion = gtk_event_controller_motion_new ();

		if (window->motion)
		{
			gtk_event_controller_set_propagation_phase (window->motion, GTK_PHASE_CAPTURE);
			g_signal_connect (window->motion, SIGNAL_ENTER, G_CALLBACK (enter), window);
			g_signal_connect (window->motion, SIGNAL_LEAVE, G_CALLBACK (leave), window);
			g_signal_connect (window->motion, SIGNAL_MOTION, G_CALLBACK (move), window);
			gtk_widget_add_controller (GTK_WIDGET (window->canvas), window->motion);
		}

		window->gesture = gtk_gesture_click_new ();

		if (window->gesture)
		{
			g_signal_connect (window->gesture, SIGNAL_PRESSED, G_CALLBACK (press), window);
			g_signal_connect (window->gesture, SIGNAL_RELEASED, G_CALLBACK (release), window);
			gtk_widget_add_controller (GTK_WIDGET (window->canvas), GTK_EVENT_CONTROLLER (window->gesture));
		}
	}
}

static void init_object_class (GObjectClass *class)
{
	class->constructed = constructed;
	class->dispose = dispose;
}

static void init_surface (PaintApplicationWindow *window, int width, int height)
{
	if (window->surface)
	{
		cairo_surface_destroy (window->surface);
	}
	if ((width > 0) && (height > 0))
	{
		window->surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);

		if (window->surface)
		{
			clear (window);
		}
		if (window->canvas)
		{
			gtk_drawing_area_set_content_width (window->canvas, width);
			gtk_drawing_area_set_content_height (window->canvas, height);
		}
		if (window->header_size)
		{
			update_header_size (window->header_size, width, height);
		}
	}
	else
	{
		window->surface = NULL;
	}
}

static void init_widget_class (GtkWidgetClass *class)
{
	char name [CCH_RESOURCE_NAME];
	format_resource_name (name, CCH_RESOURCE_NAME, TEMPLATE_WINDOW);
	gtk_widget_class_set_template_from_resource (class, name);
	gtk_widget_class_bind_template_child (class, PaintApplicationWindow, canvas);
	gtk_widget_class_bind_template_child (class, PaintApplicationWindow, header_position);
	gtk_widget_class_bind_template_child (class, PaintApplicationWindow, header_size);
	gtk_widget_class_bind_template_child (class, PaintApplicationWindow, subtitle_label);
	gtk_widget_class_bind_template_child (class, PaintApplicationWindow, title_label);
}

static void init_window_class (GtkWindowClass *class)
{
	class->close_request = close_request;
}

static void leave (GtkEventControllerMotion *motion, double x, double y, gpointer user_data)
{
}

static void move (GtkEventControllerMotion *motion, double x, double y, gpointer user_data)
{
	PaintApplicationWindow *window = user_data;

	if (window->pressed)
	{
		draw_surface (window, x, y);
		gtk_widget_queue_draw (gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (motion)));
	}
	if (window->header_position)
	{
		update_header_position (window->header_position, x, y);
	}

	window->motion_x = x;
	window->motion_y = y;
}

static void press (GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data)
{
	PaintApplicationWindow *window = user_data;
	g_print ("Press: %d\n", gtk_gesture_single_get_button (GTK_GESTURE_SINGLE (gesture)));
	window->pressed = TRUE;
}

static void release (GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data)
{
	PaintApplicationWindow *window = user_data;
	window->pressed = FALSE;
}

static void response_close (GObject *source, GAsyncResult *result, gpointer user_data)
{
	GtkAlertDialog *dialog;
	dialog = GTK_ALERT_DIALOG (source);

	if (dialog)
	{
		gtk_alert_dialog_choose_finish (dialog, result, NULL);
	}
}

/* ファイルを開くダイアログが応答しました。 */
static void response_open (GObject *source, GAsyncResult *result, gpointer user_data)
{
	PaintApplicationWindow *window = user_data;
	GError *error = NULL;
	GtkFileDialog *dialog;
	dialog = GTK_FILE_DIALOG (source);

	if (dialog)
	{
		GFile *file;
		file = gtk_file_dialog_open_finish (dialog, result, &error);

		if (file)
		{
			create_window (window, file);
			g_object_unref (file);
		}
	}
	if (error)
	{
		GtkAlertDialog *alert;
		alert = gtk_alert_dialog_new ("%d: %s", error->code, error->message);

		if (alert)
		{
			gtk_alert_dialog_show (alert, NULL);
			g_object_unref (alert);
		}

		g_error_free (error);
	}
}

static void response_save (GObject *source, GAsyncResult *result, gpointer user_data)
{
	GError *error = NULL;
	GtkFileDialog *dialog;
	dialog = GTK_FILE_DIALOG (source);

	if (dialog)
	{
		GFile *file;
		file = gtk_file_dialog_save_finish (dialog, result, &error);
		
		if (file)
		{
			g_object_unref (file);
		}
	}
	if (error)
	{
		GtkAlertDialog *alert;
		alert = gtk_alert_dialog_new ("%d: %s", error->code, error->message);

		if (alert)
		{
			gtk_alert_dialog_show (alert, NULL);
			g_object_unref (alert);
		}

		g_error_free (error);
	}
}

static void update_header_position (GtkLabel *label, double x, double y)
{
	char text [CCH_LABEL];
	int x0, y0;
	x0 = (int) lround (x);
	y0 = (int) lround (y);
	snprintf (text, CCH_LABEL, "Pos: (%d, %d)", x0, y0);
	gtk_label_set_label (label, text);
}

static void update_header_size (GtkLabel *label, int width, int height)
{
	char text [CCH_LABEL];
	snprintf (text, CCH_LABEL, "Size: (%d, %d)", width, height);
	gtk_label_set_label (label, text);
}
