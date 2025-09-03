/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#define APPLICATION_FLAGS G_APPLICATION_HANDLES_OPEN
#define APPLICATION_ID "com.github.mi19a009.paint"
GApplication *paint_application_new (const char *application_id, GApplicationFlags flags);

/*******************************************************************************
アプリケーションのメイン エントリ ポイントです。
アプリケーションを開始します。
この操作が成功した場合は 0 を返します。
*/ int
main (int argc, char *argv [])
{
	GApplication *application;
	int result;
	application = paint_application_new (APPLICATION_ID, APPLICATION_FLAGS);

	if (application)
	{
		result = g_application_run (application, argc, argv);
		g_object_unref (application);
	}
	else
	{
		result = EXIT_FAILURE;
	}

	return result;
}
