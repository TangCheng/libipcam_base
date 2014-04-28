#ifndef __ACTION_HANDLER_H__
#define __ACRION_HANDLER_H__

#include <glib.h>
#include <glib-object.h>
#include "service.h"
#include "message.h"

#define IPCAM_ACTION_HANDLER_TYPE (ipcam_action_handler_get_type())
#define IPCAM_ACTION_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_ACTION_HANDLER_TYPE, IpcamActionHandler))
#define IPCAM_ACTION_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_ACTION_HANDLER_TYPE, IpcamActionHandlerClass))
#define IPCAM_IS_ACTION_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_ACTION_HANDLER_TYPE))
#define IPCAM_IS_ACTION_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_ACTION_HANDLER_TYPE))
#define IPCAM_ACTION_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_ACTION_HANDLER_TYPE, IpcamActionHandlerClass))

typedef struct _IpcamActionHandler IpcamActionHandler;
typedef struct _IpcamActionHandlerClass IpcamActionHandlerClass;

struct _IpcamActionHandler
{
    GObject parent;
};

struct _IpcamActionHandlerClass
{
    GObjectClass parent_class;
};

GType ipcam_action_handler_get_type(void);
void ipcam_action_handler_run(IpcamActionHandler *action_handler,
                              IpcamService *service,
                              IpcamMessage *message);

#endif /* __ACTION_HANDLER_H__ */
