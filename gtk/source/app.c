/*
Copyright 2025 Taichi Murakami.
アプリケーション ウィンドウを実装します。
*/

#include <math.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "source.h"
#define CCH_CAPTION             256
#define DEFAULT_HEIGHT          480
#define DEFAULT_WIDTH           640
#define SIG_CLOSE               "delete-event"
#define SIG_DESTROY             "destroy"
#define SIG_DRAW                "draw"
#define SIG_ENTER               "enter-notify-event"
#define SIG_MOTION              "motion-notify-event"
#define SIG_STATECHANGED        "window-state-event"
#define DATA_FILENAME           "filename"
#define DATA_POSITION           "position"
#define DATA_STATUS             "status"
#define DATA_SURFACE            "surface"
#define DATA_WINDOWSTATE        "state"

typedef struct
{
	double x;
	double y;
} Point;

static GtkWidget *CreateCanvas(GtkWidget *window);
static GtkWidget *CreateClient(GtkWidget *window);
static void Draw(GtkWidget *window, double x1, double y1);
static gchar *GetFileName(GtkWidget *window);
static void GetPosition(GtkWidget *window, double *x, double *y);
static GtkWidget *GetStatus(GtkWidget *window);
static cairo_surface_t *GetSurface(GtkWidget *window);
static GdkWindowState GetWindowState(GtkWidget *window);
static cairo_surface_t *InitializeSurface(GtkWidget *window, int width, int height);
static gboolean OnClose(GtkWidget *window);
static void OnDestroy(GtkWidget *window);
static gboolean OnDraw(GtkWidget *canvas, cairo_t *cairo, GtkWidget *window);
static gboolean OnMotion(GtkWidget *canvas, GdkEventMotion *event, GtkWidget *window);
static void OnStateChanged(GtkWidget *window, GdkEventWindowState *event);
static void SetFileName(GtkWidget *window, gchar *filename);
static void SetPosition(GtkWidget *window, double x, double y);
static void SetStatus(GtkWidget *window, GtkWidget *status);
static void SetSurface(GtkWidget *window, cairo_surface_t *surface);
static void SetWindowState(GtkWidget *window, GdkWindowState state);
static void UpdateCaption(GtkWidget *window);
static void UpdateMotion(GtkWidget *window, double x, double y);

void OnActionAbout(GtkAction *action, GtkWidget *window)
{
	AboutDialog();
}

void OnActionFullscreen(GtkAction *action, GtkWidget *window)
{
	GdkWindowState state;
	state = GetWindowState(window);

	if (state & GDK_WINDOW_STATE_FULLSCREEN)
	{
		gtk_window_unfullscreen(GTK_WINDOW(window));
	}
	else
	{
		gtk_window_fullscreen(GTK_WINDOW(window));
	}
}

void OnActionNew(GtkAction *action, GtkWidget *window)
{
	NewDialogParam param;
	param.parent = window;
	param.width = DEFAULT_WIDTH;
	param.height = DEFAULT_HEIGHT;

	if (NewDialog(&param))
	{
		InitializeSurface(window, param.width, param.height);
		gtk_widget_queue_draw(window);
	}
}

void OnActionOpen(GtkAction *action, GtkWidget *window)
{
}

void OnActionQuit(GtkAction *action, GtkWidget *window)
{
	gtk_window_close(GTK_WINDOW(window));
}

void OnActionSave(GtkAction *action, GtkWidget *window)
{
}

void OnActionSaveAs(GtkAction *action, GtkWidget *window)
{
}

void OnApplicationActivate(GApplication *app)
{
	GtkWidget *window;
	window = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_default_size(GTK_WINDOW(window), DEFAULT_WIDTH, DEFAULT_HEIGHT);
	g_signal_connect(window, SIG_CLOSE, G_CALLBACK(OnClose), NULL);
	g_signal_connect(window, SIG_DESTROY, G_CALLBACK(OnDestroy), NULL);
	g_signal_connect(window, SIG_STATECHANGED, G_CALLBACK(OnStateChanged), NULL);
	CreateClient(window);
	InitializeSurface(window, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	UpdateCaption(window);
	gtk_widget_show_all(window);
}

static GtkWidget *CreateCanvas(GtkWidget *window)
{
	GtkWidget *canvas;
	canvas = gtk_drawing_area_new();
	gtk_widget_add_events(canvas, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK);
	g_signal_connect(canvas, SIG_DRAW, G_CALLBACK(OnDraw), window);
	g_signal_connect(canvas, SIG_MOTION, G_CALLBACK(OnMotion), window);
	return canvas;
}

static GtkWidget *CreateClient(GtkWidget *window)
{
	GtkWidget *box, *item;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(window), box);
	item = CreateMenu(window);
	gtk_box_pack_start(GTK_BOX(box), item, FALSE, FALSE, 0);
	item = CreateToolbar(window);
	gtk_box_pack_start(GTK_BOX(box), item, FALSE, FALSE, 0);
	item = CreateCanvas(window);
	gtk_box_pack_start(GTK_BOX(box), item, TRUE, TRUE, 0);
	item = CreateStatus();
	gtk_box_pack_start(GTK_BOX(box), item, FALSE, FALSE, 0);
	SetStatus(window, item);
	return box;
}

static void Draw(GtkWidget *window, double x1, double y1)
{
	cairo_t *cairo;
	cairo_surface_t *surface;
	double x0, y0;
	surface = GetSurface(window);

	if (surface)
	{
		cairo = cairo_create(surface);

		if (cairo)
		{
			GetPosition(window, &x0, &y0);
			cairo_move_to(cairo, x0, y0);
			cairo_line_to(cairo, x1, y1);
			cairo_stroke(cairo);
			cairo_destroy(cairo);
		}
	}
}

static gchar *GetFileName(GtkWidget *window)
{
	return (gchar*)g_object_get_data(G_OBJECT(window), DATA_FILENAME);
}

static void GetPosition(GtkWidget *window, double *x, double *y)
{
	Point *position;
	position = (Point*)g_object_get_data(G_OBJECT(window), DATA_POSITION);

	if (position)
	{
		*x = position->x;
		*y = position->y;
	}
	else
	{
		*x = 0.0;
		*y = 0.0;
	}
}

static GtkWidget *GetStatus(GtkWidget *window)
{
	return (GtkWidget*)g_object_get_data(G_OBJECT(window), DATA_STATUS);
}

static cairo_surface_t *GetSurface(GtkWidget *window)
{
	return (cairo_surface_t*)g_object_get_data(G_OBJECT(window), DATA_SURFACE);
}

static GdkWindowState GetWindowState(GtkWidget *window)
{
	return (GdkWindowState)g_object_get_data(G_OBJECT(window), DATA_WINDOWSTATE);
}

static cairo_surface_t *InitializeSurface(GtkWidget *window, int width, int height)
{
	cairo_surface_t *surface;
	SetSurface(window, NULL);
	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	SetSurface(window, surface);
	return surface;
}

static gboolean OnClose(GtkWidget *window)
{
	return FALSE;
}

static void OnDestroy(GtkWidget *window)
{
	SetFileName(window, NULL);
	SetPosition(window, 0, 0);
	SetSurface(window, NULL);
}

static gboolean OnDraw(GtkWidget *canvas, cairo_t *cairo, GtkWidget *window)
{
	cairo_surface_t *surface;
	surface = GetSurface(window);

	if (surface)
	{
		cairo_set_source_surface(cairo, surface, 0, 0);
		cairo_paint(cairo);
	}

	return FALSE;
}

static gboolean OnMotion(GtkWidget *canvas, GdkEventMotion *event, GtkWidget *window)
{
	UpdateMotion(window, event->x, event->y);

	if (event->state & GDK_BUTTON1_MASK)
	{
		Draw(window, event->x, event->y);
		gtk_widget_queue_draw(canvas);
	}

	SetPosition(window, event->x, event->y);
	return FALSE;
}

static void OnStateChanged(GtkWidget *window, GdkEventWindowState *event)
{
	SetWindowState(window, event->new_window_state);
}

static void SetFileName(GtkWidget *window, gchar *filename)
{
	gpointer previous;
	previous = g_object_get_data(G_OBJECT(window), DATA_FILENAME);
	g_object_set_data(G_OBJECT(window), DATA_FILENAME, (gpointer)filename);

	if (previous)
	{
		g_free(previous);
	}
}

static void SetPosition(GtkWidget *window, double x, double y)
{
	Point *position;
	position = (Point*)g_object_get_data(G_OBJECT(window), DATA_POSITION);

	if (x || y)
	{
		if (!position)
		{
			position = malloc(sizeof(Point));
			g_object_set_data(G_OBJECT(window), DATA_POSITION, position);
		}
		if (position)
		{
			position->x = x;
			position->y = y;
		}
	}
	else
	{
		if (position)
		{
			free(position);
		}

		g_object_set_data(G_OBJECT(window), DATA_POSITION, NULL);
	}
}

static void SetStatus(GtkWidget *window, GtkWidget *status)
{
	g_object_set_data(G_OBJECT(window), DATA_STATUS, (gpointer)status);
}

static void SetSurface(GtkWidget *window, cairo_surface_t *surface)
{
	cairo_surface_t *previous;
	previous = (cairo_surface_t*)g_object_get_data(G_OBJECT(window), DATA_SURFACE);
	g_object_set_data(G_OBJECT(window), DATA_SURFACE, (gpointer)surface);

	if (previous)
	{
		cairo_surface_destroy(previous);
	}
}

static void SetWindowState(GtkWidget *window, GdkWindowState state)
{
	g_object_set_data(G_OBJECT(window), DATA_WINDOWSTATE, (gpointer)state);
}

static void UpdateCaption(GtkWidget *window)
{
	gchar *filename;
	gchar caption[CCH_CAPTION];
	filename = GetFileName(window);
	GetCaptionText(caption, CCH_CAPTION, filename);
	gtk_window_set_title(GTK_WINDOW(window), caption);
}

static void UpdateMotion(GtkWidget *window, double x, double y)
{
	GtkWidget *status;
	status = GetStatus(window);

	if (status)
	{
		SetStatusPosition(status, (int)lround(x), (int)lround(y));
	}
}
