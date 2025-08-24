/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint.h"
#define PAINT_APPLICATION_ID "com.github.mi19a009.paint"

/*******************************************************************************
Main 関数:
アプリケーションのメイン エントリ ポイントです。
アプリケーションを開始します。
この操作が成功した場合は 0 を返します。
*/ int
main (int argc, char *argv [])
{
	GApplication *application;
	int result;
	application = paint_application_new (PAINT_APPLICATION_ID);

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
