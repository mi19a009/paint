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
GTK 4 を用いてアプリケーションを作成する。

### Paint プロジェクト各部の名称
```
Frame Window
+-------------------------+
| Title Bar               |
+-------------------------+
| Toolbar                 |
+-------------------------+
| Client Window           |
| +---------------------+ |
| | Viewport            | |
| | +-----------------+ | |
| | | Document Window | | |
| | |                 | | |
| | +-----------------+ | |
| |                     | |
| +---------------------+ |
|                         |
+-------------------------+
| Tab Bar                 |
+-------------------------+
| Status Bar              |
+-------------------------+
```

### Paint プロジェクトのオブジェクト
#### Paint About Dialog オブジェクト
アプリケーションのバージョンを説明する。
GTK About Dialog クラスのインスタンスを作成する。

#### Paint Canvas オブジェクト
指定した画像を描画する。
最初に、ドキュメント ウィンドウと縮小表示ウィンドウにキャンバスを作成する。
次に、各キャンバスに同じインスタンスの画像を与える。
結果として、各ウィンドウ上にユーザーが開いた画像を表示できる。
GTK Drawing Area クラスを継承する。

#### Paint Client Window オブジェクト
MDI クライアントを表す。

#### Paint Document Window オブジェクト
ユーザーがドキュメントを編集する方法を提供する。
ユーザーが開いた画像を所有する。
ユーザーがドキュメントをスクロールするためにスクロール バーを実装する。
ユーザーが画像上に描画するために GTK イベント コントローラーを実装する。
GTK Grid クラスを継承する。

#### Paint Error Dialog オブジェクト
指定したエラーを説明する。
GTK Alert Dialog クラスのインスタンスを作成する。

#### Paint File Dialog オブジェクト
ファイルを選択するダイアログ ボックスを表示する。
GTK File Dialog クラスのインスタンスを作成する。

#### Paint File Filter オブジェクト
ファイルを選択するダイアログ ボックスのフィルターを表す。
GTK File Filter クラスのインスタンスを作成する。

#### Paint Frame Window オブジェクト
ドキュメント ウィンドウ、ツールバー、およびステータス バーを所有する。
GTK Application Window クラスを継承する。

#### Paint Layer オブジェクト
ユーザーがレイヤーを選択する方法を提供する。
このオブジェクトはツール ウィンドウ上に配置される。
GTK Grid クラスを継承する。

#### Paint Main オブジェクト
GTK アプリケーションを開始する。
GTK Application クラスを継承する。

#### Paint Tool Window オブジェクト
モードレス ウィンドウを表す。
GTK Window クラスを継承する。

#### Paint Resource オブジェクト
リソースに関するヘルパー。

#### Paint Surface オブジェクト
Cairo Surface オブジェクトを表す。

#### Paint Statusbar オブジェクト
ステータス バーを表す。
GTK Box クラスを継承する。

#### Paint Tabbar オブジェクト
MDI クライアントを利用する際はタブ バー上にドキュメント ウィンドウへのボタンを列挙する。

#### Paint Toolbar オブジェクト
ツールバーを表す。
GTK Box クラスを継承する。

#### Paint Viewport オブジェクト
ユーザーがドキュメント ウィンドウをスクロールする方法を提供する。
GTK Viewport クラスのインスタンスを作成する。

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
