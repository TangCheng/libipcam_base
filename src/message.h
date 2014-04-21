#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <glib.h>
#include <glib-object.h>

#define IPCAM_MESSAGE_TYPE (ipcam_message_get_type())
#define IPCAM_MESSAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_MESSAGE_TYPE, IpcamMessage))
#define IPCAM_MESSAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_MESSAGE_TYPE, IpcamMessageClass))
#define IPCAM_IS_MESSAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_MESSAGE_TYPE))
#define IPCAM_IS_MESSAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_MESSAGE_TYPE))
#define IPCAM_MESSAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_MESSAGE_TYPE, IpcamMessageClass))

typedef struct _IpcamMessage IpcamMessage;
typedef struct _IpcamMessageClass IpcamMessageClass;

struct _IpcamMessage
{
    GObject parent;
};

struct _IpcamMessageClass
{
    GObjectClass parent_class;
};

GType ipcam_message_get_type(void);

#endif /* __MESSAGE_H__ */
