#include <string.h>
#include "base_app.h"
#include "config_manager.h"
#include "timer_manager.h"
#include "timer_pump.h"
#include "socket_manager.h"
#include "action_handler.h"
#include "event_handler.h"

#define IPCAM_TIMER_CLIENT_NAME "_timer_client"

typedef struct _IpcamBaseAppPrivate
{
    IpcamConfigManager *config_manager;
    IpcamTimerManager *timer_manager;
    IpcamMessageManager *msg_manager;
    GHashTable *handler_hash;
} IpcamBaseAppPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamBaseApp, ipcam_base_app, IPCAM_SERVICE_TYPE);

static void ipcam_base_app_server_receive_string_impl(IpcamBaseApp *self,
                                                      const gchar *name,
                                                      const gchar *client_id,
                                                      const gchar *string);
static void ipcam_base_app_client_receive_string_impl(IpcamBaseApp *self,
                                                      const gchar *name,
                                                      const gchar *string);
static void ipcam_base_app_connect_to_timer(IpcamBaseApp *base_app);
static void ipcam_base_app_load_config(IpcamBaseApp *base_app);
static void ipcam_base_app_add_timer(IpcamBaseApp *base_app,
                                     const gchar *timer_id,
                                     const gchar *interval,
                                     TCFunc callback);
static void ipcam_base_app_message_manager_clear(GObject *base_app);
static void ipcam_base_app_on_timer(IpcamBaseApp *base_app, const gchar *timer_id);
static void ipcam_base_app_receive_string(IpcamBaseApp *base_app,
                                          const gchar *string,
                                          const gchar *name,
                                          const gint type,
                                          const gchar *client_id);
static void ipcam_base_app_action_handler(IpcamBaseApp *base_app, IpcamMessage *msg);
static void ipcam_base_app_notice_handler(IpcamBaseApp *base_app, IpcamMessage *msg);


static GObject *ipcam_base_app_constructor(GType self_type,
                                           guint n_properties,
                                           GObjectConstructParam *properties)
{
    GObject *obj;
    
    obj = G_OBJECT_CLASS(ipcam_base_app_parent_class)->constructor(self_type, n_properties, properties);

    return obj;
}
static void ipcam_base_app_dispose(GObject *self)
{
    static gboolean first_run = TRUE;

    if (first_run)
    {
        first_run = FALSE;
        IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(IPCAM_BASE_APP(self));
        g_object_unref(priv->config_manager);
        g_object_unref(priv->timer_manager);
        g_object_unref(priv->msg_manager);
        G_OBJECT_CLASS(ipcam_base_app_parent_class)->dispose(self);
    }
}
static void ipcam_base_app_finalize(GObject *self)
{
    IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(IPCAM_BASE_APP(self));
    g_hash_table_destroy(priv->handler_hash);
    G_OBJECT_CLASS(ipcam_base_app_parent_class)->finalize(self);
}
static void ipcam_base_app_init(IpcamBaseApp *self)
{
    IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(self);
    priv->config_manager = g_object_new(IPCAM_CONFIG_MANAGER_TYPE, NULL);
    priv->timer_manager = g_object_new(IPCAM_TIMER_MANAGER_TYPE, NULL);
    priv->msg_manager = g_object_new(IPCAM_MESSAGE_MANAGER_TYPE, NULL);
    priv->handler_hash = g_hash_table_new(g_str_hash, g_str_equal);

    ipcam_base_app_load_config(self);
    ipcam_base_app_connect_to_timer(self);
    ipcam_base_app_add_timer(self, "clear_message_manager", "10", ipcam_base_app_message_manager_clear);
}
static void ipcam_base_app_class_init(IpcamBaseAppClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    this_class->constructor = &ipcam_base_app_constructor;
    this_class->dispose = &ipcam_base_app_dispose;
    this_class->finalize = &ipcam_base_app_finalize;

    IpcamServiceClass *service_class = IPCAM_SERVICE_CLASS(klass);
    service_class->server_receive_string = &ipcam_base_app_server_receive_string_impl;
    service_class->client_receive_string = &ipcam_base_app_client_receive_string_impl;
}
static void ipcam_base_app_server_receive_string_impl(IpcamBaseApp *self,
                                                      const gchar *name,
                                                      const gchar *client_id,
                                                      const gchar *string)
{
    ipcam_base_app_receive_string(self, string, name, IPCAM_SOCKET_TYPE_SERVER, client_id);
}
static void ipcam_base_app_client_receive_string_impl(IpcamBaseApp *self,
                                                      const gchar *name,
                                                      const gchar *string)
{
    if (0 == strcmp(name, IPCAM_TIMER_CLIENT_NAME))
    {
        ipcam_base_app_on_timer(self, string);
    }
    else
    {
        ipcam_base_app_receive_string(self, string, name, IPCAM_SOCKET_TYPE_CLIENT, NULL);
    }
}
static void ipcam_base_app_load_config(IpcamBaseApp *base_app)
{
    IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(base_app);
    ipcam_config_manager_load_config(priv->config_manager, "config/app.yml");
    ipcam_config_manager_merge(priv->config_manager, "token",
                               G_OBJECT_CLASS_NAME(IPCAM_BASE_APP_GET_CLASS(base_app)));
}
static void ipcam_base_app_connect_to_timer(IpcamBaseApp *base_app)
{
    IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(base_app);
    gchar *token = ipcam_config_manager_get(priv->config_manager, "token");
    ipcam_service_connect_by_name(IPCAM_SERVICE(base_app),
                                  IPCAM_TIMER_CLIENT_NAME,
                                  IPCAM_TIMER_PUMP_ADDRESS,
                                  token);
}
static void ipcam_base_app_add_timer(IpcamBaseApp *base_app,
                                     const gchar *timer_id,
                                     const gchar *interval,
                                     TCFunc callback)
{
    IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(base_app);
    ipcam_timer_manager_add_timer(priv->timer_manager, timer_id, G_OBJECT(base_app), callback);
    const gchar **strings = (const gchar **)g_new(gpointer, 2);
    strings[0] = timer_id;
    strings[1] = interval;
    ipcam_service_send_strings(IPCAM_SERVICE(base_app), IPCAM_TIMER_CLIENT_NAME, strings, NULL);
    g_free(strings);
}
static void ipcam_base_app_message_manager_clear(GObject *base_app)
{
    g_return_if_fail(IPCAM_IS_BASE_APP(base_app));
    IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(IPCAM_BASE_APP(base_app));
    ipcam_message_manager_clear(priv->msg_manager);
}
static void ipcam_base_app_on_timer(IpcamBaseApp *base_app, const gchar *timer_id)
{
    IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(base_app);
    ipcam_timer_manager_trig_timer(priv->timer_manager, timer_id);
}
static void ipcam_base_app_receive_string(IpcamBaseApp *base_app,
                                          const gchar *string,
                                          const gchar *name,
                                          const gint type,
                                          const gchar *client_id)
{
    IpcamMessage *msg = ipcam_message_parse_from_string(string);
    if (msg)
    {
        if (type == IPCAM_SOCKET_TYPE_SERVER)
        {
            gchar *strval;
            g_object_get(G_OBJECT(msg), "token", &strval, NULL);
            if (0 != strcmp(client_id, strval))
            {
                g_object_unref(msg);
                g_free(strval);
                return;
            }
            g_free(strval);
        }
        
        if (ipcam_message_is_request(msg))
        {
            ipcam_base_app_action_handler(base_app, msg);
        }
        else if (ipcam_message_is_notice(msg))
        {
            ipcam_base_app_notice_handler(base_app, msg);
        }
        else if (ipcam_message_is_response(msg))
        {
            IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(base_app);
            ipcam_message_manager_handle(priv->msg_manager, msg);
        }
        else
        {
            g_object_unref(msg);
        }
    }
}
static void ipcam_base_app_action_handler(IpcamBaseApp *base_app, IpcamMessage *msg)
{
    GType action_handler_class_type = G_TYPE_INVALID;
    gchar *strval;
    g_object_get(G_OBJECT(msg), "action", &strval, NULL);
    IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(base_app);
    action_handler_class_type = (GType)g_hash_table_lookup(priv->handler_hash, (gpointer)strval);
    g_free(strval);
    if (G_TYPE_INVALID != action_handler_class_type)
    {
        IpcamActionHandler *handler = g_object_new(action_handler_class_type, "service", base_app, NULL);
        if (IPCAM_IS_ACTION_HANDLER(handler))
        {
            ipcam_action_handler_run(handler, msg);
        }
        g_object_unref(handler);
    }
}
static void ipcam_base_app_notice_handler(IpcamBaseApp *base_app, IpcamMessage *msg)
{
    GType event_handler_class_type = G_TYPE_INVALID;
    gchar *strval;
    g_object_get(G_OBJECT(msg), "event", &strval, NULL);
    IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(base_app);
    event_handler_class_type = (GType)g_hash_table_lookup(priv->handler_hash, (gpointer)strval);
    g_free(strval);
    if (G_TYPE_INVALID != event_handler_class_type)
    {
        IpcamEventHandler *handler = g_object_new(event_handler_class_type, "service", base_app, NULL);
        if (IPCAM_IS_EVENT_HANDLER(handler))
        {
            ipcam_event_handler_run(handler, msg);
        }
        g_object_unref(handler);
    }
}
void ipcam_base_app_register_handler(IpcamBaseApp *base_app,
                                    const gchar *handler_name,
                                    GType handler_class_type)
{
    g_return_if_fail(IPCAM_IS_BASE_APP(base_app));
    IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(base_app);
    g_return_if_fail(!g_hash_table_contains(priv->handler_hash, (gpointer)handler_name));
    g_hash_table_insert(priv->handler_hash, (gpointer)handler_name, (gpointer)handler_class_type);
}
void ipcam_base_app_send_message(IpcamBaseApp *base_app,
                                 IpcamMessage *msg,
                                 const gchar *name,
                                 const gchar *client_id,
                                 MsgHandler callback,
                                 guint timeout)
{
    g_return_if_fail(IPCAM_IS_BASE_APP(base_app));
    g_return_if_fail(IPCAM_IS_MESSAGE(msg));
    IpcamBaseAppPrivate *priv = ipcam_base_app_get_instance_private(base_app);
    gboolean is_server = ipcam_service_is_server(IPCAM_SERVICE(base_app), name);
    gchar *token = "";
    if (!is_server)
    {
        token = ipcam_config_manager_get(priv->config_manager, "token");
    }
    g_object_set(G_OBJECT(msg), "token", token, NULL);
    ipcam_message_manager_register(priv->msg_manager, msg, G_OBJECT(base_app), callback, timeout);
    const gchar **strings = (const gchar **)g_new(gpointer, 2);
    strings[0] = ipcam_message_to_string(msg);
    strings[1] = NULL;
    ipcam_service_send_strings(IPCAM_SERVICE(base_app), name, strings, client_id);
    g_free(strings);
}
