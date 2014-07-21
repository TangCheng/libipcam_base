#include <assert.h>
#include <czmq.h>
#include "service.h"
#include "socket_manager.h"

typedef struct _IpcamServicePrivate
{
    IpcamSocketManager *socket_manager;
    GList *publish_lists;
} IpcamServicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamService, ipcam_service, IPCAM_BASE_SERVICE_TYPE);

static void ipcam_service_stop_impl(IpcamBaseService *service);
static void ipcam_service_on_read_impl(IpcamBaseService *service, void *mq_socket);

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
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(IPCAM_SERVICE(self));
    g_list_free_full(priv->publish_lists, g_free);
    g_object_unref(priv->socket_manager);
    G_OBJECT_CLASS(ipcam_service_parent_class)->finalize(self);
}
static void ipcam_service_init(IpcamService *self)
{
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(IPCAM_SERVICE(self));
    priv->socket_manager = g_object_new(IPCAM_SOCKET_MANAGER_TYPE, NULL);
    priv->publish_lists = NULL;
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

    klass->server_receive_string = NULL;
    klass->client_receive_string = NULL;
}
static void ipcam_service_server_receive_string(IpcamService *self, const gchar *name, const gchar *client_id, const gchar *string)
{
    if (IPCAM_SERVICE_GET_CLASS(self)->server_receive_string != NULL)
        IPCAM_SERVICE_GET_CLASS(self)->server_receive_string(self, name, client_id, string);
    else
        g_warning ("Class '%s' does not override the mandatory "
                   "IpcamServiceClass.server_receive_string() virtual function.",
                   G_OBJECT_TYPE_NAME(self));
}static void ipcam_service_client_receive_string(IpcamService *self, const gchar *name, const gchar *string)
{
    if (IPCAM_SERVICE_GET_CLASS(self)->client_receive_string != NULL)
        IPCAM_SERVICE_GET_CLASS(self)->client_receive_string(self, name, string);
    else
        g_warning ("Class '%s' does not override the mandatory "
                   "IpcamServiceClass.client_receive_string() virtual function.",
                   G_OBJECT_TYPE_NAME(self));
}
static void ipcam_service_stop_impl(IpcamBaseService *self)
{
    IpcamService *service = IPCAM_SERVICE(self);
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);
    ipcam_socket_manager_close_all_socket(priv->socket_manager);
        
    IpcamBaseServiceClass *parent_class = g_type_class_peek_parent(IPCAM_SERVICE_GET_CLASS(service));
    if (parent_class->stop)
    {
        parent_class->stop(IPCAM_BASE_SERVICE(service));
    }
}
static void ipcam_service_on_read_impl(IpcamBaseService *self, void *mq_socket)
{
    gchar *name = NULL;
    gchar *string = NULL;
    gchar *client_id = NULL;
    gint type;
    IpcamService *service = IPCAM_SERVICE(self);
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);
    ipcam_socket_manager_get_by_socket(priv->socket_manager, mq_socket, &name, &type);
    g_return_if_fail(name);

    switch(type)
    {
    case IPCAM_SOCKET_TYPE_SERVER:
        client_id = zstr_recv(mq_socket);
        string = zstr_recv(mq_socket);
        ipcam_service_server_receive_string(service, name, client_id, string);
        break;
    case IPCAM_SOCKET_TYPE_SUBSCRIBER:
    case IPCAM_SOCKET_TYPE_CLIENT:
        string = zstr_recv(mq_socket);
        ipcam_service_client_receive_string(service, name, string);
        break;
    default:
        g_print("unkonw type\n");
        break;
    }
    
    g_free(name);
    zstr_free(&string);
    zstr_free(&client_id);
}
static gint zmq_send_strings(void *socket, const gchar *strings[])
{
    zmsg_t *msg = zmsg_new();
    gint i = 0;
    while (strings[i])
    {
        zmsg_addstr(msg, strings[i]);
        i++;
    }
    gint ret = zmsg_send(&msg, socket);
    return ret;
}
gboolean ipcam_service_send_strings(IpcamService *service,
                                    const gchar *name,
                                    const gchar *strings[],
                                    const gchar *client_id)
{
    gboolean ret = FALSE;
    gint type;
    void *mq_socket = NULL;
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);
    g_return_val_if_fail(ipcam_socket_manager_get_by_name(priv->socket_manager, name, &type, &mq_socket), FALSE);
    switch(type)
    {
    case IPCAM_SOCKET_TYPE_SERVER:
        g_return_val_if_fail(client_id, FALSE);
        zstr_sendm(mq_socket, client_id);
        zmq_send_strings(mq_socket, strings);
        ret = TRUE;
        break;
    case IPCAM_SOCKET_TYPE_PUBLISHER:
    case IPCAM_SOCKET_TYPE_CLIENT:
        zmq_send_strings(mq_socket, strings);
        ret = TRUE;
        break;
    default:
        break;
    }
    return ret;
}
gboolean ipcam_service_is_server(IpcamService *service, const gchar *name)
{
    gint type;
    void *mq_socket = NULL;
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);
    g_return_val_if_fail(ipcam_socket_manager_get_by_name(priv->socket_manager, name, &type, &mq_socket), FALSE);
    return type == IPCAM_SOCKET_TYPE_SERVER || type == IPCAM_SOCKET_TYPE_PUBLISHER;
}
gboolean ipcam_service_is_client(IpcamService *service, const gchar *name)
{
    gint type;
    void *mq_socket = NULL;
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);
    g_return_val_if_fail(ipcam_socket_manager_get_by_name(priv->socket_manager, name, &type, &mq_socket), FALSE);
    return type == IPCAM_SOCKET_TYPE_CLIENT || type == IPCAM_SOCKET_TYPE_SUBSCRIBER;
}
gboolean ipcam_service_connect_by_name(IpcamService *service,
                                       const gchar *name,
                                       const gchar *address,
                                       const gchar *client_id)
{
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);
    g_return_val_if_fail(!ipcam_socket_manager_has_name(priv->socket_manager, name), FALSE);
    void *mq_socket = ipcam_base_service_connect(IPCAM_BASE_SERVICE(service), client_id, address);
    return ipcam_socket_manager_add(priv->socket_manager, name, IPCAM_SOCKET_TYPE_CLIENT, mq_socket);
}
gboolean ipcam_service_subscirbe_by_name(IpcamService *service,
                                         const gchar *name,
                                         const gchar *address)
{
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);
    g_return_val_if_fail(!ipcam_socket_manager_has_name(priv->socket_manager, name), FALSE);
    void *mq_socket = ipcam_base_service_subscribe(IPCAM_BASE_SERVICE(service), address);
    return ipcam_socket_manager_add(priv->socket_manager, name, IPCAM_SOCKET_TYPE_SUBSCRIBER, mq_socket);
}
gboolean ipcam_service_bind_by_name(IpcamService *service, const gchar *name, const gchar *address)
{
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);
    g_return_val_if_fail(!ipcam_socket_manager_has_name(priv->socket_manager, name), FALSE);
    void *mq_socket = ipcam_base_service_bind(IPCAM_BASE_SERVICE(service), address);
    return ipcam_socket_manager_add(priv->socket_manager, name, IPCAM_SOCKET_TYPE_SERVER, mq_socket);
}
gboolean ipcam_service_publish_by_name(IpcamService *service, const gchar *name, const gchar *address)
{
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);
    g_return_val_if_fail(!ipcam_socket_manager_has_name(priv->socket_manager, name), FALSE);
    void *mq_socket = ipcam_base_service_publish(IPCAM_BASE_SERVICE(service), address);
    priv->publish_lists = g_list_append(priv->publish_lists, g_strdup(name));
    return ipcam_socket_manager_add(priv->socket_manager, name, IPCAM_SOCKET_TYPE_PUBLISHER, mq_socket);
}

GList *ipcam_service_get_publish_names(IpcamService *service)
{
    IpcamServicePrivate *priv = ipcam_service_get_instance_private(service);

    return priv->publish_lists;
}
