/**
	@file main.h
	@author Taichi Murakami
	@date 2025
	@brief メイン関数を定義します。
*/
#include "paint.h"

const char *PAINT_APPLICATION_ID                    = "com.github.mi19a009.paint";
const char *PAINT_APPLICATION_WINDOW_CLASS_TEMPLATE = "/com/github/mi19a009/paint/src/paintapplicationwindow.ui";
const char *PAINT_RESOURCE_PATH_DIALOG              = "/com/github/mi19a009/paint/gtk/dialog.ui";
const char *PAINT_RESOURCE_PATH_MENUS               = "/com/github/mi19a009/paint/gtk/menus.ui";

/**
	アプリケーションのメイン エントリ ポイントです。
	@param argc コマンドライン引数の配列の要素数。
	@param argv コマンドライン引数の配列。
	@return この操作が成功した場合は 0 を返す。
*/
int main (int argc, char *argv [])
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
		result = EXIT_FAILURE;
	}

	return result;
}
