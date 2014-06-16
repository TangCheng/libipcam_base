#include <json-glib/json-glib.h>
#include "test_action_handler.h"
#include "messages.h"
#include "app1.h"

G_DEFINE_TYPE(IpcamTestActionHandler, ipcam_test_action_handler, IPCAM_ACTION_HANDLER_TYPE);

static void ipcam_test_action_handler_run_impl(IpcamTestActionHandler *test_action_handler,
                                               IpcamMessage *message);

static void ipcam_test_action_handler_init(IpcamTestActionHandler *self)
{
}
static void ipcam_test_action_handler_class_init(IpcamTestActionHandlerClass *klass)
{
    IpcamActionHandlerClass *action_handler_class = IPCAM_ACTION_HANDLER_CLASS(klass);
    action_handler_class->run = &ipcam_test_action_handler_run_impl;
}
static void ipcam_test_action_handler_run_impl(IpcamTestActionHandler *test_action_handler,
                                               IpcamMessage *message)
{
    gpointer service;
    g_object_get(G_OBJECT(test_action_handler), "service", &service, NULL);
    IpcamRequestMessage *rq_msg = IPCAM_REQUEST_MESSAGE(message);
    IpcamResponseMessage *rp_msg = ipcam_request_message_get_response_message(rq_msg, "1");

    JsonBuilder *builder = json_builder_new();
    json_builder_begin_object (builder);
    json_builder_set_member_name (builder, "items");
    json_builder_begin_object (builder);
    json_builder_set_member_name (builder, "device_name");
    json_builder_add_string_value (builder, "ipcamera");
    json_builder_set_member_name (builder, "comment");
    json_builder_add_string_value (builder, "comment");
    json_builder_end_object (builder);
    json_builder_end_object (builder);
    JsonNode *body = json_builder_get_root (builder);
    g_object_unref(builder);

    g_object_set(G_OBJECT(rp_msg), "body", body, NULL);

    gchar *token;
    g_object_get(G_OBJECT(rq_msg), "token", &token, NULL);
    ipcam_base_app_send_message(IPCAM_BASE_APP(service), rp_msg, "test_app", token, NULL, 0);
    g_object_unref(rp_msg);
    g_free(token);
}
