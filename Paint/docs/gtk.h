/**
	@file gtk.h
	@author Taichi Murakami
	@date 2025
	@brief GTK について Doxygen 用コメントを記述します。
	@note 各 C ソース ファイルはこのファイルをインクルードしない。
	@details
	アプリケーション定義クラスが継承したクラスを説明する。
*/
/* GObject */
/**
	@class GInitiallyUnowned
	@extends GObject
	@brief 初期浮動参照を持つオブジェクト型を表します。
	@sa https://docs.gtk.org/gobject/class.InitiallyUnowned.html
	@details
	GInitiallyUnowned 構造体内の全フィールドはプライベート実装であり、直接アクセスしない。
*/
/**
	@class GObject
	@brief 基底オブジェクト型です。
	@sa https://docs.gtk.org/gobject/class.Object.html
	@details
	GObject は GTK 内の各オブジェクト型について共通属性と操作方法の基本的な型を提供する。
	Pango とその他ライブラリは GObject を基とする。
	GObject クラスはオブジェクトを構築および破壊し、プロパティにアクセスする方法、およびシグナルをサポート方法を提供する。
*/
/* GIO */
/**
	@class GApplication
	@extends GObject
	@brief GApplication はアプリケーション サポート核となるクラスです。
	@sa https://docs.gtk.org/gio/class.Application.html
	@details
	GApplication はアプリケーションの基盤である。
	これは複数の低水準プラットフォーム定義サービスをラッピングし、
	GtkApplication と MxApplication を基盤とする高水準アプリケーションのクラス活動を意図する。
	このクラスは上位水準フレームワークの外側で使用しない。
*/
/* GTK */
/**
	@class GtkApplication
	@extends GApplication
	@brief アプリケーションを記述する高水準 API を表します。
	@sa https://docs.gtk.org/gtk4/class.Application.html
	@details
	GtkApplication は GTK アプリケーション記述について多くの側面をサポートする。
	GtkApplication は one-size-fit-all モデルを強制しない。

	GTK を初期化する。
	アプリケーションを独自化する。
	セッション管理する。
	基本的スケーラビリティを提供する。
	輸出活動によるデスクトップ シェル統合、メニュー、および最上位ウィンドウのリスト管理を操作する。
	最上位ウィンドウのライフサイクルは自動的にアプリケーションのライフサイクルと同等にする。

	GtkApplication は GtkWindow が生存中に稼働する。
	ウィンドウとして GtkApplicationWindow を使用することを推奨する。
*/
/**
	@class GtkApplicationWindow
	@extends GtkWindow
	@brief GtkApplication を統合する GtkWindow 派生クラスです。
	@sa https://docs.gtk.org/gtk4/class.ApplicationWindow.html
	@note GtkApplicationWindow はアプリケーションのメニューバーを操作できる。
	@note GtkApplicationWindow の内側に位置するウィジェットは GtkActionable インターフェースを実装したアクションをアクティブ化しても良い。
	@details
	このクラスは GActionGroup インターフェースと GActionMap インターフェースを実装する。
	GtkApplication 関連から輸入したウィンドウ定義アクションやアプリケーション帯域アクションを追加しても良い。
	ウィンドウ定義アクションは前置詞 "win." で修飾される。
	アプリケーション帯域アクションは前置詞 "app." で修飾される。
	各アクションはメニュー モデルから参照する修飾済み名前へ対応付けを必須とする。
*/
/**
	@class GtkWidget
	@extends GInitiallyUnowned
	@brief すべてのウィジェットの基底クラスです。
	@sa https://docs.gtk.org/gtk4/class.Widget.html
	@details
	ウィジェットの寿命、配置、状態、およびスタイルを管理する。
*/
/**
	@class GtkWindow
	@extends GtkWidget
	@brief その他ウィジェットを内包可能な最上位ウィンドウを表します。
	@sa https://docs.gtk.org/gtk4/class.Window.html
	@details
	普通ウィンドウはウィンドウ化システムの影響下で装飾されている。
	利用者がウィンドウを操作することを許可する。
	(ウィンドウをリサイズする。ウィンドウを移動する。ウィンドウを閉じる。)
*/
