/* Copyright (C) 2025 Taichi Murakami.
ドキュメント ウィンドウ クラス。
このクラスはユーザーが開いたドキュメントを所有する。
このクラスはユーザーがドキュメントを編集する方法を提供する。
*/
#include <gtk/gtk.h>
#include "paint.h"
#define PAINT_DOCUMENT_PROPERTY_HADJUSTMENT_NAME          "hadjustment"
#define PAINT_DOCUMENT_PROPERTY_HADJUSTMENT_NICK          "Horizontal Adjustment"
#define PAINT_DOCUMENT_PROPERTY_HADJUSTMENT_BLURB         "Horizontal Adjustment"
#define PAINT_DOCUMENT_PROPERTY_HADJUSTMENT_DEFAULT_VALUE NULL
#define PAINT_DOCUMENT_PROPERTY_HADJUSTMENT_FLAGS         G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_DOCUMENT_PROPERTY_HFLIP_NAME                "hflip"
#define PAINT_DOCUMENT_PROPERTY_HFLIP_NICK                "Horizontal Flip"
#define PAINT_DOCUMENT_PROPERTY_HFLIP_BLURB               "Horizontal Flip"
#define PAINT_DOCUMENT_PROPERTY_HFLIP_DEFAULT_VALUE       FALSE
#define PAINT_DOCUMENT_PROPERTY_HFLIP_FLAGS               G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_DOCUMENT_PROPERTY_ROTATION_NAME             "rotation"
#define PAINT_DOCUMENT_PROPERTY_ROTATION_NICK             "Rotation DEG"
#define PAINT_DOCUMENT_PROPERTY_ROTATION_BLURB            "Rotation in degrees."
#define PAINT_DOCUMENT_PROPERTY_ROTATION_MINIMUM          G_MININT16
#define PAINT_DOCUMENT_PROPERTY_ROTATION_MAXIMUM          G_MAXINT16
#define PAINT_DOCUMENT_PROPERTY_ROTATION_DEFAULT_VALUE    0
#define PAINT_DOCUMENT_PROPERTY_ROTATION_FLAGS            G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_DOCUMENT_PROPERTY_VADJUSTMENT_NAME          "vadjustment"
#define PAINT_DOCUMENT_PROPERTY_VADJUSTMENT_NICK          "Vertical Adjustment"
#define PAINT_DOCUMENT_PROPERTY_VADJUSTMENT_BLURB         "Vertical Adjustment"
#define PAINT_DOCUMENT_PROPERTY_VADJUSTMENT_DEFAULT_VALUE NULL
#define PAINT_DOCUMENT_PROPERTY_VADJUSTMENT_FLAGS         G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_DOCUMENT_PROPERTY_VFLIP_NAME                "vflip"
#define PAINT_DOCUMENT_PROPERTY_VFLIP_NICK                "Vertical Flip"
#define PAINT_DOCUMENT_PROPERTY_VFLIP_BLURB               "Vertical Flip"
#define PAINT_DOCUMENT_PROPERTY_VFLIP_DEFAULT_VALUE       FALSE
#define PAINT_DOCUMENT_PROPERTY_VFLIP_FLAGS               G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_DOCUMENT_PROPERTY_ZOOM_NAME                 "zoom"
#define PAINT_DOCUMENT_PROPERTY_ZOOM_NICK                 "Zoom%%"
#define PAINT_DOCUMENT_PROPERTY_ZOOM_BLURB                "Zoom in percent."
#define PAINT_DOCUMENT_PROPERTY_ZOOM_MINIMUM              1
#define PAINT_DOCUMENT_PROPERTY_ZOOM_MAXIMUM              G_MAXUINT16
#define PAINT_DOCUMENT_PROPERTY_ZOOM_DEFAULT_VALUE        100
#define PAINT_DOCUMENT_PROPERTY_ZOOM_FLAGS                G_PARAM_READWRITE | G_PARAM_CONSTRUCT
#define PAINT_DOCUMENT_STEP_INCREMENT                     10
#define PAINT_DOCUMENT_PAGE_INCRENENT                     10
#define PAINT_DOCUMENT_SIGNAL_ADJUSTMENT                  "value-changed"

/* Document クラスのプロパティ */
enum _PaintDocumentProperty
{
	PAINT_DOCUMENT_PROPERTY_0,
	PAINT_DOCUMENT_PROPERTY_HADJUSTMENT,
	PAINT_DOCUMENT_PROPERTY_HFLIP,
	PAINT_DOCUMENT_PROPERTY_ROTATION,
	PAINT_DOCUMENT_PROPERTY_VADJUSTMENT,
	PAINT_DOCUMENT_PROPERTY_VFLIP,
	PAINT_DOCUMENT_PROPERTY_ZOOM,
};

/* Document クラスのインスタンス */
struct _PaintDocument
{
	GtkDrawingArea   parent_instance;
	GFile           *file;
	GtkAdjustment   *hadjustment;
	GtkAdjustment   *vadjustment;
	cairo_surface_t *surface;
	gdouble          surface_x;
	gdouble          surface_y;
	gulong           hadjustment_handler;
	gulong           vadjustment_handler;
	gint             height;
	gint             margin;
	gint             width;
	gint16           rotation;
	guint16          zoom;
	guint8           drawing;
	guint8           hflip;
	guint8           loading;
	guint8           pressed;
	guint8           vflip;
};

static void
paint_document_change_adjustment (GtkAdjustment *, gpointer);
static void
paint_document_class_init (PaintDocumentClass *);
static void
paint_document_class_init_area (GtkDrawingAreaClass *);
static void
paint_document_class_init_object (GObjectClass *);
static void
paint_document_destroy (PaintDocument *);
static void
paint_document_dispose (GObject *);
static void
paint_document_draw (PaintDocument *);
static void
paint_document_draw_line (PaintDocument *, gdouble, gdouble);
static void
paint_document_draw_surface (GtkDrawingArea *, cairo_t *, gint, gint, gpointer);
static void
paint_document_get_property (GObject *, guint, GValue *, GParamSpec *);
static void
paint_document_init (PaintDocument *);
static void
paint_document_init_controller (PaintDocument *);
static void
paint_document_init_gesture (PaintDocument *);
static void
paint_document_load (PaintDocument *document, cairo_t *cairo);
static void
paint_document_move_motion (GtkEventControllerMotion *, gdouble, gdouble, gpointer);
static void
paint_document_move_pressed (GtkGestureClick *, gint, gdouble, gdouble, gpointer);
static void
paint_document_move_released (GtkGestureClick *, gint, gdouble, gdouble, gpointer);
static void
paint_document_resize (GtkDrawingArea *, gint, gint);
static void
paint_document_set_page_size (PaintDocument *, gint, gint);
static void
paint_document_set_property (GObject *, guint, const GValue *, GParamSpec *);
static void
paint_document_to_surface (PaintDocument *, gdouble *, gdouble *);
static void
paint_document_update_adjustment (PaintDocument *);

G_DEFINE_FINAL_TYPE (PaintDocument, paint_document, GTK_TYPE_DRAWING_AREA);

void
paint_document_change_adjustment (GtkAdjustment *, gpointer self)
{
	paint_document_draw (PAINT_DOCUMENT (self));
}

void
paint_document_class_init (PaintDocumentClass *document)
{
	paint_document_class_init_object (G_OBJECT_CLASS (document));
	paint_document_class_init_area (GTK_DRAWING_AREA_CLASS (document));
}

void
paint_document_class_init_area (GtkDrawingAreaClass *area)
{
	area->resize = paint_document_resize;
}

void
paint_document_class_init_object (GObjectClass *object)
{
	object->dispose      = paint_document_dispose;
	object->get_property = paint_document_get_property;
	object->set_property = paint_document_set_property;
	g_object_class_install_property (object, PAINT_DOCUMENT_PROPERTY_HADJUSTMENT, PAINT_PARAM_SPEC_OBJECT  (PAINT_DOCUMENT_PROPERTY_HADJUSTMENT, GTK_TYPE_ADJUSTMENT));
	g_object_class_install_property (object, PAINT_DOCUMENT_PROPERTY_HFLIP,       PAINT_PARAM_SPEC_BOOLEAN (PAINT_DOCUMENT_PROPERTY_HFLIP));
	g_object_class_install_property (object, PAINT_DOCUMENT_PROPERTY_ROTATION,    PAINT_PARAM_SPEC_INT     (PAINT_DOCUMENT_PROPERTY_ROTATION));
	g_object_class_install_property (object, PAINT_DOCUMENT_PROPERTY_VADJUSTMENT, PAINT_PARAM_SPEC_OBJECT  (PAINT_DOCUMENT_PROPERTY_VADJUSTMENT, GTK_TYPE_ADJUSTMENT));
	g_object_class_install_property (object, PAINT_DOCUMENT_PROPERTY_VFLIP,       PAINT_PARAM_SPEC_BOOLEAN (PAINT_DOCUMENT_PROPERTY_VFLIP));
	g_object_class_install_property (object, PAINT_DOCUMENT_PROPERTY_ZOOM,        PAINT_PARAM_SPEC_UINT    (PAINT_DOCUMENT_PROPERTY_ZOOM));
}

void
paint_document_destroy (PaintDocument *document)
{
	paint_document_set_hadjustment (document, NULL);
	paint_document_set_vadjustment (document, NULL);
	g_clear_pointer (&document->surface, cairo_surface_destroy);
}

void
paint_document_dispose (GObject *object)
{
	paint_document_destroy (PAINT_DOCUMENT (object));
	G_OBJECT_CLASS (paint_document_parent_class)->dispose (object);
}

void
paint_document_draw (PaintDocument *document)
{
	if (!document->drawing)
	{
		document->drawing = TRUE;
		gtk_widget_queue_draw (GTK_WIDGET (document));
	}
}

void
paint_document_draw_line (PaintDocument *document, gdouble x, gdouble y)
{
	cairo_t *cairo;

	if (document->surface)
	{
		cairo = cairo_create (document->surface);

		if (cairo)
		{
			cairo_move_to (cairo, document->surface_x, document->surface_y);
			cairo_line_to (cairo, x, y);
			cairo_stroke (cairo);
			cairo_destroy (cairo);
		}
	}
}

void
paint_document_draw_surface (GtkDrawingArea *, cairo_t *cairo, gint, gint, gpointer self)
{
	PaintDocument *document;
	document = PAINT_DOCUMENT (self);
	document->drawing = TRUE;

	if (document->loading)
	{
		paint_document_load (document, cairo);
		document->loading = FALSE;
	}
	if (document->surface)
	{
		cairo_translate (cairo, gtk_adjustment_get_value (document->hadjustment), gtk_adjustment_get_value (document->vadjustment));
		cairo_set_source_surface (cairo, document->surface, 0, 0);
		cairo_paint (cairo);
	}

	document->drawing = FALSE;
}

GFile *
paint_document_get_file (PaintDocument *document)
{
	return document->file;
}

GtkAdjustment *
paint_document_get_hadjustment (PaintDocument *document)
{
	return document->hadjustment;
}

gboolean
paint_document_get_hflip (PaintDocument *document)
{
	return document->hflip;
}

void
paint_document_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	switch (property_id)
	{
	case PAINT_DOCUMENT_PROPERTY_HADJUSTMENT:
		g_value_set_object (value, PAINT_DOCUMENT (object)->hadjustment);
		break;
	case PAINT_DOCUMENT_PROPERTY_HFLIP:
		g_value_set_boolean (value, PAINT_DOCUMENT (object)->hflip);
		break;
	case PAINT_DOCUMENT_PROPERTY_ROTATION:
		g_value_set_int (value, PAINT_DOCUMENT (object)->rotation);
		break;
	case PAINT_DOCUMENT_PROPERTY_VADJUSTMENT:
		g_value_set_object (value, PAINT_DOCUMENT (object)->vadjustment);
		break;
	case PAINT_DOCUMENT_PROPERTY_VFLIP:
		g_value_set_boolean (value, PAINT_DOCUMENT (object)->vflip);
		break;
	case PAINT_DOCUMENT_PROPERTY_ZOOM:
		g_value_set_uint (value, PAINT_DOCUMENT (object)->zoom);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

gint
paint_document_get_rotation (PaintDocument *document)
{
	return document->rotation;
}

GtkAdjustment *
paint_document_get_vadjustment (PaintDocument *document)
{
	return document->vadjustment;
}

gboolean
paint_document_get_vflip (PaintDocument *document)
{
	return document->vflip;
}

guint
paint_document_get_zoom (PaintDocument *document)
{
	return document->zoom;
}

void
paint_document_init (PaintDocument *document)
{
	paint_document_init_controller (document);
	paint_document_init_gesture    (document);
	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (document), paint_document_draw_surface, document, NULL);
}

void
paint_document_init_controller (PaintDocument *document)
{
	GtkEventController *controller;
	controller = gtk_event_controller_motion_new ();

	if (controller)
	{
		g_signal_connect (controller, "motion", G_CALLBACK (paint_document_move_motion), document);
		gtk_widget_add_controller (GTK_WIDGET (document), controller);
	}
}

void
paint_document_init_gesture (PaintDocument *document)
{
	GtkGesture *gesture;
	gesture = gtk_gesture_click_new ();

	if (gesture)
	{
		g_signal_connect (gesture, "pressed",  G_CALLBACK (paint_document_move_pressed),   document);
		g_signal_connect (gesture, "released", G_CALLBACK (paint_document_move_released), document);
		gtk_widget_add_controller (GTK_WIDGET (document), GTK_EVENT_CONTROLLER (gesture));
	}
}

void
paint_document_load (PaintDocument *document, cairo_t *cairo)
{
	GError *error;

	if (document->surface)
	{
		cairo_surface_destroy (document->surface);
	}

	document->surface = paint_surface_new_from_file (document->file, cairo, &error);
	paint_document_update_adjustment (document);

	if (error)
	{
		paint_error_dialog_show (GTK_WINDOW (document), error);
		g_clear_error (&error);
	}
}

void
paint_document_move_motion (GtkEventControllerMotion *, gdouble x, gdouble y, gpointer self)
{
	PaintDocument *document;
	document = PAINT_DOCUMENT (self);

	if (document->pressed)
	{
		paint_document_to_surface (document, &x, &y);
		paint_document_draw_line (document, x, y);
		paint_document_draw (document);
	}

	document->surface_x = x;
	document->surface_y = y;
}

void
paint_document_move_pressed (GtkGestureClick *click, gint, gdouble x, gdouble y, gpointer self)
{
	PaintDocument *document;

	if (gtk_gesture_single_get_current_button (GTK_GESTURE_SINGLE (click)) == GDK_BUTTON_PRIMARY)
	{
		document = PAINT_DOCUMENT (self);
		paint_document_to_surface (document, &x, &y);
		document->surface_x = x;
		document->surface_y = y;
		document->pressed = TRUE;
	}
}

void
paint_document_move_released (GtkGestureClick *click, gint, gdouble x, gdouble y, gpointer self)
{
	PaintDocument *document;

	if (gtk_gesture_single_get_current_button (GTK_GESTURE_SINGLE (click)) == GDK_BUTTON_PRIMARY)
	{
		document = PAINT_DOCUMENT (self);
		paint_document_to_surface (document, &x, &y);
		document->surface_x = x;
		document->surface_y = y;
		document->pressed = FALSE;
	}
}

GtkWidget *
paint_document_new (GtkAdjustment *hadjustment, GtkAdjustment *vadjustment)
{
	return g_object_new (PAINT_TYPE_DOCUMENT,
		"hadjustment", hadjustment,
		"vadjustment", vadjustment,
		NULL);
}

void
paint_document_reset (PaintDocument *document)
{
	g_clear_object (&document->surface);
}

void
paint_document_resize (GtkDrawingArea *area, gint width, gint height)
{
	paint_document_set_page_size (PAINT_DOCUMENT (area), width, height);
}

void
paint_document_save (PaintDocument *document, const GFile *file)
{
}

void
paint_document_set_file (PaintDocument *document, GFile *file)
{
	if (document->file)
	{
		g_object_unref (document->file);
	}
	if (file)
	{
		document->file = g_object_ref (file);
		document->loading = TRUE;
	}
	else
	{
		document->file = NULL;
		document->loading = FALSE;
	}

	paint_document_draw (document);
}

void
paint_document_set_hadjustment (PaintDocument *document, GtkAdjustment *hadjustment)
{
	if (document->hadjustment != hadjustment)
	{
		if (document->hadjustment)
		{
			g_signal_handler_disconnect (document->hadjustment, document->hadjustment_handler);
			g_object_unref (document->hadjustment);
		}
		if (hadjustment)
		{
			document->hadjustment = g_object_ref (hadjustment);
			document->hadjustment_handler = g_signal_connect (hadjustment, PAINT_DOCUMENT_SIGNAL_ADJUSTMENT, G_CALLBACK (paint_document_change_adjustment), document);
		}
		else
		{
			document->hadjustment = NULL;
			document->hadjustment_handler = 0;
		}
	}
}

void
paint_document_set_hflip (PaintDocument *document, gboolean hflip)
{
	document->hflip = hflip != 0;
}

void
paint_document_set_page_size (PaintDocument *document, gint width, gint height)
{
	if (document->hadjustment)
	{
		gtk_adjustment_set_page_size (document->hadjustment, width);
	}
	if (document->vadjustment)
	{
		gtk_adjustment_set_page_size (document->vadjustment, height);
	}
}

void
paint_document_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	switch (property_id)
	{
	case PAINT_DOCUMENT_PROPERTY_HADJUSTMENT:
		paint_document_set_hadjustment (PAINT_DOCUMENT (object), g_value_get_object (value));
		break;
	case PAINT_DOCUMENT_PROPERTY_HFLIP:
		paint_document_set_hflip (PAINT_DOCUMENT (object), g_value_get_boolean (value));
		break;
	case PAINT_DOCUMENT_PROPERTY_ROTATION:
		paint_document_set_rotation (PAINT_DOCUMENT (object), g_value_get_int (value));
		break;
	case PAINT_DOCUMENT_PROPERTY_VADJUSTMENT:
		paint_document_set_vadjustment (PAINT_DOCUMENT (object), g_value_get_object (value));
		break;
	case PAINT_DOCUMENT_PROPERTY_VFLIP:
		paint_document_set_vflip (PAINT_DOCUMENT (object), g_value_get_boolean (value));
		break;
	case PAINT_DOCUMENT_PROPERTY_ZOOM:
		paint_document_set_zoom (PAINT_DOCUMENT (object), g_value_get_uint (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

void
paint_document_set_rotation (PaintDocument *document, gint canvas_rotation)
{
	document->rotation = CLAMP_PROPERTY (canvas_rotation, PAINT_DOCUMENT_PROPERTY_ROTATION);
}

void
paint_document_set_vadjustment (PaintDocument *document, GtkAdjustment *vadjustment)
{
	if (document->vadjustment != vadjustment)
	{
		if (document->vadjustment)
		{
			g_signal_handler_disconnect (document->vadjustment, document->vadjustment_handler);
			g_object_unref (document->vadjustment);
		}
		if (vadjustment)
		{
			document->vadjustment = g_object_ref (vadjustment);
			document->vadjustment_handler = g_signal_connect (vadjustment, PAINT_DOCUMENT_SIGNAL_ADJUSTMENT, G_CALLBACK (paint_document_change_adjustment), document);
		}
		else
		{
			document->vadjustment = NULL;
			document->vadjustment_handler = 0;
		}
	}
}

void
paint_document_set_vflip (PaintDocument *document, gboolean vflip)
{
	document->vflip = vflip != 0;
}

void
paint_document_set_zoom (PaintDocument *document, guint canvas_zoom)
{
	document->zoom = CLAMP_PROPERTY (canvas_zoom, PAINT_DOCUMENT_PROPERTY_ZOOM);
}

void
paint_document_to_surface (PaintDocument *document, gdouble *x, gdouble *y)
{
	double zoom;
	zoom = document->zoom / (double) PAINT_ZOOM_DEFAULT_VALUE;
	*x *= zoom;
	*y *= zoom;
}

void
paint_document_update_adjustment (PaintDocument *document)
{
	cairo_surface_t *image;
	gdouble margin;
	document->width = 0;
	document->height = 0;
	margin = document->margin;

	if (document->surface)
	{
		image = cairo_surface_map_to_image (document->surface, NULL);

		if (image)
		{
			document->width = cairo_image_surface_get_width (image);
			document->height = cairo_image_surface_get_height (image);
			cairo_surface_unmap_image (document->surface, image);
		}
	}
	if (document->hadjustment)
	{
		gtk_adjustment_set_lower (document->hadjustment, -margin);
		gtk_adjustment_set_upper (document->hadjustment, margin + document->width);
	}
	if (document->vadjustment)
	{
		gtk_adjustment_set_lower (document->vadjustment, -margin);
		gtk_adjustment_set_upper (document->vadjustment, margin + document->height);
	}
}
