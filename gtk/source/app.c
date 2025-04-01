/*
Copyright 2025 Taichi Murakami.
アプリケーション ウィンドウを実装します。
*/

#include <gtk/gtk.h>
#include "source.h"
#define CCH_CAPTION             256
#define DEFAULT_HEIGHT          480
#define DEFAULT_WIDTH           640
#define SIG_CLOSE               "delete-event"
#define SIG_DESTROY             "destroy"
#define SIG_STATECHANGED        "window-state-event"
#define DATA_FILENAME           "filename"
#define DATA_WINDOWSTATE        "windowstate"

static GtkWidget *CreateClient(GtkWidget *window);
static GtkWidget *CreateStatusbar(GtkWidget *widget);
static gchar *GetFileName(GtkWidget *window);
static GdkWindowState GetWindowState(GtkWidget *window);
static gboolean OnClose(GtkWidget *window);
static void OnDestroy(GtkWidget *window);
static void OnStateChanged(GtkWidget *window, GdkEventWindowState *event);
static void SetFileName(GtkWidget *window, gchar *filename);
static void SetWindowState(GtkWidget *window, GdkWindowState state);
static void UpdateCaption(GtkWidget *window);

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
	NewDialog(&param);
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
	UpdateCaption(window);
	gtk_widget_show_all(window);
}

static GtkWidget *CreateClient(GtkWidget *window)
{
	GtkWidget *box, *item;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(window), box);
	item = CreateMenuBar(window);
	gtk_box_pack_start(GTK_BOX(box), item, FALSE, FALSE, 0);
	item = CreateToolbar(window);
	gtk_box_pack_start(GTK_BOX(box), item, FALSE, FALSE, 0);
	item = CreateStatusbar(window);
	gtk_box_pack_start(GTK_BOX(box), item, FALSE, FALSE, 0);
	return box;
}

static GtkWidget *CreateStatusbar(GtkWidget *widget)
{
	GtkWidget *statusbar;
	statusbar = gtk_statusbar_new();
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), 0, "Status Bar");
	return statusbar;
}

static gchar *GetFileName(GtkWidget *window)
{
	return (gchar*)g_object_get_data(G_OBJECT(window), DATA_FILENAME);
}

static GdkWindowState GetWindowState(GtkWidget *window)
{
	return (GdkWindowState)g_object_get_data(G_OBJECT(window), DATA_WINDOWSTATE);
}

static gboolean OnClose(GtkWidget *window)
{
	return FALSE;
}

static void OnDestroy(GtkWidget *window)
{
	SetFileName(window, NULL);
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
