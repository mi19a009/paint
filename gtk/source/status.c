/*
Copyright 2025 Taichi Murakami.
ステータスを作成します。
*/

#include <gtk/gtk.h>
#include "source.h"
#define CCH_POSITION 32
#define DATA_POSITION "position"

static GtkWidget *GetPositionWidget(GtkWidget *status);
static void SetPositionWidget(GtkWidget *status, GtkWidget *item);

GtkWidget *CreateStatus(void)
{
	GtkWidget *item, *layout, *status;
	status = gtk_statusbar_new();
	layout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(status), layout);
	item = gtk_separator_new(0);
	gtk_box_pack_start(GTK_BOX(layout), item, FALSE, FALSE, 1);
	item = gtk_label_new("AAA");
	gtk_box_pack_start(GTK_BOX(layout), item, FALSE, FALSE, 1);
	SetPositionWidget(status, item);
	item = gtk_separator_new(0);
	gtk_box_pack_start(GTK_BOX(layout), item, FALSE, FALSE, 1);
	item = gtk_label_new("BBB");
	gtk_box_pack_start(GTK_BOX(layout), item, FALSE, FALSE, 1);
	item = gtk_separator_new(0);
	gtk_box_pack_start(GTK_BOX(layout), item, FALSE, FALSE, 1);
	item = gtk_label_new("CCC");
	gtk_box_pack_start(GTK_BOX(layout), item, FALSE, FALSE, 1);
	return status;
}

void SetStatusPosition(GtkWidget *status, int x, int y)
{
	gchar text[CCH_POSITION];
	status = GetPositionWidget(status);
	snprintf(text, CCH_POSITION, "Pos: (%d, %d)", x, y);
	gtk_label_set_text(GTK_LABEL(status), text);
}

static GtkWidget *GetPositionWidget(GtkWidget *status)
{
	return (GtkWidget*)g_object_get_data(G_OBJECT(status), DATA_POSITION);
}

static void SetPositionWidget(GtkWidget *status, GtkWidget *item)
{
	g_object_set_data(G_OBJECT(status), DATA_POSITION, (gpointer)item);
}
