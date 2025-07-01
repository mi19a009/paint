# Paint ソリューション
開発中。

## インストール
開発環境をインストールする例を以下に示します。

``` sh
# GCC をインストールする。
sudo apt install build-essential
# GTK 4 をインストールする。
sudo apt install libgtk-4-dev
```

## Docs プロジェクト
GitHub Pages 用 HTML 文書を記述します。

## Draw プロジェクト
開発中。

## Paint プロジェクト
開発中。

## Text プロジェクト
指定した CSV ファイルから C ソース ファイルやヘッダー ファイルを生成します。

## Settings ファイル
Visual Studio Code 用 [Settings ファイル](.vscode/settings.json)の内容を以下に示します。

``` json: .vscode/settings.json
{
	"C_Cpp.default.includePath": [
		"/usr/include/blkid",
		"/usr/include/cairo",
		"/usr/include/freetype2",
		"/usr/include/fribidi",
		"/usr/include/gdk-pixbuf-2.0",
		"/usr/include/glib-2.0",
		"/usr/include/graphene-1.0",
		"/usr/include/gtk-4.0",
		"/usr/include/harfbuzz",
		"/usr/include/libpng16",
		"/usr/include/libmount",
		"/usr/include/pango-1.0",
		"/usr/include/pixman-1",
		"/usr/include/x86_64-linux-gnu",
		"/usr/include/webp",
		"/usr/lib/x86_64-linux-gnu/glib-2.0/include",
		"/usr/lib/x86_64-linux-gnu/graphene-1.0/include",
	]
}
```

## ライセンス
このソフトウェアは Apache 2.0 でライセンスされました。

--------

Copyright © 2025 Taichi Murakami.
