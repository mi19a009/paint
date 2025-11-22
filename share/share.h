/* Copyright (C) 2025 Taichi Murakami. */
#pragma once
#include <gtk/gtk.h>

typedef enum   _ShareFileDialogFlag ShareFileDialogFlag;
typedef struct _ShareAccelEntry     ShareAccelEntry;

/* ファイル フィルター */
enum _ShareFileDialogFlag
{
	SHARE_FILE_FILTER_NONE  = 0x00,
	SHARE_FILE_FILTER_ALL   = 0x01,
	SHARE_FILE_FILTER_IMAGE = 0x02,
};

/* メニュー アクセラレーター */
struct _ShareAccelEntry
{
	const char  *detailed_action_name;
	const char **accels;
};

/* Share モジュール */
void                    share_about_dialog_show             (GtkWindow *parent, const char *title, const char *logo_icon_name);
void                    share_alert_dialog_show             (GtkWindow *parent, const GError *error);
int                     share_application_run               (int argc, char **argv, GType type, const char *application_id, GApplicationFlags flags);
void                    share_application_set_accel_entries (GtkApplication *application, const ShareAccelEntry *entries, int n_entries);
void                    share_file_dialog_open              (GtkWindow *parent, GFile *initial_file, GAsyncReadyCallback callback, gpointer user_data, ShareFileDialogFlag flags);
void                    share_file_dialog_save              (GtkWindow *parent, GFile *initial_file, GAsyncReadyCallback callback, gpointer user_data, ShareFileDialogFlag flags);
GdkPixbuf              *share_pixbuf_create_from_file       (GFile *file, GError **error);
GtkPrintOperationResult share_print_operation_run           (GtkWindow *parent, GdkPixbuf *pixbuf, GError **error);
void                    share_surface_load                  (cairo_surface_t *surface, GdkPixbuf *pixbuf);

/* 32-ビット色 */
#define GetBValue(rgba)  ((rgba) & 255)
#define GetGValue(rgba)  (((rgba) >> 8) & 255)
#define GetRValue(rgba)  (((rgba) >> 16) & 255)
#define GetAValue(rgba)  (((rgba) >> 24) & 255)
#define RGBA(r, g, b, a) (((guint) (b) & 255) | (((guint) (g) & 255) << 8) | (((guint) (r) & 255) << 16) | (((guint) (a) & 255) << 24))

/* オブジェクト クラス プロパティ */
#define CLAMP_PROPERTY(VALUE, PROPERTY) CLAMP ((VALUE), (PROPERTY ## _MINIMUM), (PROPERTY ## _MAXIMUM))
#define OBJECT_CLASS_INSTALL_PROPERTY_BOOLEAN(THIS, PROPERTY) (g_object_class_install_property ((THIS), (PROPERTY ## _ID), g_param_spec_boolean ((PROPERTY ## _NAME), (PROPERTY ## _NICK), (PROPERTY ## _BLURB),                                                 (PROPERTY ## _DEFAULT_VALUE), (PROPERTY ## _FLAGS))))
#define OBJECT_CLASS_INSTALL_PROPERTY_BOXED(THIS, PROPERTY)   (g_object_class_install_property ((THIS), (PROPERTY ## _ID), g_param_spec_boxed   ((PROPERTY ## _NAME), (PROPERTY ## _NICK), (PROPERTY ## _BLURB), (PROPERTY ## _BOXED_TYPE),                                                    (PROPERTY ## _FLAGS))))
#define OBJECT_CLASS_INSTALL_PROPERTY_INT(THIS, PROPERTY)     (g_object_class_install_property ((THIS), (PROPERTY ## _ID), g_param_spec_int     ((PROPERTY ## _NAME), (PROPERTY ## _NICK), (PROPERTY ## _BLURB), (PROPERTY ## _MINIMUM), (PROPERTY ## _MAXIMUM), (PROPERTY ## _DEFAULT_VALUE), (PROPERTY ## _FLAGS))))
#define OBJECT_CLASS_INSTALL_PROPERTY_OBJECT(THIS, PROPERTY)  (g_object_class_install_property ((THIS), (PROPERTY ## _ID), g_param_spec_object  ((PROPERTY ## _NAME), (PROPERTY ## _NICK), (PROPERTY ## _BLURB), (PROPERTY ## _OBJECT_TYPE),                                                   (PROPERTY ## _FLAGS))))
#define OBJECT_CLASS_INSTALL_PROPERTY_UINT(THIS, PROPERTY)    (g_object_class_install_property ((THIS), (PROPERTY ## _ID), g_param_spec_uint    ((PROPERTY ## _NAME), (PROPERTY ## _NICK), (PROPERTY ## _BLURB), (PROPERTY ## _MINIMUM), (PROPERTY ## _MAXIMUM), (PROPERTY ## _DEFAULT_VALUE), (PROPERTY ## _FLAGS))))
