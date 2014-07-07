#include "message_manager.h"
#include <assert.h>

typedef struct _IpcamMessageWaiter
{
	GCond condition;
	IpcamMessage *message;
} IpcamMessageWaiter;

typedef struct _IpcamMessageManagerHashValue
{
    gint time;
    guint timeout;
    GObject *obj;
    MsgHandler callback;
	IpcamMessageWaiter *waiter;
} IpcamMessageManagerHashValue;

typedef IpcamMessageManagerHashValue hash_value;

typedef struct _IpcamMessageManagerPrivate
{
    GHashTable *msg_hash;
	GMutex msg_hash_mutex;
} IpcamMessageManagerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamMessageManager, ipcam_message_manager, G_TYPE_OBJECT);

static void ipcam_message_manager_dispose(GObject *self)
{
    static gboolean first_run = TRUE;
    if (first_run)
    {
        first_run = FALSE;
        G_OBJECT_CLASS(ipcam_message_manager_parent_class)->dispose(self);
    }
}

static void ipcam_message_manager_finalize(GObject *self)
{
    IpcamMessageManagerPrivate *priv =
            ipcam_message_manager_get_instance_private(IPCAM_MESSAGE_MANAGER(self));

	g_mutex_lock(&priv->msg_hash_mutex);
    g_hash_table_remove_all(priv->msg_hash);
    g_hash_table_destroy(priv->msg_hash);
	g_mutex_unlock(&priv->msg_hash_mutex);
	g_mutex_clear(&priv->msg_hash_mutex);

	G_OBJECT_CLASS(ipcam_message_manager_parent_class)->finalize(self);
}

static void destroy_notify(gpointer data)
{
    hash_value *value = (hash_value *)data;
    g_free(value);
}

static void destroy_key(gpointer data)
{
    g_free(data);
}

static void ipcam_message_manager_init(IpcamMessageManager *self)
{
    IpcamMessageManagerPrivate *priv = ipcam_message_manager_get_instance_private(self);
    priv->msg_hash = g_hash_table_new_full(g_str_hash, g_str_equal, destroy_key, (GDestroyNotify)destroy_notify);
    assert(priv->msg_hash);
	g_mutex_init(&priv->msg_hash_mutex);
}

static void ipcam_message_manager_class_init(IpcamMessageManagerClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    this_class->dispose = &ipcam_message_manager_dispose;
    this_class->finalize = &ipcam_message_manager_finalize;
}

gboolean ipcam_message_manager_register(IpcamMessageManager *message_manager,
                                        IpcamMessage *message,
                                        GObject *obj,
                                        MsgHandler handler,
                                        guint timeout)
{
    g_return_val_if_fail(ipcam_message_is_request(message), FALSE);

    gboolean ret = FALSE;
    IpcamMessageManagerPrivate *priv = ipcam_message_manager_get_instance_private(message_manager);
    gchar *msg_id;
    g_object_get(G_OBJECT(message), "id", &msg_id, NULL);

	g_mutex_lock(&priv->msg_hash_mutex);
    if (!g_hash_table_contains(priv->msg_hash, msg_id))
    {
        hash_value *value = g_new(hash_value, 1);
        value->time = time((time_t *)NULL);
        value->timeout = timeout;
        value->obj = obj;
        value->callback = handler;
		value->waiter = NULL;

        ret = g_hash_table_insert(priv->msg_hash, (gpointer)g_strdup(msg_id), (gpointer)value);
    }
	g_mutex_unlock(&priv->msg_hash_mutex);

    g_free(msg_id);
    return ret;
}

gboolean ipcam_message_manager_wait_for(IpcamMessageManager *message_manager,
                                        const char *message_id,
                                        gint64 timeout_ms,
                                        IpcamMessage **ret_msg)
{
	IpcamMessageManagerPrivate *priv = ipcam_message_manager_get_instance_private(message_manager);
	hash_value *value;
	IpcamMessageWaiter *waiter = NULL;
	gboolean ret = FALSE;

	g_mutex_lock(&priv->msg_hash_mutex);
	value = (hash_value *)g_hash_table_lookup(priv->msg_hash, (gconstpointer)message_id);
	if (value) {
		if (value->waiter) {
			g_mutex_unlock(&priv->msg_hash_mutex);
			g_warn_if_reached();
			return FALSE;
		}

		waiter = g_new0(IpcamMessageWaiter, 1);
		if (waiter == NULL) {
			g_mutex_unlock(&priv->msg_hash_mutex);
			g_warn_if_reached();
			return FALSE;
		}

		waiter->message = NULL;
		g_cond_init(&waiter->condition);

		value->waiter = waiter;

		if (timeout_ms > 0) {
			gint64 endtime = g_get_monotonic_time () + timeout_ms * G_TIME_SPAN_MILLISECOND;
			ret = g_cond_wait_until (&waiter->condition, &priv->msg_hash_mutex, endtime);
		}
		else {
			g_cond_wait (&waiter->condition, &priv->msg_hash_mutex);
		}

		*ret_msg = waiter->message;

		g_cond_clear(&waiter->condition);
		g_free(waiter);
	}
	else {
		g_mutex_unlock(&priv->msg_hash_mutex);
		g_warn_if_reached();
		return FALSE;
	}
	g_mutex_unlock(&priv->msg_hash_mutex);

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
    g_return_val_if_fail(ipcam_message_is_response(message), FALSE);

    gboolean ret = FALSE;
    IpcamMessageManagerPrivate *priv = ipcam_message_manager_get_instance_private(message_manager);
    gchar *msg_id;
    g_object_get(G_OBJECT(message), "id", &msg_id, NULL);

	g_mutex_lock(&priv->msg_hash_mutex);
    if (g_hash_table_contains(priv->msg_hash, msg_id))
    {
        hash_value *value = (hash_value *)g_hash_table_lookup(priv->msg_hash, (gconstpointer)msg_id);
        assert(value);
		if (value->waiter) {
			value->waiter->message = g_object_ref(message);
			g_cond_signal (&value->waiter->condition);
		}
		else {
			if (value->callback)
				value->callback(value->obj, message, FALSE);
		}
    
        ret = g_hash_table_remove(priv->msg_hash, (gpointer)msg_id);
    }
	g_mutex_unlock(&priv->msg_hash_mutex);

    g_free(msg_id);
    return ret;
}

void ipcam_message_manager_clear(IpcamMessageManager *message_manager)
{
    gint now = time((time_t *)NULL);
    IpcamMessageManagerPrivate *priv = ipcam_message_manager_get_instance_private(message_manager);

	g_mutex_lock(&priv->msg_hash_mutex);
    g_hash_table_foreach(priv->msg_hash, (GHFunc)clear, (gpointer)&now);
    g_hash_table_foreach_remove(priv->msg_hash, (GHRFunc)remove, (gpointer)&now);
	g_mutex_unlock(&priv->msg_hash_mutex);
}
