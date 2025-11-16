/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#include "share.h"
#define APPLICATION_ID    "com.github.mi19a009.paint"
#define APPLICATION_FLAGS G_APPLICATION_HANDLES_OPEN
#define RESOURCE_FORMAT   "/com/github/mi19a009/paint/%s"

/*******************************************************************************
* @brief アプリケーションのメイン エントリ ポイントです。
*/
int
main (int argc, char *argv [])
{
	return share_application_run (argc, argv, PAINT_TYPE_APPLICATION, APPLICATION_ID, APPLICATION_FLAGS);
}

/*******************************************************************************
* @brief リソースへのパスを取得します。
*/
int
paint_get_resource_path (char *buffer, size_t maxlen, const char *name)
{
	return g_snprintf (buffer, maxlen, RESOURCE_FORMAT, name);
}

/*******************************************************************************
* @brief アプリケーションの環境設定を取得します。
*/
GSettings *
paint_get_settings (void)
{
	return g_settings_new (APPLICATION_ID);
}
