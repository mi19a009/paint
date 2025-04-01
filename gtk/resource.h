/*
Copyright 2025 Taichi Murakami.
アプリケーション内で使用する定数を定義します。
*/

#pragma once
#include <gtk/gtk.h>
#define DOCUMENT_MAXIMUM        65536
#define DOCUMENT_MINIMUM        1
#define ID_APPLICATION          "com.github.mi19a009.paint"
#define ID_RESOURCEPREFIX       "/com/github/mi19a009/paint/resource/"
#define MAKELOCALE(a, b) (guint)(((a) & 0xFF) | (((b) & 0xFF) << 8))
#define MAKERESOURCE(name) (ID_RESOURCEPREFIX name)

extern const gchar *RESOURCE_IMAGE_NEW;
extern const gchar *RESOURCE_IMAGE_OPEN;
extern const gchar *RESOURCE_IMAGE_SAVE;
