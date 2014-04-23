#include <assert.h>
#include "service.h"
#include "socket_manager.h"

typedef struct _IpcamServicePrivate
{
    IpcamSocketManager *socket_manager;
} IpcamServicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamService, ipcam_service, IPCAM_BASE_SERVICE_TYPE);

static void ipcam_service_stop_impl(IpcamService *service);
static void ipcam_service_on_read_impl(IpcamService *service, void *mq_socket);

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
        IpcamServicePrivate *priv = ipcam_service_get_instance_private(IPCAM_SERVICE(self));
        g_object_unref(priv->socket_manager);
        G_OBJECT_CLASS(ipcam_service_parent_class)->dispose(self);
    }
}
static void ipcam_service_finalize(GObject *self)
{
    G_OBJECT_CLASS(ipcam_service_parent_class)->finalize(self);
}
static void ipcam_service_init(IpcamService *self)
{
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(IPCAM_SERVICE(self));
    priv->socket_manager = g_object_new(IPCAM_SOCKET_MANAGER_TYPE, NULL);
}
static void ipcam_service_class_init(IpcamServiceClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    
    this_class->constructor = &ipcam_service_constructor;
    this_class->dispose = &ipcam_service_dispose;
    this_class->finalize = &ipcam_service_finalize;

    IpcamBaseServiceClass *base_service_class = IPCAM_BASE_SERVICE_CLASS(klass);
    base_service_class->stop = &ipcam_service_stop_impl;
    base_service_class->on_read = &ipcam_service_on_read_impl;
}
static void ipcam_service_stop_impl(IpcamService *service)
{
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);
    ipcam_socket_manager_close_all_socket(priv->socket_manager);
        
    IpcamBaseServiceClass *parent_class = g_type_class_peek_parent(IPCAM_SERVICE_GET_CLASS(service));
    if (parent_class->stop)
    {
        parent_class->stop(IPCAM_BASE_SERVICE(service));
    }
}
static void ipcam_service_on_read_impl(IpcamService *service, void *mq_socket)
{
    gchar *name = NULL;
    gint type;
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);
    ipcam_socket_manager_get_by_socket(priv->socket_manager, mq_socket, name, &type);
    g_return_if_fail(name);

    switch(type)
    {
    case IPCAM_SOCKET_TYPE_SERVER:
        // ToDo
        break;
    case IPCAM_SOCKET_TYPE_CLIENT:
        // ToDo
        break;
    defalut:
        break;
    }
    
    g_free(name);

    IpcamBaseServiceClass *parent_class = g_type_class_peek_parent(IPCAM_SERVICE_GET_CLASS(service));
    if (parent_class->on_read)
    {
        parent_class->on_read(IPCAM_BASE_SERVICE(service), mq_socket);
    }
}


