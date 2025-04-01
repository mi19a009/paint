/*
Copyright 2025 Taichi Murakami.
メニューを作成します。
*/

#include <gtk/gtk.h>
#include "source.h"
#define ACTIVATE "activate"

static const gchar *AboutMenuItem[LANG_MAX] = { "_About", "バージョン情報(_A)" };
static const gchar *FileMenuItem[LANG_MAX] = { "_File", "ファイル(_F)" };
static const gchar *FullscreenMenuItem[LANG_MAX] = { "_Fullscreen", "全画面表示(_F)" };
static const gchar *HelpMenuItem[LANG_MAX] = { "_Help", "ヘルプ(_H)" };
static const gchar *NewMenuItem[LANG_MAX] = { "_New", "新規(_N)" };
static const gchar *OpenMenuItem[LANG_MAX] = { "_Open...", "開く(_O)..." };
static const gchar *QuitMenuItem[LANG_MAX] = { "_Quit", "アプリケーションを終了(_Q)" };
static const gchar *SaveAsMenuItem[LANG_MAX] = { "Save _as...", "名前を付けて保存(_A)..." };
static const gchar *SaveMenuItem[LANG_MAX] = { "_Save", "上書き保存(_S)" };
static const gchar *ViewMenuItem[LANG_MAX] = { "_View", "表示(_V)" };

GtkWidget *CreateMenuBar(GtkWidget *window)
{
	GtkAccelGroup *accel;
	GtkWidget *bar, *item, *menu;
	Language language;
	language = GetDefaultLanguage();
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
	gtk_widget_add_accelerator(item, ACTIVATE, accel, GDK_KEY_N, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), ACTIVATE, G_CALLBACK(OnActionNew), window);

	/* Open */
	item = gtk_menu_item_new_with_mnemonic(OpenMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	gtk_widget_add_accelerator(item, ACTIVATE, accel, GDK_KEY_O, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), ACTIVATE, G_CALLBACK(OnActionOpen), window);

	/* --- */
	item = gtk_separator_menu_item_new();
	gtk_container_add(GTK_CONTAINER(menu), item);

	/* Save */
	item = gtk_menu_item_new_with_mnemonic(SaveMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	gtk_widget_add_accelerator(item, ACTIVATE, accel, GDK_KEY_S, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), ACTIVATE, G_CALLBACK(OnActionSave), window);

	/* Save As */
	item = gtk_menu_item_new_with_mnemonic(SaveAsMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	gtk_widget_add_accelerator(item, ACTIVATE, accel, GDK_KEY_S, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), ACTIVATE, G_CALLBACK(OnActionSaveAs), window);

	/* --- */
	item = gtk_separator_menu_item_new();
	gtk_container_add(GTK_CONTAINER(menu), item);

	/* Quit */
	item = gtk_menu_item_new_with_mnemonic(QuitMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	gtk_widget_add_accelerator(item, ACTIVATE, accel, GDK_KEY_Q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), ACTIVATE, G_CALLBACK(OnActionQuit), window);

	/* View */
	item = gtk_menu_item_new_with_mnemonic(ViewMenuItem[language]);
	menu = gtk_menu_new();
	gtk_container_add(GTK_CONTAINER(bar), item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);

	/* Fullscreen */
	item = gtk_menu_item_new_with_mnemonic(FullscreenMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	gtk_widget_add_accelerator(item, ACTIVATE, accel, GDK_KEY_F11, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), ACTIVATE, G_CALLBACK(OnActionFullscreen), window);

	/* Help */
	item = gtk_menu_item_new_with_mnemonic(HelpMenuItem[language]);
	menu = gtk_menu_new();
	gtk_container_add(GTK_CONTAINER(bar), item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);

	/* About */
	item = gtk_menu_item_new_with_mnemonic(AboutMenuItem[language]);
	gtk_container_add(GTK_CONTAINER(menu), item);
	g_signal_connect(G_OBJECT(item), ACTIVATE, G_CALLBACK(OnActionAbout), window);

	return bar;
}
