#include "service.h"

enum
{
    PROP_0,
    
    IPCAM_SERVICE_CONSTRUCT_NAME = 1,
    
    N_PROPERTIES
};

typedef struct _IpcamServicePrivate
{
    gchar* name;
} IpcamServicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamService, ipcam_service, G_TYPE_OBJECT);

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };
/*
GType ipcam_service_get_type(void)
{
	static GType ipcam_service_type = 0;
	if (!ipcam_service_type)
	{
		static const GTypeInfo ipcam_service_info = {
			sizeof(IpcamServiceClass), // size of class
			NULL, // parent class initialize function
            NULL, // parent class finalize function
			(GClassInitFunc)ipcam_service_class_init, // class initialize function
			NULL, // class finalize function
            NULL, // class data
			sizeof(IpcamService), // size of instance
			0, // prealloce bytes
			(GInstanceInitFunc)ipcam_service_instance_init // instance initialize function
		};
		ipcam_service_type = g_type_register_static(G_TYPE_OBJECT, "IpcamService", &ipcam_service_info, 0);
	}
	return ipcam_service_type;
}
*/
static GObject *ipcam_service_constructor(GType self_type,
                                          guint n_properties,
                                          GObjectConstructParam *properties)
{
    GObject *obj;
    
    obj = G_OBJECT_CLASS(ipcam_service_parent_class)->constructor(self_type, n_properties, properties);

    return obj;
}
static void ipcam_service_dispose(GObject *self)
{
    static gboolean first_run = TRUE;

    if (first_run)
    {
        first_run = FALSE;
        G_OBJECT_CLASS(ipcam_service_parent_class)->dispose(self);
    }
}
static void ipcam_service_finalize(GObject *self)
{
    G_OBJECT_CLASS(ipcam_service_parent_class)->finalize(self);
}
static void ipcam_service_get_property(GObject *object,
                                       guint property_id,
                                       GValue *value,
                                       GParamSpec *pspec)
{
    IpcamService *self = IPCAM_SERVICE(object);
    switch(property_id)
    {
    case IPCAM_SERVICE_CONSTRUCT_NAME:
        {
            struct _IpcamServicePrivate *priv = ipcam_service_get_instance_private(self);
            g_value_set_string(value, priv->name);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_service_set_property(GObject *object,
                                       guint property_id,
                                       const GValue *value,
                                       GParamSpec *pspec)
{
    IpcamService *self = IPCAM_SERVICE(object);
    switch(property_id)
    {
    case IPCAM_SERVICE_CONSTRUCT_NAME:
        {
            struct _IpcamServicePrivate *priv = ipcam_service_get_instance_private(self);
            g_free(priv->name);
            priv->name = g_value_dup_string(value);
            g_print("ipcam: %s\n", priv->name);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_service_init(IpcamService *self)
{
}
static void ipcam_service_class_init(IpcamServiceClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    
    this_class->constructor = &ipcam_service_constructor;
    this_class->dispose = &ipcam_service_dispose;
    this_class->finalize = &ipcam_service_finalize;
    this_class->set_property = &ipcam_service_set_property;
    this_class->get_property = &ipcam_service_get_property;

    obj_properties[IPCAM_SERVICE_CONSTRUCT_NAME] =
        g_param_spec_string("service-name",
                            "Service construct property",
                            "Set service name",
                            "unknown-service", // default value
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
    
    g_object_class_install_properties(this_class, N_PROPERTIES, obj_properties);
}

