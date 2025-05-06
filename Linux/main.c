/*
Copyright 2025 Taichi Murakami.
*/

#include <gtk/gtk.h>
#include "paint.h"

#define FORMAT_RESOURCE_NAME    "%s%s"
#define SETTINGS_EXTENSION      ".ini"
#define SETTINGS_FILENAME       "/proc/self/exe"
#define SIGNAL_ACTIVATE         "activate"

/* 新しいアプリケーションを開始します。 */
int main(int argc, char **argv)
{
	GApplication *application;
	int result;
	application = paint_application_new ();

	if (application)
	{
		result = g_application_run (application, argc, argv);
		g_object_unref (application);
	}
	else
	{
		result = -1;
	}

	return result;
}

/* メニュー項目が選択された際の行動を決定します。 */
void paint_action_map_add_action_entries (GActionMap *map, const PaintActionEntry *entries)
{
	GSimpleAction *action;
	GVariant *state;

	while (entries->name)
	{
		switch ((ptrdiff_t) entries->state)
		{
		case 0:
			action = g_simple_action_new (entries->name, NULL);
			break;
		case (ptrdiff_t) PAINT_ACTION_ENTRY_STATEFUL:
			state = g_variant_new_boolean (FALSE);
			action = g_simple_action_new_stateful (entries->name, NULL, state);
			break;
		default:
			state = g_variant_new_string (entries->state);
			action = g_simple_action_new_stateful (entries->name, G_VARIANT_TYPE_STRING, state);
			break;
		}

		g_signal_connect (action, SIGNAL_ACTIVATE, G_CALLBACK (entries->activate), map);
		g_action_map_add_action (map, G_ACTION (action));
		entries++;
	}
}

gchar *paint_get_settings_filename (void)
{
	gchar *filename, *path;
	size_t length;
	path = g_file_read_link (SETTINGS_FILENAME, NULL);

	if (path)
	{
		length = strlen (path);
		filename = g_renew (char, path, length + G_N_ELEMENTS (SETTINGS_EXTENSION));

		if (filename)
		{
			memcpy (filename + length, SETTINGS_EXTENSION, G_N_ELEMENTS (SETTINGS_EXTENSION));
		}
		else
		{
			g_free (path);
		}
	}
	else
	{
		filename = NULL;
	}

	return filename;
}

/* リソースへの完全パスを作成します。 */
int format_resource_name (char *buffer, size_t length, const char *name)
{
	return snprintf (buffer, length, FORMAT_RESOURCE_NAME, paint_application_prefix, name);
}
