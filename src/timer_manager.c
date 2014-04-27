#include "timer_manager.h"

typedef struct _IpcamTimerManagerHashValue
{
    gchar *timer_id;
    GObject *object;
    TCFunc callback;
} IpcamTimerManagerHashValue;

typedef IpcamTimerManagerHashValue hash_value;

typedef struct _IpcamTimerManagerPrivate
{
    GHashTable *timers_hash;
} IpcamTimerManagerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamTimerManager, ipcam_timer_manager, G_TYPE_OBJECT);

static void ipcam_timer_manager_dispose(GObject *self)
{
    static gboolean first_run = TRUE;

    if (first_run)
    {
        first_run = FALSE;
        IpcamTimerManagerPrivate *priv = ipcam_timer_manager_get_instance_private(IPCAM_TIMER_MANAGER(self));
        g_hash_table_destroy(priv->timers_hash);
        G_OBJECT_CLASS(ipcam_timer_manager_parent_class)->dispose(self);
    }
}
static void destroy(gpointer data)
{
    hash_value *value = (hash_value *)data;
    g_free(value->timer_id);
    g_free(value);
}
static void ipcam_timer_manager_init(IpcamTimerManager *self)
{
    IpcamTimerManagerPrivate *priv = ipcam_timer_manager_get_instance_private(self);
    priv->timers_hash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (GDestroyNotify)destroy);
}
static void ipcam_timer_manager_class_init(IpcamTimerManagerClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    this_class->dispose = &ipcam_timer_manager_dispose;
}
gboolean ipcam_timer_manager_add_timer(IpcamTimerManager *timer_manager,
                                       const gchar *timer_id,
                                       GObject *object,
                                       TCFunc callback)
{
    g_return_val_if_fail(IPCAM_IS_TIMER_MANAGER(timer_manager), FALSE);
    IpcamTimerManagerPrivate *priv = ipcam_timer_manager_get_instance_private(timer_manager);
    g_return_val_if_fail(!g_hash_table_contains(priv->timers_hash, timer_id), FALSE);
    hash_value *value = g_new(hash_value, 1);
    value->timer_id = g_strdup(timer_id);
    value->object = object;
    value->callback = callback;
    return g_hash_table_insert(priv->timers_hash, (gpointer)timer_id, (gpointer)value);
}
void ipcam_timer_manager_del_timer(IpcamTimerManager *timer_manager, const gchar *timer_id)
{
    g_return_if_fail(IPCAM_IS_TIMER_MANAGER(timer_manager));
    IpcamTimerManagerPrivate *priv = ipcam_timer_manager_get_instance_private(timer_manager);
    g_hash_table_remove(priv->timers_hash, (gpointer)timer_id);
}
void ipcam_timer_manager_trig_timer(IpcamTimerManager *timer_manager, const gchar *timer_id)
{
    g_return_if_fail(IPCAM_IS_TIMER_MANAGER(timer_manager));
    IpcamTimerManagerPrivate *priv = ipcam_timer_manager_get_instance_private(timer_manager);
    hash_value *value = (hash_value *)g_hash_table_lookup(priv->timers_hash, (gpointer)timer_id);
    if (value)
    {
        value->callback(value->object);
    }
}
