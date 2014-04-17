#include "base_service.h"
#include <zmq.h>
#include <assert.h>

enum
{
    PROP_0,
    
    IPCAM_BASE_SERVICE_CONSTRUCT_NAME = 1,
    
    N_PROPERTIES
};

typedef struct _IpcamBaseServicePrivate
{
    gchar* name;
    void* mq_context;
} IpcamBaseServicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamBaseService, ipcam_base_service, G_TYPE_OBJECT);

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static GObject *ipcam_base_service_constructor(GType self_type,
                                               guint n_properties,
                                               GObjectConstructParam *properties)
{
    GObject *obj;
    
    obj = G_OBJECT_CLASS(ipcam_base_service_parent_class)->constructor(self_type, n_properties, properties);

    return obj;
}
static void ipcam_base_service_dispose(GObject *self)
{
    static gboolean first_run = TRUE;

    if (first_run)
    {
        first_run = FALSE;
        G_OBJECT_CLASS(ipcam_base_service_parent_class)->dispose(self);
        IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(IPCAM_BASE_SERVICE(self));
        zmq_term(priv->mq_context);
    }
}
static void ipcam_base_service_finalize(GObject *self)
{
    G_OBJECT_CLASS(ipcam_base_service_parent_class)->finalize(self);
}
static void ipcam_base_service_get_property(GObject *object,
                                            guint property_id,
                                            GValue *value,
                                            GParamSpec *pspec)
{
    IpcamBaseService *self = IPCAM_BASE_SERVICE(object);
    switch(property_id)
    {
    case IPCAM_BASE_SERVICE_CONSTRUCT_NAME:
        {
            IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);
            g_value_set_string(value, priv->name);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_base_service_set_property(GObject *object,
                                            guint property_id,
                                            const GValue *value,
                                            GParamSpec *pspec)
{
    IpcamBaseService *self = IPCAM_BASE_SERVICE(object);
    switch(property_id)
    {
    case IPCAM_BASE_SERVICE_CONSTRUCT_NAME:
        {
            IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);
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
static void ipcam_base_service_init(IpcamBaseService *self)
{
    IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);
    priv->mq_context = zmq_ctx_new();
}
static void ipcam_base_service_start_impl(IpcamBaseService *self)
{
    while(TRUE)
    {
        
    }
}
static void ipcam_base_service_stop_impl(IpcamBaseService *self)
{
    
}
static void *ipcam_base_service_bind_impl(IpcamBaseService *self, gchar *address)
{
    IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);
    void *mq_socket = NULL;
    mq_socket = zmq_socket(priv->mq_context, ZMQ_ROUTER);
    assert(mq_socket);
    int rc = zmq_bind(mq_socket, address);
    assert(rc == 0);
    return mq_socket;
}
static void *ipcam_base_service_connect_impl(IpcamBaseService *self, gchar *identity, gchar *address)
{
    IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);
    void *mq_socket = NULL;
    mq_socket = zmq_socket(priv->mq_context, ZMQ_DEALER);
    assert(mq_socket);
    int rc = zmq_connect(mq_socket, address);
    assert(rc == 0);
    return mq_socket;
}
static void ipcam_base_service_class_init(IpcamBaseServiceClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    
    this_class->constructor = &ipcam_base_service_constructor;
    this_class->dispose = &ipcam_base_service_dispose;
    this_class->finalize = &ipcam_base_service_finalize;
    this_class->set_property = &ipcam_base_service_set_property;
    this_class->get_property = &ipcam_base_service_get_property;

    obj_properties[IPCAM_BASE_SERVICE_CONSTRUCT_NAME] =
        g_param_spec_string("service-name",
                            "Service construct property",
                            "Set service name",
                            "noname", // default value
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
    
    g_object_class_install_properties(this_class, N_PROPERTIES, obj_properties);

    klass->start = &ipcam_base_service_start_impl;
    klass->stop = &ipcam_base_service_stop_impl;
    klass->bind = &ipcam_base_service_bind_impl;
    klass->connect = &ipcam_base_service_connect_impl;
    klass->doregister = NULL;
    klass->unregister = NULL;
}

void ipcam_base_service_start(IpcamBaseService *self)
{
    g_return_if_fail(IPCAM_IS_BASE_SERVICE(self));
    IPCAM_BASE_SERVICE_GET_CLASS(self)->start(self);
}

void ipcam_base_service_stop(IpcamBaseService *self)
{
    g_return_if_fail(IPCAM_IS_BASE_SERVICE(self));
    IPCAM_BASE_SERVICE_GET_CLASS(self)->stop(self);
}

void* ipcam_base_service_bind(IpcamBaseService *self, gchar *address)
{
    g_return_if_fail(IPCAM_IS_BASE_SERVICE(self));
    IPCAM_BASE_SERVICE_GET_CLASS(self)->bind(self, address);
}

void* ipcam_base_service_connect(IpcamBaseService *self, gchar *identity, gchar *address)
{
    g_return_if_fail(IPCAM_IS_BASE_SERVICE(self));
    IPCAM_BASE_SERVICE_GET_CLASS(self)->connect(self, identity, address);
}

