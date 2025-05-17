/*******************************************************************************
Copyright 2025 Taichi Murakami.
*******************************************************************************/

#include <locale.h>
#include <string.h>
#include "text.h"
#define TEXT_LOCALE(a, b) (((unsigned) (a) & 255) | (((unsigned) (b) & 255) << 8))
#define TEXT_LOCALE_JA TEXT_LOCALE ('j', 'a')
#define TEXT_LOCALE_LEN 2
#define TEXT_MAX TEXT_LANGUAGE_MAX

TextLanguage text_language;
typedef unsigned TextLocale;
static TextLocale text_locale_get (void);

Text text_label_about [TEXT_MAX] = {
	"_About",
	"このアプリケーションについて(_A)",
};
Text text_label_accept [TEXT_MAX] = {
	"_OK",
	"_OK",
};
Text text_label_brush [TEXT_MAX] = {
	"_Brush",
	"ブラシ(_B)",
};
Text text_label_cancel [TEXT_MAX] = {
	"_Cancel",
	"キャンセル(_C)",
};
Text text_label_edit [TEXT_MAX] = {
	"_Edit",
	"編集(_E)",
};
Text text_label_eraser [TEXT_MAX] = {
	"_Eraser",
	"消しゴム(_E)",
};
Text text_label_file [TEXT_MAX] = {
	"_File",
	"ファイル(_F)",
};
Text text_label_flip_horizontal [TEXT_MAX] = {
	"Flip _Horizontal",
	"水平に反転",
};
Text text_label_flip_vertical [TEXT_MAX] = {
	"Flip _Vertical",
	"垂直に反転",
};
Text text_label_help [TEXT_MAX] = {
	"_Help",
	"ヘルプ(_H)",
};
Text text_label_new [TEXT_MAX] = {
	"_New Window",
	"新規ウィンドウ(_N)",
};
Text text_label_open [TEXT_MAX] = {
	"_Open File...",
	"ファイルを開く(_O)...",
};
Text text_label_pencil [TEXT_MAX] = {
	"_Pencil",
	"えんぴつ(_P)",
};
Text text_label_quit [TEXT_MAX] = {
	"_Quit",
	"終了(_Q)",
};
Text text_label_save [TEXT_MAX] = {
	"_Save",
	"上書き保存(_S)",
};
Text text_label_save_as [TEXT_MAX] = {
	"Save _As...",
	"名前を付けて保存(_A)...",
};
Text text_label_tool [TEXT_MAX] = {
	"_Tool",
	"ツール(_T)",
};
Text text_label_undo [TEXT_MAX] = {
	"_Undo",
	"元に戻す(_U)",
};
Text text_label_view [TEXT_MAX] = {
	"_View",
	"表示(_V)",
};
Text text_title [TEXT_MAX] = {
	"Paint",
	"お絵かきソフト",
};
Text text_title_open [TEXT_MAX] = {
	"Open File",
	"ファイルを開く",
};
Text text_title_save [TEXT_MAX] = {
	"Save File",
	"ファイルを保存",
};
Text text_tooltip_brush [TEXT_MAX] = {
	"Brush",
	"ブラシ",
};
Text text_tooltip_eraser [TEXT_MAX] = {
	"Eraser",
	"消しゴム",
};
Text text_tooltip_new [TEXT_MAX] = {
	"New Window",
	"新規ウィンドウ",
};
Text text_tooltip_open [TEXT_MAX] = {
	"Open File",
	"ファイルを開く",
};
Text text_tooltip_pencil [TEXT_MAX] = {
	"Pencil",
	"えんぴつ",
};
Text text_tooltip_print [TEXT_MAX] = {
	"Print",
	"印刷",
};
Text text_tooltip_save [TEXT_MAX] = {
	"Save",
	"保存",
};

/*******************************************************************************
現在のロケールを初期化します。
@return
*******************************************************************************/
void text_language_init (void)
{
	switch (text_locale_get ())
	{
	case TEXT_LOCALE_JA:
		text_language = TEXT_LANGUAGE_JAPANESE;
		break;
	default:
		text_language = TEXT_LANGUAGE_ENGLISH;
		break;
	}
}

/*******************************************************************************
現在のロケールを 2 文字で説明します。
@return ロケールを説明する値。
*******************************************************************************/
TextLocale text_locale_get (void)
{
	const char *name;
	TextLocale locale = 0;
	name = setlocale (LC_ALL, NULL);

	if (name)
	{
		strncpy ((char *) &locale, name, TEXT_LOCALE_LEN);
	}

	return locale;
}
