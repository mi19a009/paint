/* Copyright (C) 2025 Taichi Murakami. */
#include <gio/gio.h>
#include "text.h"

static GMenuModel *
paint_menu_new_help (void);
static GMenuModel *
paint_menu_new_file (void);

/*******************************************************************************
 * @brief アプリケーション用メニューを作成する。
 * @return 作成したメニューを返す。
 ******************************************************************************/
GMenuModel *
paint_menu_new (void)
{
	GMenu *menu;
	GMenuModel *item;
	menu = g_menu_new ();

	if (menu)
	{
		item = paint_menu_new_file ();

		if (item)
		{
			g_menu_append_submenu (menu, TEXT_MENU_FILE, item);
			g_object_unref (item);
		}

		item = paint_menu_new_help ();

		if (item)
		{
			g_menu_append_submenu (menu, TEXT_MENU_HELP, item);
			g_object_unref (item);
		}
	}

	return G_MENU_MODEL (menu);
}

/*******************************************************************************
 * @brief [ファイル] メニューを作成する。
 * @return 作成したメニューを返す。
 ******************************************************************************/
GMenuModel *
paint_menu_new_file (void)
{
	GMenu *menu, *section;
	menu = g_menu_new ();

	if (menu)
	{
		section = g_menu_new ();

		if (section)
		{
			g_menu_append (section, TEXT_MENU_NEW, "app.new");
			g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
			g_object_unref (section);
		}

		section = g_menu_new ();

		if (section)
		{
			g_menu_append (section, TEXT_MENU_CLOSE, "window.close");
			g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
			g_object_unref (section);
		}
	}

	return G_MENU_MODEL (menu);
}

/*******************************************************************************
 * @brief [ヘルプ] メニューを作成する。
 * @return 作成したメニューを返す。
 ******************************************************************************/
GMenuModel *
paint_menu_new_help (void)
{
	GMenu *menu, *section;
	menu = g_menu_new ();

	if (menu)
	{
		section = g_menu_new ();

		if (section)
		{
			g_menu_append (section, TEXT_MENU_ABOUT, "win.about");
			g_menu_append_section (menu, NULL, G_MENU_MODEL (section));
			g_object_unref (section);
		}
	}

	return G_MENU_MODEL (menu);
}
