/*
Copyright 2025 Taichi Murakami.
アプリケーションのメイン エントリ ポイントを実装します。
*/

#include <locale.h>
#include <gtk/gtk.h>
#include "source.h"
#include "resource.h"
#define CAPTIONFORMAT           "%s - %s"
#define ACTIVATE                "activate"

static int FormatCaptionText(gchar *buffer, size_t cch, const gchar *filename);

const gchar *RESOURCE_IMAGE_NEW = MAKERESOURCE("new.svg");
const gchar *RESOURCE_IMAGE_OPEN = MAKERESOURCE("open.svg");
const gchar *RESOURCE_IMAGE_SAVE = MAKERESOURCE("save.svg");
const gchar *TitleTexts[LANG_MAX] = { "Paint", "お絵かきソフト" };
const gchar *UntitledTexts[LANG_MAX] = { "(Untitled)", "(無題)" };

int main(int argc, char **argv)
{
	GtkApplication *app;
	int result;
	app = gtk_application_new(ID_APPLICATION, G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, ACTIVATE, G_CALLBACK(OnApplicationActivate), NULL);
	result = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return result;
}

void GetCaptionText(gchar *buffer, size_t cch, const gchar *filename)
{
	gchar *basename;

	if (filename)
	{
		basename = g_path_get_basename(filename);

		if (basename)
		{
			FormatCaptionText(buffer, cch, basename);
			g_free(basename);
			return;
		}
	}

	FormatCaptionText(buffer, cch, NULL);
}

Language GetDefaultLanguage(void)
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

static int FormatCaptionText(gchar *buffer, size_t cch, const gchar *filename)
{
	Language language;
	language = GetDefaultLanguage();

	if (!filename)
	{
		filename = UntitledTexts[language];
	}

	return snprintf(buffer, cch, CAPTIONFORMAT, filename, TitleTexts[language]);
}
