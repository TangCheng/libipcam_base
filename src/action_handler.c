#include "action_handler.h"

G_DEFINE_TYPE(IpcamActionHandler, ipcam_action_handler, G_TYPE_OBJECT);

static void ipcam_action_handler_init(IpcamActionHandler *self)
{
}
static void ipcam_action_handler_class_init(IpcamActionHandlerClass *klass)
{
}

void ipcam_action_handler_run(IpcamActionHandler *action_handler,
                              IpcamService *service,
                              IpcamMessage *message)
{
    g_return_if_fail(IPCAM_IS_ACTION_HANDLER(action_handler));
}
