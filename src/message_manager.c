#include "message_manager.h"
#include <assert.h>

typedef struct _IpcamMessageWaiterHashValue
{
	GMutex mutex;
	GCond condition;
	IpcamMessage *message;
} IpcamMessageWaiterHashValue;

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
	GHashTable *waiter_hash;
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

    g_hash_table_unref(priv->msg_hash);
	g_hash_table_unref(priv->waiter_hash);

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
    g_assert(priv->msg_hash);
	priv->waiter_hash = g_hash_table_new_full(g_str_hash, g_str_equal, destroy_key, NULL);
	g_assert(priv->waiter_hash);
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

    if (!g_hash_table_contains(priv->msg_hash, msg_id))
    {
        hash_value *value = g_new(hash_value, 1);
        value->time = time((time_t *)NULL);
        value->timeout = timeout;
        value->obj = obj;
        value->callback = handler;

        ret = g_hash_table_insert(priv->msg_hash, (gpointer)g_strdup(msg_id), (gpointer)value);
    }

    g_free(msg_id);
    return ret;
}

gboolean ipcam_message_manager_wait_for(IpcamMessageManager *message_manager,
                                        const char *message_id,
                                        gint64 timeout_ms,
                                        IpcamMessage **response)
{
	IpcamMessageManagerPrivate *priv = ipcam_message_manager_get_instance_private(message_manager);
	IpcamMessageWaiterHashValue *waiter = NULL;
	GHashTable *waiter_hash;
	gboolean ret = FALSE;

	g_assert(IPCAM_IS_MESSAGE_MANAGER(message_manager) && message_id && response);

	waiter_hash = g_hash_table_ref(priv->waiter_hash);

	if (g_hash_table_lookup(waiter_hash, (gconstpointer)message_id)) {
		g_hash_table_unref(waiter_hash);
		g_warning("There is already a thread waiting for message '%s'.\n", message_id);
		return FALSE;
	}

	waiter = g_new0(IpcamMessageWaiterHashValue, 1);
	if (waiter == NULL) {
		g_hash_table_unref(waiter_hash);
		g_error("Out of memory\n");
		return FALSE;
	}

	waiter->message = NULL;
	g_cond_init(&waiter->condition);
	g_mutex_init(&waiter->mutex);

	g_mutex_lock(&waiter->mutex);

	g_hash_table_insert(waiter_hash, (gpointer)g_strdup(message_id), waiter);

	if (timeout_ms > 0) {
		gint64 endtime = g_get_monotonic_time () + timeout_ms * G_TIME_SPAN_MILLISECOND;
		ret = g_cond_wait_until (&waiter->condition, &waiter->mutex, endtime);
	}
	else {
		g_cond_wait (&waiter->condition, &waiter->mutex);
	}

	g_hash_table_remove(priv->waiter_hash, message_id);

	g_mutex_unlock(&waiter->mutex);

	g_hash_table_unref(waiter_hash);

	*response = waiter->message;

	g_cond_clear(&waiter->condition);
	g_mutex_clear(&waiter->mutex);
	g_free(waiter);

	return (ret && *response);
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

	if (g_hash_table_contains(priv->waiter_hash, msg_id))
	{
		IpcamMessageWaiterHashValue *waiter = g_hash_table_lookup(priv->waiter_hash, msg_id);
		if (waiter) {
			g_mutex_lock(&waiter->mutex);
			waiter->message = g_object_ref(message);
			g_cond_broadcast(&waiter->condition);
			g_mutex_unlock(&waiter->mutex);
		}
	}

    if (g_hash_table_contains(priv->msg_hash, msg_id))
    {
        hash_value *value = (hash_value *)g_hash_table_lookup(priv->msg_hash, (gconstpointer)msg_id);
        assert(value);
		if (value->callback)
			value->callback(value->obj, message, FALSE);

		ret = g_hash_table_remove(priv->msg_hash, (gpointer)msg_id);
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
