#include "app.h"
#include "messages.h"

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
    ipcam_service_connect_by_name(IPCAM_SERVICE(app), "test_app", "tcp://127.0.0.1:3001", "dispatch_token_123456");
    /*
    IpcamRequestMessage *rq_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE, "action", "test", NULL);
    ipcam_base_app_send_message(IPCAM_BASE_APP(app), IPCAM_MESSAGE(rq_msg), "test_app", "dispatch_token_123456",
                                message_handler, 5);
    g_object_unref(rq_msg);
    */
    ipcam_base_app_add_timer(IPCAM_BASE_APP(app), "send_message_test", "1", ipcam_app_send_message_test);
}
static void ipcam_app_in_loop_impl(IpcamApp *app)
{
}
static void ipcam_app_send_message_test(GObject *app)
{
    g_print("on_timer: send_message_test\n");
    g_return_if_fail(IPCAM_IS_APP(app));
    IpcamRequestMessage *rq_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE, "action", "test", NULL);
    ipcam_base_app_send_message(IPCAM_BASE_APP(app), IPCAM_MESSAGE(rq_msg), "test_app", "dispatch_token_123456",
                                message_handler, 5);
    g_object_unref(rq_msg);
}
static void message_handler(GObject *obj, IpcamMessage* msg, gboolean timeout)
{
    g_return_if_fail(IPCAM_IS_APP(obj));
}
