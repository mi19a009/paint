/*******************************************************************************
Copyright 2025 Taichi Murakami.
このヘッダー ファイルは GTK に依存しない機能を公開します。
文字列定数へのポインターを提供します。
*******************************************************************************/

#pragma once
#define TEXT(text) ((text) [text_language])

/* 文字列リテラルへの言語。 */
typedef enum _TextLanguage
{
	TEXT_LANGUAGE_ENGLISH,
	TEXT_LANGUAGE_JAPANESE,
	TEXT_LANGUAGE_MAX,
} TextLanguage;

typedef const char *Text;
void text_language_init (void);
extern TextLanguage text_language;
extern Text text_label_about [];
extern Text text_label_accept [];
extern Text text_label_brush [];
extern Text text_label_cancel [];
extern Text text_label_edit [];
extern Text text_label_eraser [];
extern Text text_label_file [];
extern Text text_label_flip_horizontal [];
extern Text text_label_flip_vertical [];
extern Text text_label_help [];
extern Text text_label_new [];
extern Text text_label_open [];
extern Text text_label_pencil [];
extern Text text_label_quit [];
extern Text text_label_save [];
extern Text text_label_save_as [];
extern Text text_label_tool [];
extern Text text_label_undo [];
extern Text text_label_view [];
extern Text text_title [];
extern Text text_title_library [];
extern Text text_title_open [];
extern Text text_title_save [];
extern Text text_tooltip_brush [];
extern Text text_tooltip_eraser [];
extern Text text_tooltip_new [];
extern Text text_tooltip_open [];
extern Text text_tooltip_pencil [];
extern Text text_tooltip_print [];
extern Text text_tooltip_save [];
