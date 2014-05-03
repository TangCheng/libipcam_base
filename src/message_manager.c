#include "message_manager.h"
#include <assert.h>

typedef struct _IpcamMessageManagerHashValue
{
    gint time;
    guint timeout;
    GObject *obj;
    MsgHandler callback;
} IpcamMessageManagerHashValue;

typedef IpcamMessageManagerHashValue hash_value;

typedef struct _IpcamMessageManagerPrivate
{
    GHashTable *msg_hash;
} IpcamMessageManagerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamMessageManager, ipcam_message_manager, G_TYPE_OBJECT);

static void ipcam_message_manager_dispose(GObject *self)
{
    static gboolean first_run = TRUE;
    if (first_run)
    {
        first_run = FALSE;
        G_OBJECT_CLASS(ipcam_message_manager_parent_class)->dispose(self);
        IpcamMessageManagerPrivate *priv =
            ipcam_message_manager_get_instance_private(IPCAM_MESSAGE_MANAGER(self));
        g_hash_table_remove_all(priv->msg_hash);
        g_hash_table_destroy(priv->msg_hash);
    }
}
static void destroy_notify(gpointer data)
{
    hash_value *value = (hash_value *)data;
    g_free(value);
}
static void ipcam_message_manager_init(IpcamMessageManager *self)
{
    IpcamMessageManagerPrivate *priv = ipcam_message_manager_get_instance_private(self);
    priv->msg_hash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (GDestroyNotify)destroy_notify);
    assert(priv->msg_hash);
}
static void ipcam_message_manager_class_init(IpcamMessageManagerClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    this_class->dispose = &ipcam_message_manager_dispose;
}
gboolean ipcam_message_manager_register(IpcamMessageManager *message_manager,
                                        IpcamMessage *message,
                                        GObject *obj,
                                        MsgHandler handler,
                                        guint timeout)
{
    g_return_val_if_fail(ipcam_message_is_request(message), FALSE);
    g_return_val_if_fail(handler, FALSE);

    gboolean ret = FALSE;
    IpcamMessageManagerPrivate *priv = ipcam_message_manager_get_instance_private(message_manager);
    gchar *msg_id;
    g_object_get(G_OBJECT(message), "id", &msg_id, NULL);
    if (!g_hash_table_contains(priv->msg_hash, msg_id))
    {
        hash_value *value = g_new(hash_value, 1);
        value->time = time((time_t *)NULL);
        value->timeout = timeout;
        value->obj = obj;
        value->callback = handler;

        ret = g_hash_table_insert(priv->msg_hash, (gpointer)msg_id, (gpointer)value);
    }

    g_free(msg_id);
    return ret;
}
static void clear(gpointer key, gpointer value, gpointer user_data)
{
    hash_value *val = (hash_value *)value;
    gint now = *(gint *)user_data;

    if (now - val->time >= val->timeout)
        val->callback(val->obj, NULL, TRUE);
}
static gboolean remove(gpointer key, gpointer value, gpointer user_data)
{
    gboolean ret = FALSE;
    hash_value *val = (hash_value *)value;
    gint now = *(gint *)user_data;

    if (now - val->time >= val->timeout)
        ret = TRUE;
    
    return ret;
}
gboolean ipcam_message_manager_handle(IpcamMessageManager *message_manager, IpcamMessage *message)
{
    g_return_val_if_fail(!ipcam_message_is_response(message), FALSE);

    gboolean ret = FALSE;
    IpcamMessageManagerPrivate *priv = ipcam_message_manager_get_instance_private(message_manager);
    gchar *msg_id;
    g_object_get(G_OBJECT(message), "id", &msg_id, NULL);
    if (g_hash_table_contains(priv->msg_hash, msg_id))
    {
        hash_value *value = (hash_value *)g_hash_table_lookup(priv->msg_hash, (gconstpointer)msg_id);
        assert(value);
        value->callback(value->obj, message, FALSE);
    
        g_hash_table_remove(priv->msg_hash, (gpointer)msg_id);
    }

    g_free(msg_id);
    return ret;
}
void ipcam_message_manager_clear(IpcamMessageManager *message_manager)
{
    gint now = time((time_t *)NULL);
    IpcamMessageManagerPrivate *priv = ipcam_message_manager_get_instance_private(message_manager);

    g_hash_table_foreach(priv->msg_hash, (GHFunc)clear, (gpointer)&now);
    g_hash_table_foreach_remove(priv->msg_hash, (GHRFunc)remove, (gpointer)&now);
}
