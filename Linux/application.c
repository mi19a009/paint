/*
Copyright 2025 Taichi Murakami.
Paint.Application class.
*/

#include "stdafx.h"
#define ACTION_CLOSE_NAME       "win.close"
#define ACTION_CLOSE_ACCEL      "<Ctrl>Q"
#define ACTION_COPY_NAME        "win.copy"
#define ACTION_COPY_ACCEL       "<Ctrl>C"
#define ACTION_CUT_NAME         "win.cut"
#define ACTION_CUT_ACCEL        "<Ctrl>X"
#define ACTION_NEW_NAME         "win.new"
#define ACTION_NEW_ACCEL        "<Ctrl>N"
#define ACTION_OPEN_NAME        "win.open"
#define ACTION_OPEN_ACCEL       "<Ctrl>O"
#define ACTION_PASTE_NAME       "win.paste"
#define ACTION_PASTE_ACCEL      "<Ctrl>V"
#define ACTION_PRINT_NAME       "win.print"
#define ACTION_PRINT_ACCEL      "<Ctrl>P"
#define ACTION_QUIT             "quit"
#define ACTION_REDO_NAME        "win.redo"
#define ACTION_REDO_ACCEL       "<Shift><Ctrl>Z"
#define ACTION_SAVE_AS_NAME     "win.save-as"
#define ACTION_SAVE_AS_ACCEL    "<Shift><Ctrl>S"
#define ACTION_SAVE_NAME        "win.save"
#define ACTION_SAVE_ACCEL       "<Ctrl>S"
#define ACTION_SELECT_ALL_NAME  "win.select-all"
#define ACTION_SELECT_ALL_ACCEL "<Ctrl>A"
#define ACTION_UNDO_NAME        "win.undo"
#define ACTION_UNDO_ACCEL       "<Ctrl>Z"
#define PROPERTY_APPLICATION_ID "application-id"
#define PROPERTY_FLAGS          "flags"
#define TEMPLATE_MENU           "menu"

struct _PaintApplication
{
	GtkApplication parent;
	GtkWidget *window;
	GSettings *settings;
};

typedef struct _AccelEntry
{
	const char *action_name;
	const char *accels[2];
} AccelEntry;

static void paint_application_class_init (PaintApplicationClass *class);
static void paint_application_init (PaintApplication *application);
static void activate (GApplication *application);
static void activate_quit (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void clear (PaintApplication *application);
static void create_window (GApplication *application, GFile *file);
static void dispose (GObject *object);
static void init_accel (GtkApplication *application);
static void init_object (GObjectClass *class);
static void init_menu (GtkApplication *application);
static void init_application (GApplicationClass *class);
static void open (GApplication *application, GFile **files, gint n_files, const gchar *hint);
static void startup (GApplication *application);

G_DEFINE_FINAL_TYPE (PaintApplication, paint_application, GTK_TYPE_APPLICATION);
#define SELF    PAINT_APPLICATION
#define SUPER   paint_application_parent_class
#define TYPE    paint_application_get_type ()

static const AccelEntry accel_entries[] =
{
	{ ACTION_CLOSE_NAME, { ACTION_CLOSE_ACCEL, NULL }},
	{ ACTION_COPY_NAME, { ACTION_COPY_ACCEL, NULL }},
	{ ACTION_CUT_NAME, { ACTION_CUT_ACCEL, NULL }},
	{ ACTION_NEW_NAME, { ACTION_NEW_ACCEL, NULL }},
	{ ACTION_OPEN_NAME, { ACTION_OPEN_ACCEL, NULL }},
	{ ACTION_PASTE_NAME, { ACTION_PASTE_ACCEL, NULL }},
	{ ACTION_PRINT_NAME, { ACTION_PRINT_ACCEL, NULL }},
	{ ACTION_REDO_NAME, { ACTION_REDO_ACCEL, NULL }},
	{ ACTION_SAVE_AS_NAME, { ACTION_SAVE_AS_ACCEL, NULL }},
	{ ACTION_SAVE_NAME, { ACTION_SAVE_ACCEL, NULL }},
	{ ACTION_SELECT_ALL_NAME, { ACTION_SELECT_ALL_ACCEL, NULL }},
	{ ACTION_UNDO_NAME, { ACTION_UNDO_ACCEL, NULL }},
	{ NULL },
};

static const GActionEntry action_entries[] =
{
	{ ACTION_QUIT, activate_quit },
};

GApplication *paint_application_new (void)
{
	return (GApplication *) g_object_new (TYPE, PROPERTY_APPLICATION_ID, paint_application_id, PROPERTY_FLAGS, G_APPLICATION_HANDLES_OPEN, NULL);
}

static void paint_application_class_init (PaintApplicationClass *class)
{
	init_object (G_OBJECT_CLASS(class));
	init_application (G_APPLICATION_CLASS(class));
}

static void paint_application_init (PaintApplication *application)
{
	g_action_map_add_action_entries (G_ACTION_MAP (application), action_entries, G_N_ELEMENTS (action_entries), application);
}

static void activate (GApplication *application)
{
	G_APPLICATION_CLASS (SUPER)->activate (application);
	create_window (application, NULL);
}

static void activate_quit (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	PaintApplication *application = user_data;
	g_application_quit (G_APPLICATION (application));
}

static void clear (PaintApplication *application)
{
	g_clear_object (&application->settings);
}

static void create_window (GApplication *application, GFile *file)
{
	GtkWidget *window;
	window = paint_application_window_new (application);

	if (window)
	{
		gtk_window_present (GTK_WINDOW (window));
	}
}

static void dispose (GObject *object)
{
	clear (SELF (object));
	G_OBJECT_CLASS (SUPER)->dispose (object);
}

static void init_object (GObjectClass *class)
{
	class->dispose = dispose;
}

static void init_menu (GtkApplication *application)
{
	GMenuModel *menu;
	GtkBuilder *builder;
	char name [CCH_RESOURCE_NAME];
	format_resource_name (name, CCH_RESOURCE_NAME, TEMPLATE_MENU);
	builder = gtk_builder_new_from_resource(name);

	if (builder)
	{
		menu = G_MENU_MODEL (gtk_builder_get_object (builder, TEMPLATE_MENU));
	
		if (menu)
		{
			gtk_application_set_menubar (application, menu);
		}

		g_object_unref (builder);
	}
}

static void init_accel (GtkApplication *application)
{
	const AccelEntry *entries;

	for (entries = accel_entries; entries->action_name; entries++)
	{
		gtk_application_set_accels_for_action (application, entries->action_name, entries->accels);
	}
}

static void init_application (GApplicationClass *class)
{
	class->startup = startup;
	class->activate = activate;
	class->open = open;
}

static void open (GApplication *application, GFile **files, gint n_files, const gchar *hint)
{
	int n;

	for (n = 0; n < n_files; n++)
	{
		create_window (application, files[n]);
	}
}

static void startup (GApplication *application)
{
	G_APPLICATION_CLASS (SUPER)->startup (application);
	init_menu (GTK_APPLICATION (application));
	init_accel (GTK_APPLICATION (application));
}

const char *paint_application_id = "com.github.mi19a009.paint";
const char *paint_application_prefix = "/com/github/mi19a009/paint/";
