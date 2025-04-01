/*
Copyright 2025 Taichi Murakami.
ツールバーを作成します。
*/

#include <gtk/gtk.h>
#include "source.h"
#include "resource.h"
#define CLICK "clicked"

const gchar *NewToolItem[LANG_MAX] = { "New", "新規" };
const gchar *OpenToolItem[LANG_MAX] = { "Open...", "開く..." };
const gchar *SaveAsToolItem[LANG_MAX] = { "Save as...", "名前を付けて保存..." };
const gchar *SaveToolItem[LANG_MAX] = { "Save", "上書き保存" };

GtkWidget *CreateToolbar(GtkWidget *window)
{
	const int index = -1;
	GtkWidget *toolbar;
	GtkToolItem *item;
	Language language;
	language = GetDefaultLanguage();
	toolbar = gtk_toolbar_new();

	/* New */
	item = gtk_tool_button_new(gtk_image_new_from_resource(RESOURCE_IMAGE_NEW), NewToolItem[language]);
	gtk_tool_item_set_tooltip_text(item, NewToolItem[language]);
	g_signal_connect(G_OBJECT(item), CLICK, G_CALLBACK(OnActionNew), window);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, index);

	/* Open */
	item = gtk_tool_button_new(gtk_image_new_from_resource(RESOURCE_IMAGE_OPEN), OpenToolItem[language]);
	gtk_tool_item_set_tooltip_text(item, OpenToolItem[language]);
	g_signal_connect(G_OBJECT(item), CLICK, G_CALLBACK(OnActionOpen), window);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, index);

	/* --- */
	item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, index);

	/* Save */
	item = gtk_tool_button_new(gtk_image_new_from_resource(RESOURCE_IMAGE_SAVE), SaveToolItem[language]);
	gtk_tool_item_set_tooltip_text(item, SaveToolItem[language]);
	g_signal_connect(G_OBJECT(item), CLICK, G_CALLBACK(OnActionSave), window);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, index);

	/* --- */
	item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), item, index);

	return toolbar;
}
