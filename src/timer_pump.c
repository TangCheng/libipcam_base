#include <czmq.h>
#include "timer_pump.h"

typedef struct _IpcamTimerPumpHashValue
{
    gchar *client_id;
    gchar *timer_id;
    glong interval;
    guint time_begin;
    guint count;
} IpcamTimerPumpHashValue;

typedef IpcamTimerPumpHashValue hash_value;

typedef struct _IpcamTimerPumpPrivate
{
    GHashTable *timers_hash;
    void *server_socket;
} IpcamTimerPumpPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamTimerPump, ipcam_timer_pump, IPCAM_BASE_SERVICE_TYPE);

static void ipcam_timer_pump_on_read_impl(IpcamTimerPump *timer_pump, void *mq_socket);
static void ipcam_timer_pump_in_loop_impl(IpcamTimerPump *timer_pump);
static void ipcam_timer_pump_register(IpcamTimerPump *timer_pump,
                                      const gchar *client_id,
                                      const gchar *timer_id,
                                      glong interval);
static void ipcam_timer_pump_unregister(IpcamTimerPump *timer_pump,
                                        const gchar *client_id,
                                        const gchar *timer_id);

static void ipcam_timer_pump_dispose(GObject *self)
{
    static gboolean first_run = TRUE;

    if (first_run)
    {
        first_run = FALSE;
        G_OBJECT_CLASS(ipcam_timer_pump_parent_class)->dispose(self);
    }
}
static void ipcam_timer_pump_finalize(GObject *self)
{
    IpcamTimerPumpPrivate *priv = ipcam_timer_pump_get_instance_private(IPCAM_TIMER_PUMP(self));
    g_hash_table_remove_all(priv->timers_hash);
    g_hash_table_destroy(priv->timers_hash);
    G_OBJECT_CLASS(ipcam_timer_pump_parent_class)->finalize(self);
}
static void destroy_value(gpointer data)
{
    hash_value *value = (hash_value*)data;
    g_free(value->client_id);
    g_free(value->timer_id);
    g_free(value);
}
static void ipcam_timer_pump_init(IpcamTimerPump *self)
{
    IpcamTimerPumpPrivate *priv = ipcam_timer_pump_get_instance_private(self);
    priv->server_socket = ipcam_base_service_bind(IPCAM_BASE_SERVICE(self), IPCAM_TIMER_PUMP_ADDRESS);
    assert(priv->server_socket);
    priv->timers_hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, 
                                              (GDestroyNotify)destroy_value);
}
static void ipcam_timer_pump_class_init(IpcamTimerPumpClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    this_class->dispose = &ipcam_timer_pump_dispose;
    this_class->finalize = &ipcam_timer_pump_finalize;
    
    IpcamBaseServiceClass *base_service_class = IPCAM_BASE_SERVICE_CLASS(klass);
    base_service_class->on_read = &ipcam_timer_pump_on_read_impl;
    base_service_class->in_loop = &ipcam_timer_pump_in_loop_impl;
}
static void ipcam_timer_pump_on_read_impl(IpcamTimerPump *timer_pump, void *mq_socket)
{
    gchar *timer_id = NULL;
    gchar *interval = NULL;
    gchar *client_id = NULL;
    glong n_interval = 0;
    IpcamTimerPumpPrivate *priv = ipcam_timer_pump_get_instance_private(timer_pump);
    g_return_if_fail(mq_socket == priv->server_socket);

    client_id = zstr_recv(mq_socket);
    timer_id = zstr_recv(mq_socket);
    interval = zstr_recv(mq_socket);

    g_print("client_id = %s, timer_id = %s, interval = %s\n", client_id, timer_id, interval);
    
    if (interval && client_id && timer_id)
    {
        n_interval = strtol(interval, NULL, 10);
        g_print("%ld\n", n_interval);
        if (n_interval < 1)
        {
            ipcam_timer_pump_unregister(timer_pump, client_id, timer_id);
        }
        else
        {
            ipcam_timer_pump_register(timer_pump, client_id, timer_id, n_interval);
        }
    }

    zstr_free(&interval);
    zstr_free(&timer_id);
    zstr_free(&client_id);
}
static void ipcam_timer_pump_check_timer(gpointer key, gpointer value, gpointer user_data)
{
    gint now = time((time_t *)NULL);
    IpcamTimerPumpPrivate *priv = (IpcamTimerPumpPrivate *)user_data;
    hash_value *val = (hash_value *)value;
    if (val->time_begin + val->interval <= now)
    {
        val->time_begin = now;
        val->count++;
        zstr_sendm(priv->server_socket, val->client_id);
        zstr_send(priv->server_socket, val->timer_id);
    }
}
static void ipcam_timer_pump_in_loop_impl(IpcamTimerPump *timer_pump)
{
    IpcamTimerPumpPrivate *priv = ipcam_timer_pump_get_instance_private(timer_pump);
    g_hash_table_foreach(priv->timers_hash, (GHFunc)ipcam_timer_pump_check_timer, (gpointer)priv);
}
static void ipcam_timer_pump_register(IpcamTimerPump *timer_pump,
                                      const gchar *client_id,
                                      const gchar *timer_id,
                                      glong interval)
{
    IpcamTimerPumpPrivate *priv = ipcam_timer_pump_get_instance_private(timer_pump);
    gchar *key = g_new(gchar, strlen(client_id) + strlen(timer_id) + 1);
    strcpy(key, client_id);
    strcat(key, timer_id);
    if (g_hash_table_contains(priv->timers_hash, key))
    {
        g_free(key);
        return;
    }
    
    hash_value *value = g_new(hash_value, 1);
    value->client_id = g_strdup(client_id);
    value->timer_id = g_strdup(timer_id);
    value->interval = interval;
    value->time_begin = time((time_t *)NULL);
    value->count = 0;
    
    g_hash_table_insert(priv->timers_hash, key, value);
}
static void ipcam_timer_pump_unregister(IpcamTimerPump *timer_pump,
                                        const gchar *client_id,
                                        const gchar *timer_id)
{
    IpcamTimerPumpPrivate *priv = ipcam_timer_pump_get_instance_private(timer_pump);
    gchar *key = g_new(gchar, strlen(client_id) + strlen(timer_id) + 1);
    strcpy(key, client_id);
    strcat(key, timer_id);

    g_hash_table_remove(priv->timers_hash, key);
    
    g_free(key);
}
