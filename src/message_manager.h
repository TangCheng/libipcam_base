#ifndef __MESSAGE_MANAGER_H__
#define __MESSAGE_MANAGER_H__

#include <glib.h>
#include <glib-object.h>
#include "message.h"

#define IPCAM_MESSAGE_MANAGER_TYPE (ipcam_message_manager_get_type())
#define IPCAM_MESSAGE_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_MESSAGE_MANAGER_TYPE, IpcamMessageManager))
#define IPCAM_MESSAGE_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_MESSAGE_MANAGER_TYPE, IpcamMessageManagerClass))
#define IPCAM_IS_MESSAGE_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_MESSAGE_MANAGER_TYPE))
#define IPCAM_IS_MESSAGE_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_MESSAGE_MANAGER_TYPE))
#define IPCAM_MESSAGE_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_MESSAGE_MANAGER_TYPE, IpcamMessageManagerClass))

typedef struct _IpcamMessageManager IpcamMessageManager;
typedef struct _IpcamMessageManagerClass IpcamMessageManagerClass;

typedef void (*MsgHandler)(IpcamMessage* msg, gboolean timeout);

struct _IpcamMessageManager
{
    GObject parent;
};

struct _IpcamMessageManagerClass
{
    GObjectClass parent_class;
};

GType ipcam_message_manager_get_type(void);
gboolean ipcam_message_manager_register(IpcamMessageManager *message_manager,
                                        IpcamMessage *message,
                                        MsgHandler handler,
                                        guint timeout);
gboolean ipcam_message_manager_handle(IpcamMessageManager *message_manager, IpcamMessage *message);
void ipcam_message_manager_clear(IpcamMessageManager *message_manager);

#endif /* __MESSAGE_MANAGER_H__ */
