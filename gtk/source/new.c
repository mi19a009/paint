/*
Copyright 2025 Taichi Murakami.
新規ダイアログ ボックスを実装します。
*/

#include <gtk/gtk.h>
#include "source.h"
#define DIALOGFLAGS (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT)
#define MAXIMUM 65536
#define MINIMUM 1
#define STEP 1.0

//static const gchar *AcceptTexts[LANG_MAX] = { "OK", "OK" };
//static const gchar *CancelTexts[LANG_MAX] = { "Cancel", "キャンセル" };
static const gchar *Captions[LANG_MAX] = { "New", "新規" };
static const gchar *HeightTexts[LANG_MAX] = { "_Height:", "高さ(_H):" };
static const gchar *UnitTexts[LANG_MAX] = { "pixels", "ピクセル" };
static const gchar *WidthTexts[LANG_MAX] = { "_Width:", "幅(_W):" };
static GtkWidget *CreateLayout(GtkWidget *dialog);
static GtkWidget *CreateHeightSpinButton(GtkWidget *layout, Language language);
static GtkWidget *CreateWidthSpinButton(GtkWidget *layout, Language language);

gboolean NewDialog(NewDialogParam *param)
{
	GtkWidget *dialog, *layout, *height, *width;
	Language language;
	gboolean result;
	language = GetDefaultLanguage();
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	dialog = gtk_dialog_new_with_buttons(Captions[language], GTK_WINDOW(param->parent), DIALOGFLAGS, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
#pragma GCC diagnostic warning "-Wdeprecated-declarations"
	layout = CreateLayout(dialog);
	width = CreateWidthSpinButton(layout, language);
	height = CreateHeightSpinButton(layout, language);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(width), param->width);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(height), param->height);
	gtk_widget_show_all(dialog);
	result = gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT;

	if (result)
	{
		param->width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(width));
		param->height = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(height));
	}

	gtk_widget_destroy(dialog);
	return result;
}

static GtkWidget *CreateLayout(GtkWidget *dialog)
{
	GtkWidget *content, *layout;
	content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	layout = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(content), layout);
	return layout;
}

static GtkWidget *CreateHeightSpinButton(GtkWidget *layout, Language language)
{
	GtkWidget *result, *item;
	item = gtk_label_new_with_mnemonic(HeightTexts[language]);
	gtk_grid_attach(GTK_GRID(layout), item, 0, 1, 1, 1);
	result = gtk_spin_button_new_with_range(MINIMUM, MAXIMUM, STEP);
	gtk_grid_attach(GTK_GRID(layout), result, 1, 1, 1, 1);
	item = gtk_label_new_with_mnemonic(UnitTexts[language]);
	gtk_grid_attach(GTK_GRID(layout), item, 2, 1, 1, 1);
	return result;
}

static GtkWidget *CreateWidthSpinButton(GtkWidget *layout, Language language)
{
	GtkWidget *result, *item;
	item = gtk_label_new_with_mnemonic(WidthTexts[language]);
	gtk_grid_attach(GTK_GRID(layout), item, 0, 0, 1, 1);
	result = gtk_spin_button_new_with_range(MINIMUM, MAXIMUM, STEP);
	gtk_grid_attach(GTK_GRID(layout), result, 1, 0, 1, 1);
	item = gtk_label_new_with_mnemonic(UnitTexts[language]);
	gtk_grid_attach(GTK_GRID(layout), item, 2, 0, 1, 1);
	return result;
}
