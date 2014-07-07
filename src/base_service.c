#include <czmq.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "base_service.h"

#define TIMEOUT_PERIOD    500 /* millsecond */

enum
{
    PROP_0,
    
    IPCAM_BASE_SERVICE_CONSTRUCT_NAME = 1,
    
    N_PROPERTIES
};

typedef struct _IpcamBaseServicePrivate
{
    gchar* name;
    zctx_t* mq_context;
    zpoller_t *poller;
	pthread_t service_thread;
    gboolean terminated;
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
    }
}
static void ipcam_base_service_finalize(GObject *self)
{
    IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(IPCAM_BASE_SERVICE(self));
    zpoller_destroy(&priv->poller);
    priv->poller = NULL;
    zctx_destroy(&priv->mq_context);
    priv->mq_context = NULL;
    g_free(priv->name);
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
    priv->mq_context = zctx_new();
    priv->poller = NULL;
	priv->service_thread = pthread_self();
    priv->terminated = FALSE;
}
static void ipcam_base_service_register_impl(IpcamBaseService *self, void *mq_socket)
{
    IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);
    if (priv->poller)
    {
        int rc = zpoller_add(priv->poller, mq_socket);
        assert(rc != -1);
    }
    else
    {
        priv->poller = zpoller_new(mq_socket, NULL);
        assert(priv->poller);
    }
}
static void ipcam_base_service_unregister_impl(IpcamBaseService *self, void *mq_socket)
{
    //IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);
}
static void ipcam_base_service_before_start(IpcamBaseService *self)
{
    if (IPCAM_BASE_SERVICE_GET_CLASS(self)->before != NULL)
        IPCAM_BASE_SERVICE_GET_CLASS(self)->before(self);
    else
        g_warning ("Class '%s' does not override the mandatory "
                   "IpcamBaseServiceClass.before() virtual function.",
                   G_OBJECT_TYPE_NAME(self));
}
static void ipcam_base_service_in_loop(IpcamBaseService *self)
{
    if (IPCAM_BASE_SERVICE_GET_CLASS(self)->in_loop != NULL)
        IPCAM_BASE_SERVICE_GET_CLASS(self)->in_loop(self);
    else
        g_warning ("Class '%s' does not override the mandatory "
                   "IpcamBaseServiceClass.in_loop() virtual function.",
                   G_OBJECT_TYPE_NAME(self));
}
static void ipcam_base_service_on_read(IpcamBaseService *self, void *mq_socket)
{
    if (IPCAM_BASE_SERVICE_GET_CLASS(self)->on_read != NULL)
        IPCAM_BASE_SERVICE_GET_CLASS(self)->on_read(self, mq_socket);
    else
        g_warning ("Class '%s' does not override the mandatory "
                   "IpcamBaseServiceClass.on_read() virtual function.",
                   G_OBJECT_TYPE_NAME(self));
}

static void ipcam_base_service_do_poll(IpcamBaseService *self)
{
    IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);
    if (priv->poller)
    {
        void *which = zpoller_wait(priv->poller, TIMEOUT_PERIOD);
        //g_return_if_fail(!zpoller_expired(priv->poller));
        if (zpoller_terminated(priv->poller))
        {
            priv->terminated = TRUE;
        }
        else if (which)
        {
            ipcam_base_service_on_read(self, which);
        }
        else
        {
            // do nothing
        }
    }
    else
    {
        zclock_sleep(TIMEOUT_PERIOD);
    }
}

static void ipcam_base_service_start_impl(IpcamBaseService *self)
{
    IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);

	if (!pthread_equal(pthread_self(), priv->service_thread)) {
		g_warn_if_reached();
		return;
	}

	ipcam_base_service_before_start(self);
    while (!priv->terminated)
    {
        ipcam_base_service_do_poll(self);
        ipcam_base_service_in_loop(self);
    }
}
static void ipcam_base_service_stop_impl(IpcamBaseService *self)
{
    IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);
    priv->terminated = TRUE;
}
static void *ipcam_base_service_bind_impl(IpcamBaseService *self, const gchar *address)
{
    IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);
    void *mq_socket = NULL;
    mq_socket = zsocket_new(priv->mq_context, ZMQ_ROUTER);
    assert(mq_socket);
    int rc = zsocket_bind(mq_socket, address);
    ipcam_base_service_register_impl(self, mq_socket);
    return mq_socket;
}
static void *ipcam_base_service_connect_impl(IpcamBaseService *self, const gchar *identity, const gchar *address)
{
    IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(self);
    void *mq_socket = NULL;
    mq_socket = zsocket_new(priv->mq_context, ZMQ_DEALER);
    assert(mq_socket);
    zsocket_set_identity(mq_socket, identity);
    int rc = zsocket_connect(mq_socket, address);
    ipcam_base_service_register_impl(self, mq_socket);
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
        g_param_spec_string("name",
                            "Service construct property",
                            "Set service name",
                            "noname", // default value
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
    
    g_object_class_install_properties(this_class, N_PROPERTIES, obj_properties);

    klass->start = &ipcam_base_service_start_impl;
    klass->stop = &ipcam_base_service_stop_impl;
    klass->bind = &ipcam_base_service_bind_impl;
    klass->connect = &ipcam_base_service_connect_impl;
    klass->before = NULL;
    klass->in_loop = NULL;
    klass->on_read = NULL;
}

void ipcam_base_service_start(IpcamBaseService *base_service)
{
    g_return_if_fail(IPCAM_IS_BASE_SERVICE(base_service));
    IPCAM_BASE_SERVICE_GET_CLASS(base_service)->start(base_service);
}

void ipcam_base_service_stop(IpcamBaseService *base_service)
{
    g_return_if_fail(IPCAM_IS_BASE_SERVICE(base_service));
    IPCAM_BASE_SERVICE_GET_CLASS(base_service)->stop(base_service);
}

void* ipcam_base_service_bind(IpcamBaseService *base_service, const gchar *address)
{
    g_return_val_if_fail(IPCAM_IS_BASE_SERVICE(base_service), NULL);
    return IPCAM_BASE_SERVICE_GET_CLASS(base_service)->bind(base_service, address);
}

void* ipcam_base_service_connect(IpcamBaseService *base_service, const gchar *identity, const gchar *address)
{
    g_return_val_if_fail(IPCAM_IS_BASE_SERVICE(base_service), NULL);
    return IPCAM_BASE_SERVICE_GET_CLASS(base_service)->connect(base_service, identity, address);
}

pthread_t ipcam_base_service_get_thread(IpcamBaseService *base_service)
{
    IpcamBaseServicePrivate *priv = ipcam_base_service_get_instance_private(base_service);

	return priv->service_thread;
}
