/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#define PAINT_RESOURCE_PATH_CCH 64

/* Paint モジュール */
GResource *paint_get_resource      (void);
int        paint_get_resource_path (char *buffer, size_t maxlen, const char *name);

/* Paint Application クラス */
GApplication *paint_application_new (const char *application_id);

/* Paint Document Window クラス */
GtkWidget *paint_document_window_new (GApplication *application);

/* Paint Application クラス */
struct _PaintApplicationClass
{
	GtkApplicationClass parent_class;
};

G_DECLARE_DERIVABLE_TYPE (PaintApplication,    paint_application,     PAINT, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE     (PaintDocumentWindow, paint_document_window, PAINT, DOCUMENT_WINDOW, GtkApplicationWindow);

#define PAINT_TYPE_APPLICATION     (paint_application_get_type     ())
#define PAINT_TYPE_DOCUMENT_WINDOW (paint_document_window_get_type ())
#define PAINT_TYPE_SETTINGS        (paint_settings_get_type        ())
#define PAINT_OBJECT_CLASS_INSTALL_PROPERTY(object, PROPERTY, PSPEC) (g_object_class_install_property ((object), (PROPERTY), PSPEC (PROPERTY)))
#define PAINT_PARAM_SPEC_BOOLEAN(PROPERTY) g_param_spec_boolean (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                             PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_BOXED(PROPERTY)   g_param_spec_boxed   (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _BOXED_TYPE,                                                PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_ENUM(PROPERTY)    g_param_spec_enum    (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _ENUM_TYPE,                     PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_FLAGS(PROPERTY)   g_param_spec_flags   (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _FLAGS_TYPE,                    PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_INT(PROPERTY)     g_param_spec_int     (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_OBJECT(PROPERTY)  g_param_spec_object  (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _OBJECT_TYPE,                                               PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_POINTER(PROPERTY) g_param_spec_pointer (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                                                         PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_STRING(PROPERTY)  g_param_spec_string  (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB,                                             PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
#define PAINT_PARAM_SPEC_UINT(PROPERTY)    g_param_spec_uint    (PROPERTY ## _NAME, PROPERTY ## _NICK, PROPERTY ## _BLURB, PROPERTY ## _MINIMUM, PROPERTY ## _MAXIMUM, PROPERTY ## _DEFAULT_VALUE, PROPERTY ## _FLAGS)
