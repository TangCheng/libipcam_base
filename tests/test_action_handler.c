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
    //sleep(1);
    IpcamRequestMessage *rq_msg = IPCAM_REQUEST_MESSAGE(message);
    IpcamResponseMessage *rp_msg = ipcam_request_message_get_response_message(rq_msg, "1");
    gchar *token;
    g_object_get(G_OBJECT(rq_msg), "token", &token, NULL);
    ipcam_base_app_send_message(IPCAM_BASE_APP(service), rp_msg, "test_app", token, NULL, 0);
    g_object_unref(rp_msg);
    g_free(token);
}
