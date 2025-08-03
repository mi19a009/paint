# Paint プロジェクト
GTK ウィンドウ上で画像を編集できるプログラム。

## インストール
開発環境をインストールする例を以下に示します。

``` sh
# GCC をインストールする。
sudo apt install build-essential
# GTK 4 をインストールする。
sudo apt install libgtk-4-dev
```

## コンパイル
Makefile を用いてソース ファイルをコンパイルします。

``` sh
# 実行可能ファイルを作成する。
make
# デバッグ シンボル付き実行可能ファイルを作成する。
make debug
# 最適化オプション付き実行可能ファイルを作成する。
make release
# 一時ディレクトリを削除する。
make clean
# 一時ディレクトリを作成する。
make target
# リソース ファイルを生成する。
make resource
```

## ドキュメント化
Doxygen を用いて C ソース ファイルから HTML ドキュメント ファイルを生成します。

``` sh
# Doxygen を使用可能にする。
sudo apt install doxygen
# Doxygen 上でクラス図を使用可能にする。
sudo apt install graphviz
# Doxygen を用いてドキュメントを生成する。
doxygen
```

## インクルード ファイル
Paint プロジェクトの C ソース ファイルは GTK をインクルードします。

``` c
#include <gtk/gtk.h>
```

Visual Studio Code で C を使用する場合は `.vscode/settings.json` に Default Include Path を設定します。

``` json
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
		"."
	]
}
```

Visual Studio Code で Makefile を使用する場合は設定から Makefile Safe Commands に `pkg-config` を追加します。

``` json
"makefile.safeCommands": [
  "pkg-config"
]
```

--------

- [戻る](../README.md)

Copyright © 2025 Taichi Murakami.
