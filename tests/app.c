#include "app.h"
#include "messages.h"
#include <json-glib/json-glib.h>

G_DEFINE_TYPE(IpcamApp, ipcam_app, IPCAM_BASE_APP_TYPE);

static void ipcam_app_before_impl(IpcamApp *app);
static void ipcam_app_in_loop_impl(IpcamApp *app);
static void ipcam_app_send_message_test(GObject *app);
static void message_handler(GObject *obj, IpcamMessage* msg, gboolean timeout);

static void ipcam_app_init(IpcamApp *self)
{
}
static void ipcam_app_class_init(IpcamAppClass *klass)
{
    IpcamBaseServiceClass *base_service_class = IPCAM_BASE_SERVICE_CLASS(klass);
    base_service_class->before = ipcam_app_before_impl;
    base_service_class->in_loop = ipcam_app_in_loop_impl;
}
static void ipcam_app_before_impl(IpcamApp *app)
{
    g_print("app before start\n");
    gchar *token = ipcam_base_app_get_config(IPCAM_BASE_APP(app), "token");
    ipcam_service_connect_by_name(IPCAM_SERVICE(app), "test_app", "tcp://127.0.0.1:3000", token);
    ipcam_base_app_add_timer(IPCAM_BASE_APP(app), "send_message_test", "5", ipcam_app_send_message_test);

    ipcam_app_send_message_test (app);
}
static void ipcam_app_in_loop_impl(IpcamApp *app)
{
}
static void ipcam_app_send_message_test(GObject *app)
{
    g_print("on_timer: send_message_test\n");
    g_return_if_fail(IPCAM_IS_APP(app));
    gchar *token = ipcam_base_app_get_config(IPCAM_BASE_APP(app), "token");
    IpcamRequestMessage *rq_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE, "action", "get_base_info", NULL);
    JsonBuilder *builder = json_builder_new();
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "items");
    json_builder_begin_array(builder);
    json_builder_add_string_value(builder, "device_name");
    json_builder_add_string_value(builder, "comment");
    json_builder_end_array(builder);
    json_builder_end_object(builder);
    JsonNode *body = json_builder_get_root(builder);
    g_object_set(G_OBJECT(rq_msg), "body", body, NULL);
    ipcam_base_app_send_message(IPCAM_BASE_APP(app), IPCAM_MESSAGE(rq_msg), "test_app", token,
                                message_handler, 5);
    g_object_unref(rq_msg);
    g_object_unref(builder);
}
static void message_handler(GObject *obj, IpcamMessage* msg, gboolean timeout)
{
    g_return_if_fail(IPCAM_IS_APP(obj));

    if (!timeout && msg)
    {
        gchar *str = ipcam_message_to_string(msg);
        g_printf("result=\n%s\n", str);
        g_free(str);
    }
}
