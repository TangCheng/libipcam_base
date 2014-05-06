#ifndef __TEST_ACTION_HANDLER_H__
#define __TEST_ACTION_HANDLER_H__

#include "action_handler.h"

#define IPCAM_TEST_ACTION_HANDLER_TYPE (ipcam_test_action_handler_get_type())
#define IPCAM_TEST_ACTION_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_TEST_ACTION_HANDLER_TYPE, IpcamTestActionHandler))
#define IPCAM_TEST_ACTION_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_TEST_ACTION_HANDLER_TYPE, IpcamTestActionHandlerClass))
#define IPCAM_IS_TEST_ACTION_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_TEST_ACTION_HANDLER_TYPE))
#define IPCAM_IS_TEST_ACTION_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_TEST_ACTION_HANDLER_TYPE))
#define IPCAM_TEST_ACTION_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_TEST_ACTION_HANDLER_TYPE, IpcamTestActionHandlerClass))

typedef struct _IpcamTestActionHandler IpcamTestActionHandler;
typedef struct _IpcamTestActionHandlerClass IpcamTestActionHandlerClass;

struct _IpcamTestActionHandler
{
    IpcamActionHandler parent;
};

struct _IpcamTestActionHandlerClass
{
    IpcamActionHandlerClass parent_class;
};

GType ipcam_test_action_handler_get_type(void);

#endif /* __TEST_ACTION_HANDLER_H__ */
