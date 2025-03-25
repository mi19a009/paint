/*
Copyright 2025 Taichi Murakami.
アプリケーションのメイン エントリ ポイントを実装します。
*/

#include <locale.h>
#include <gtk/gtk.h>
#define CAPTIONFORMAT           "%s - %s"
#define CCH_CAPTION             256
#define DEFAULT_HEIGHT          480
#define DEFAULT_WIDTH           640
#define ID_APPLICATION          "com.github.mi19a009.paint"
#define ID_RESOURCEPREFIX       "/com/github/mi19a009/paint/resource/"
#define SIG_ACTIVATE            "activate"
#define SIG_CLICK               "clicked"
#define SIG_CLOSE               "delete-event"
#define SIG_DESTROY             "destroy"
#define SIG_STATECHANGED        "window-state-event"
#define DATA_FILENAME           "FILENAME"
#define DATA_LANGUAGE           "LANGUAGE"
#define DATA_WINDOWSTATE        "WINDOWSTATE"
#define MAKELOCALE(a, b) (guint)(((a) & 0xFF) | (((b) & 0xFF) << 8))
#define MAKERESOURCE(name) (ID_RESOURCEPREFIX name)

typedef enum
{
	LANG_EN,
	LANG_JA,
	LANG_MAX,
} Language;

static GtkWidget *CreateClient(GtkWidget *window, Language language);
static GtkWidget *CreateMenuBar(GtkWidget *window, Language language);
static GtkWidget *CreateStatusbar(GtkWidget *widget);
static GtkWidget *CreateToolbar(GtkWidget *window, Language language);
static Language GetDefaultLanguage(void);
static void OnAbout(GtkAction *action, GtkWidget *window);
static void OnActivate(GApplication *app);
static gboolean OnClose(GtkWidget *window);
static void OnDestroy(GtkWidget *window);
static void OnFullscreen(GtkAction *action, GtkWidget *window);
static void OnNew(GtkAction *action, GtkWidget *window);
static void OnOpen(GtkAction *action, GtkWidget *window);
static void OnQuit(GtkAction *action, GtkWidget *window);
static void OnSave(GtkAction *action, GtkWidget *window);
static void OnSaveAs(GtkAction *action, GtkWidget *window);
static void OnStateChanged(GtkWidget *window, GdkEventWindowState *event);
static void SetCaption(GtkWidget *window, const gchar *filename, const gchar *title);
static void SetFileName(GtkWidget *window, const gchar *filename);
static void UpdateCaption(GtkWidget *window, Language language);

const gchar *AboutMenuItem[LANG_MAX] = { "_About", "バージョン情報(_A)" };
const gchar *Authors[] = { "Taichi Murakami", NULL };
const gchar *Comments = "＼（＾ｏ＾）／";
const gchar *Copyright = "Copyright © 2025 Taichi Murakami.";
const gchar *FileMenuItem[LANG_MAX] = { "_File", "ファイル(_F)" };
const gchar *FullscreenMenuItem[LANG_MAX] = { "_Fullscreen", "全画面表示(_F)" };
const gchar *HelpMenuItem[LANG_MAX] = { "_Help", "ヘルプ(_H)" };
const gchar *NewImage = MAKERESOURCE("new.svg");
const gchar *NewMenuItem[LANG_MAX] = { "_New", "新規(_N)" };
const gchar *NewToolItem[LANG_MAX] = { "New", "新規" };
const gchar *OpenImage = MAKERESOURCE("open.svg");
const gchar *OpenMenuItem[LANG_MAX] = { "_Open...", "開く(_O)..." };
const gchar *OpenToolItem[LANG_MAX] = { "Open...", "開く..." };
const gchar *QuitMenuItem[LANG_MAX] = { "_Quit", "アプリケーションを終了(_Q)" };
const gchar *SaveAsMenuItem[LANG_MAX] = { "Save _as...", "名前を付けて保存(_A)..." };
const gchar *SaveAsToolItem[LANG_MAX] = { "Save as...", "名前を付けて保存..." };
const gchar *SaveImage = MAKERESOURCE("save.svg");
const gchar *SaveMenuItem[LANG_MAX] = { "_Save", "上書き保存(_S)" };
const gchar *SaveToolItem[LANG_MAX] = { "Save", "上書き保存" };
const gchar *Title[LANG_MAX] = { "Paint", "お絵かきソフト" };
const gchar *Untitled[LANG_MAX] = { "(Untitled)", "(無題)" };
const gchar *Version = "2025";
const gchar *ViewMenuItem[LANG_MAX] = { "_View", "表示(_V)" };
const gchar *Website = "https://github.com/mi19a009/paint";

int main(int argc, char **argv)
{
	GtkApplication *app;
	int result;
	app = gtk_application_new(ID_APPLICATION, G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, SIG_ACTIVATE, G_CALLBACK(OnActivate), NULL);
	result = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return result;
}

static GtkWidget *CreateClient(GtkWidget *window, Language language)
{
	GtkWidget *box, *item;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(window), box);
	item = CreateMenuBar(window, language);
	gtk_box_pack_start(GTK_BOX(box), item, FALSE, FALSE, 0);
	item = CreateToolbar(window, language);
	gtk_box_pack_start(GTK_BOX(box), item, FALSE, FALSE, 0);
	item = CreateStatusbar(window);
	gtk_box_pack_start(GTK_BOX(box), item, FALSE, FALSE, 0);
	return box;
}

static GtkWidget *CreateMenuBar(GtkWidget *window, Language language)
{
	GtkAccelGroup *accel;
	GtkWidget *bar, *item, *menu;
	accel = gtk_accel_group_new();
	bar = gtk_menu_bar_new();
	gtk_window_add_accel_group(GTK_WINDOW(window), accel);

	/* File */
	item = gtk_menu_item_new_with_mnemonic(FileMenuItem[language]);
	menu = gtk_menu_new();
	gtk_container_add(GTK_CONTAINER(bar), item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);

	/* New */
	item = gtk_menu_item_new_with_mnemonic(NewMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	gtk_widget_add_accelerator(item, SIG_ACTIVATE, accel, GDK_KEY_N, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), SIG_ACTIVATE, G_CALLBACK(OnNew), window);

	/* Open */
	item = gtk_menu_item_new_with_mnemonic(OpenMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	gtk_widget_add_accelerator(item, SIG_ACTIVATE, accel, GDK_KEY_O, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), SIG_ACTIVATE, G_CALLBACK(OnOpen), window);

	/* --- */
	item = gtk_separator_menu_item_new();
	gtk_container_add(GTK_CONTAINER(menu), item);

	/* Save */
	item = gtk_menu_item_new_with_mnemonic(SaveMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	gtk_widget_add_accelerator(item, SIG_ACTIVATE, accel, GDK_KEY_S, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), SIG_ACTIVATE, G_CALLBACK(OnSave), window);

	/* Save As */
	item = gtk_menu_item_new_with_mnemonic(SaveAsMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	gtk_widget_add_accelerator(item, SIG_ACTIVATE, accel, GDK_KEY_S, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), SIG_ACTIVATE, G_CALLBACK(OnSaveAs), window);

	/* --- */
	item = gtk_separator_menu_item_new();
	gtk_container_add(GTK_CONTAINER(menu), item);

	/* Quit */
	item = gtk_menu_item_new_with_mnemonic(QuitMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	gtk_widget_add_accelerator(item, SIG_ACTIVATE, accel, GDK_KEY_Q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), SIG_ACTIVATE, G_CALLBACK(OnQuit), window);

	/* View */
	item = gtk_menu_item_new_with_mnemonic(ViewMenuItem[language]);
	menu = gtk_menu_new();
	gtk_container_add(GTK_CONTAINER(bar), item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);

	/* Fullscreen */
	item = gtk_menu_item_new_with_mnemonic(FullscreenMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	gtk_widget_add_accelerator(item, SIG_ACTIVATE, accel, GDK_KEY_F11, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), SIG_ACTIVATE, G_CALLBACK(OnFullscreen), window);

	/* Help */
	item = gtk_menu_item_new_with_mnemonic(HelpMenuItem[language]);
	menu = gtk_menu_new();
	gtk_container_add(GTK_CONTAINER(bar), item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);

	/* About */
	item = gtk_menu_item_new_with_mnemonic(AboutMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	g_signal_connect(G_OBJECT(item), SIG_ACTIVATE, G_CALLBACK(OnAbout), window);

	return bar;
}

static GtkWidget *CreateStatusbar(GtkWidget *widget)
{
	GtkWidget *statusbar;
	statusbar = gtk_statusbar_new();
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), 0, Comments);
	return statusbar;
}

static GtkWidget *CreateToolbar(GtkWidget *window, Language language)
{
	const int index = -1;
	GtkWidget *toolbar;
	GtkToolItem *item;
	toolbar = gtk_toolbar_new();

	/* New */
	item = gtk_tool_button_new(gtk_image_new_from_resource(NewImage), NewToolItem[language]);
	gtk_tool_item_set_tooltip_text(item, NewToolItem[language]);
	g_signal_connect(G_OBJECT(item), SIG_CLICK, G_CALLBACK(OnNew), window);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, index);

	/* Open */
	item = gtk_tool_button_new(gtk_image_new_from_resource(OpenImage), OpenToolItem[language]);
	gtk_tool_item_set_tooltip_text(item, OpenToolItem[language]);
	g_signal_connect(G_OBJECT(item), SIG_CLICK, G_CALLBACK(OnOpen), window);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, index);

	/* --- */
	item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, index);

	/* Save */
	item = gtk_tool_button_new(gtk_image_new_from_resource(SaveImage), SaveToolItem[language]);
	gtk_tool_item_set_tooltip_text(item, SaveToolItem[language]);
	g_signal_connect(G_OBJECT(item), SIG_CLICK, G_CALLBACK(OnSave), window);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, index);

	/* --- */
	item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, index);

	return toolbar;
}

static Language GetDefaultLanguage(void)
{
	const char *locale;
	Language language;
	locale = setlocale(LC_ALL, NULL);

	if (locale)
	{
		language = (Language)locale[0];

		if (language)
		{
			language |= ((Language)locale[1]) << 8;
		}
	}
	else
	{
		language = 0;
	}
	switch ((guint)language)
	{
	case MAKELOCALE('j', 'a'):
		language = LANG_JA;
		break;
	default:
		language = LANG_EN;
		break;
	}

	return language;
}

static void OnAbout(GtkAction *action, GtkWidget *window)
{
	GtkWidget *dialog;
	dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), Authors);
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), Comments);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), Copyright);
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_APACHE_2_0);
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), Title[LANG_JA]);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), Version);
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), Website);
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog), Website);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

static void OnActivate(GApplication *app)
{
	GtkWidget *window;
	Language language;
	language = GetDefaultLanguage();
	window = gtk_application_window_new(GTK_APPLICATION(app));
	g_object_set_data(G_OBJECT(window), DATA_LANGUAGE, (gpointer)language);
	gtk_window_set_default_size(GTK_WINDOW(window), DEFAULT_WIDTH, DEFAULT_HEIGHT);
	g_signal_connect(window, SIG_CLOSE, G_CALLBACK(OnClose), NULL);
	g_signal_connect(window, SIG_DESTROY, G_CALLBACK(OnDestroy), NULL);
	g_signal_connect(window, SIG_STATECHANGED, G_CALLBACK(OnStateChanged), NULL);
	CreateClient(window, language);
	UpdateCaption(window, language);
	gtk_widget_show_all(window);
}

static gboolean OnClose(GtkWidget *window)
{
	return FALSE;
}

static void OnDestroy(GtkWidget *window)
{
	SetFileName(window, NULL);
}

static void OnFullscreen(GtkAction *action, GtkWidget *window)
{
	GdkWindowState state = (GdkWindowState)g_object_get_data(G_OBJECT(window), DATA_WINDOWSTATE);

	if (state & GDK_WINDOW_STATE_FULLSCREEN)
	{
		gtk_window_unfullscreen(GTK_WINDOW(window));
	}
	else
	{
		gtk_window_fullscreen(GTK_WINDOW(window));
	}
}

static void OnNew(GtkAction *action, GtkWidget *window)
{
}

static void OnOpen(GtkAction *action, GtkWidget *window)
{
}

static void OnQuit(GtkAction *action, GtkWidget *window)
{
	gtk_window_close(GTK_WINDOW(window));
}

static void OnSave(GtkAction *action, GtkWidget *window)
{
}

static void OnSaveAs(GtkAction *action, GtkWidget *window)
{
}

static void OnStateChanged(GtkWidget *window, GdkEventWindowState *event)
{
	g_object_set_data(G_OBJECT(window), DATA_WINDOWSTATE, (gpointer)event->new_window_state);
}

static void SetCaption(GtkWidget *window, const gchar *filename, const gchar *title)
{
	gchar caption[CCH_CAPTION];
	snprintf(caption, CCH_CAPTION, CAPTIONFORMAT, filename, title);
	gtk_window_set_title(GTK_WINDOW(window), caption);
}

static void SetFileName(GtkWidget *window, const gchar *filename)
{
	gpointer previous;
	previous = g_object_get_data(G_OBJECT(window), DATA_FILENAME);
	g_object_set_data(G_OBJECT(window), DATA_FILENAME, (gpointer)filename);

	if (previous)
	{
		g_free(previous);
	}
}

static void UpdateCaption(GtkWidget *window, Language language)
{
	gchar *filename;
	filename = (gchar*)g_object_get_data(G_OBJECT(window), DATA_FILENAME);

	if (filename)
	{
		filename = g_path_get_basename(filename);

		if (filename)
		{
			SetCaption(window, filename, Title[language]);
			g_free(filename);
			return;
		}
	}

	SetCaption(window, Untitled[language], Title[language]);
}
