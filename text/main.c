/* Copyright (C) 2025 Taichi Murakami. */
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#define CCH   256
#define FALSE 0
#define TRUE  1
#define LOCALE "ja_JP.UTF-8"
#define USAGE "Usage: \"%s\" <document-file-name> <source-file-name> <header-file-name>\n"
#define HEADER_BEGIN L"/* Text: Generated header file. */\n" \
	"#pragma once\n\n" \
	"typedef enum _TextLanguage TextLanguage;\n" \
	"extern TextLanguage text_language;\n" \
	"void text_language_init (void);\n\n" \
	"enum _TextLanguage\n{\n"
#define HEADER_ENUM L"\tTEXT_LANGUAGE_%ls,\n"
#define HEADER_MAX L"\tTEXT_LANGUAGE_MAX,\n};\n\n"
#define HEADER_TEXT L"extern const char *TEXT_%1$ls_ [];\n" \
	"#define            TEXT_%1$ls (TEXT_%1$ls_) [(text_language)]\n"
#define SOURCE_BEGIN L"/* Text: Generated source file. */\n" \
	"#include <locale.h>\n" \
	"#include <string.h>\n" \
	"#include \"text.h\"\n" \
	"#define TEXT_LOCALE(a, b) (((unsigned) (a) & 255) | (((unsigned) (b) & 255) << 8))\n" \
	"#define TEXT_LOCALE_LENGTH   2\n" \
	"#define TEXT_LOCALE_CHINESE  TEXT_LOCALE ('z', 'h')\n" \
	"#define TEXT_LOCALE_ENGLISH  TEXT_LOCALE ('e', 'n')\n" \
	"#define TEXT_LOCALE_JAPANESE TEXT_LOCALE ('j', 'a')\n" \
	"#define TEXT_LOCALE_KOREAN   TEXT_LOCALE ('k', 'o')\n\n" \
	"typedef unsigned TextLocale;\n" \
	"static TextLocale text_locale_get (void);\n" \
	"TextLanguage text_language;\n\n" \
	"void text_language_init (void)\n{\n" \
	"\tswitch (text_locale_get ())\n\t{\n"
#define SOURCE_CASE L"\tcase TEXT_LOCALE_%1$ls:\n" \
	"\t\ttext_language = TEXT_LANGUAGE_%1$ls;\n" \
	"\t\tbreak;\n"
#define SOURCE_END L"};\n"
#define SOURCE_ENUM L"\t\"%ls\",\n"
#define SOURCE_MAX L"\t}\n}\n\n" \
	"TextLocale text_locale_get (void)\n{\n" \
	"\tconst char *name;\n" \
	"\tTextLocale locale;\n" \
	"\tname = setlocale (LC_ALL, NULL);\n" \
	"\tif (name) strncpy ((char *) &locale, name, TEXT_LOCALE_LENGTH);\n" \
	"\telse locale = 0;\n" \
	"\treturn locale;\n" \
	"}\n\n"
#define SOURCE_NEXT L"};\n\n"
#define SOURCE_TEXT L"const char *\nTEXT_%ls_\n[TEXT_LANGUAGE_MAX] =\n{\n"

/* セルの位置と内容 */
typedef struct tagRANGE
{
	size_t row;
	size_t column;
	wchar_t value[CCH];
} RANGE;

static void cell(FILE *, FILE *, RANGE *);
static int run(const char *, const char *, const char *);

/*******************************************************************************
 * @brief C ソース ファイルを記述する。
 * @param source C ソース ファイル。
 * @param header C ヘッダー ファイル。
 * @param range 現在の文脈。
 ******************************************************************************/
void cell(FILE *source, FILE *header, RANGE *range)
{
	if (range->row) /* 中間行 */
	{
		if (range->column) /* 中間列 */
		{
			fwprintf(source, SOURCE_ENUM, range->value);
		}
		else /* 先頭列 */
		{
			if (range->row == 1)
			{
				fputws(SOURCE_MAX, source);
				fputws(HEADER_MAX, header);
			}
			else
			{
				fputws(SOURCE_NEXT, source);
			}

			fwprintf(source, SOURCE_TEXT, range->value);
			fwprintf(header, HEADER_TEXT, range->value);
		}
	}
	else /* 先頭行 */
	{
		if (range->column) /* 中間列 */
		{
			fwprintf(source, SOURCE_CASE, range->value);
			fwprintf(header, HEADER_ENUM, range->value);
		}
		else /* 先頭列 */
		{
			fputws(SOURCE_BEGIN, source);
			fputws(HEADER_BEGIN, header);
		}
	}
}

/*******************************************************************************
 * @brief 引数が指定されている場合は処理を開始する。
 * @param argc 引数の配列の要素数。
 * @param argv 引数の配列。
 * @return この操作が成功した場合は 0 を返す。
 ******************************************************************************/
int main(int argc, char *argv[])
{
	int exitcode = EXIT_FAILURE;

	if (argc > 0)
	{
		if (argc > 3)
		{
			exitcode = run(argv[1], argv[2], argv[3]);
		}
		else
		{
			fprintf(stderr, USAGE, *argv);
		}
	}

	return exitcode;
}

/*******************************************************************************
 * @brief 指定した CSV ファイルの内容から C ファイルを生成する。
 * @param document_path CSV ファイルへのパス。
 * @param source_path C ソース ファイルへのパス。
 * @param header_path ヘッダー ファイルへのパス。
 * @return この操作が成功した場合は 0 を返す。
 ******************************************************************************/
int run(const char *document_path, const char *source_path, const char *header_path)
{
	FILE *document, *header, *source;
	RANGE *range;
	size_t cch;
	wchar_t ch;
	int exitcode = EXIT_FAILURE;
	document = fopen(document_path, "r");

	if (document)
	{
		source = fopen(source_path, "w");

		if (source)
		{
			header = fopen(header_path, "w");

			if (header)
			{
				range = malloc(sizeof(RANGE));

				if (range)
				{
					range->row = 0;
					range->column = 0;
					cch = 0;
					exitcode = EXIT_SUCCESS;
					setlocale(LC_CTYPE, LOCALE);

					while ((ch = fgetwc(document)) != WEOF)
					{
						switch (ch)
						{
						case L'\n': /* 次の行へ進み、先頭列へ戻る。 */
							range->value[cch] = L'\0';
							cell(source, header, range);
							range->row++;
							range->column = 0;
							cch = 0;
							break;
						case L',': /* 次の列へ進む。 */
							range->value[cch] = L'\0';
							cell(source, header, range);
							range->column++;
							cch = 0;
							break;
						default: /* 次の文字へ進む。 */
							if (cch < (CCH - 1)) range->value[cch++] = ch;
							break;
						}
					}

					fputws(SOURCE_END, source);
					free(range);
				}

				fclose(header);
			}

			fclose(source);
		}

		fclose(document);
	}

	return exitcode;
}
