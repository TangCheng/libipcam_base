#include "app1.h"
#include "test_action_handler.h"

G_DEFINE_TYPE(IpcamApp1, ipcam_app1, IPCAM_BASE_APP_TYPE);

static void ipcam_app1_before_impl(IpcamApp1 *app1);
static void ipcam_app1_in_loop_impl(IpcamApp1 *app1);

static void ipcam_app1_init(IpcamApp1 *self)
{
}
static void ipcam_app1_class_init(IpcamApp1Class *klass)
{
    IpcamBaseServiceClass *base_service_class = IPCAM_BASE_SERVICE_CLASS(klass);
    base_service_class->before = ipcam_app1_before_impl;
    base_service_class->in_loop = ipcam_app1_in_loop_impl;
}
static void ipcam_app1_before_impl(IpcamApp1 *app1)
{
    g_print("app1 before start\n");
    ipcam_service_bind_by_name(IPCAM_SERVICE(app1), "test_app", "tcp://*:3001");
    ipcam_base_app_register_handler(IPCAM_BASE_APP(app1), "test", IPCAM_TEST_ACTION_HANDLER_TYPE);
}
static void ipcam_app1_in_loop_impl(IpcamApp1 *app)
{
}
