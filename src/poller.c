#include <zmq.h>
#include "poller.h"

typedef struct _IpcamPollerListData
{
    void *mq_socket;
    gint event;
} IpcamPollerListData;

typedef IpcamPollerListData list_data;

typedef struct _IpcamPollerPrivate
{
    GList *socket_list;
    gboolean list_changed;
    zmq_pollitem_t *poll_items;
} IpcamPollerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamPoller, ipcam_poller, G_TYPE_OBJECT);

static void destroy(gpointer data)
{
    list_data *a = (list_data *)data;
    g_free(a);
}
static void ipcam_poller_dispose(GObject *self)
{
    static gboolean first_run = TRUE;
    if (first_run)
    {
        first_run = FALSE;
        IpcamPollerPrivate *priv = ipcam_poller_get_instance_private(IPCAM_POLLER(self));
        if (priv->poll_items)
        {
            g_free(priv->poll_items);
            priv->poll_items = NULL;
        }
        g_list_free_full(priv->socket_list, destroy);
        G_OBJECT_CLASS(ipcam_poller_parent_class)->dispose(self);
    }
}
static void ipcam_poller_init(IpcamPoller *self)
{
    IpcamPollerPrivate *priv = ipcam_poller_get_instance_private(self);
    priv->socket_list = NULL;
    priv->list_changed = FALSE;
    priv->poll_items = NULL;
}
static void ipcam_poller_class_init(IpcamPollerClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    this_class->dispose = &ipcam_poller_dispose;
}
void ipcam_poller_register(IpcamPoller *poller, void *mq_socket, gint event)
{
    IpcamPollerPrivate *priv = ipcam_poller_get_instance_private(poller);
    list_data *data = g_new(list_data, 1);
    data->mq_socket = mq_socket;
    data->event = event;
    
    priv->socket_list = g_list_append(priv->socket_list, (gpointer)data);
    priv->list_changed = TRUE;
}
static gint compare(gconstpointer a, gconstpointer b)
{
    list_data *a_ = (list_data *)a;
    list_data *b_ = (list_data *)b;

    return a_->mq_socket - b_->mq_socket;
}
void ipcam_poller_unregister(IpcamPoller *poller, void *mq_socket)
{
    IpcamPollerPrivate *priv = ipcam_poller_get_instance_private(poller);
    list_data *data = g_new(list_data, 1);
    data->mq_socket = mq_socket;

    GList *element = g_list_find_custom(priv->socket_list, (gconstpointer)data, compare);
    if (element)
    {
        priv->socket_list = g_list_remove(priv->socket_list, (gconstpointer)element->data);
        g_free(element->data);
        priv->list_changed = TRUE;
    }

    g_free(data);
}
gint ipcam_poller_poll(IpcamPoller *poller)
{
    IpcamPollerPrivate *priv = ipcam_poller_get_instance_private(poller);
    guint size = g_list_length(priv->socket_list);
    guint i = 0;
    list_data *data = NULL;
    if (priv->list_changed)
    {
        priv->list_changed = FALSE;
        if (priv->poll_items)
        {
            g_free(priv->poll_items);
            priv->poll_items = NULL;
        }
        if (size)
        {
            priv->poll_items = g_new(zmq_pollitem_t, size);
            for (i = 0; i < size; i++)
            {
                data = (list_data *)g_list_nth_data(priv->socket_list, i);
                priv->poll_items[i].socket = data->mq_socket;
                priv->poll_items[i].events = data->event;
            }
        }
    }
    
    return zmq_poll(priv->poll_items, size, -1);
}
guint ipcam_poller_get_sockets(IpcamPoller *poller, void *mq_sockets[])
{
    guint count = 0;
    guint i = 0;
    IpcamPollerPrivate *priv = ipcam_poller_get_instance_private(poller);
    guint size = g_list_length(priv->socket_list);
    list_data *data = NULL;

    mq_sockets = g_new(gpointer, size);
    
    for (i = 0; i < size; i++)
    {
        if (priv->poll_items[i].revents & priv->poll_items[i].events)
        {
            data = (list_data *)g_list_nth_data(priv->socket_list, i);
            mq_sockets[count] = data->mq_socket;
            count++;
        }
    }
             
    return count;
}
