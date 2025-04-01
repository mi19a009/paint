/*
Copyright 2025 Taichi Murakami.
外部リンケージを持つシンボルを宣言します。
*/

#include <gtk/gtk.h>

typedef enum
{
	LANG_EN,
	LANG_JA,
	LANG_MAX,
} Language;

typedef struct
{
	GtkWidget *parent;
	int width;
	int height;
} NewDialogParam;

extern const gchar *TitleTexts[];
extern const gchar *UntitledTexts[];

void AboutDialog(void);
GtkWidget *CreateMenuBar(GtkWidget *window);
GtkWidget *CreateToolbar(GtkWidget *window);
void GetCaptionText(gchar *buffer, size_t cch, const gchar *filename);
Language GetDefaultLanguage(void);
gboolean NewDialog(NewDialogParam *param);
void OnActionAbout(GtkAction *action, GtkWidget *window);
void OnActionFullscreen(GtkAction *action, GtkWidget *window);
void OnActionNew(GtkAction *action, GtkWidget *window);
void OnActionOpen(GtkAction *action, GtkWidget *window);
void OnActionQuit(GtkAction *action, GtkWidget *window);
void OnActionSave(GtkAction *action, GtkWidget *window);
void OnActionSaveAs(GtkAction *action, GtkWidget *window);
void OnApplicationActivate(GApplication *app);
