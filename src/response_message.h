#ifndef __RESPONSE_MESSAGE_H__
#define __RESPONSE_MESSAGE_H__

#include "message.h"

#define IPCAM_RESPONSE_MESSAGE_TYPE (ipcam_response_message_get_type())
#define IPCAM_RESPONSE_MESSAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_RESPONSE_MESSAGE_TYPE, IpcamResponseMessage))
#define IPCAM_RESPONSE_MESSAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_RESPONSE_MESSAGE_TYPE, IpcamResponseMessageClass))
#define IPCAM_IS_RESPONSE_MESSAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_RESPONSE_MESSAGE_TYPE))
#define IPCAM_IS_RESPONSE_MESSAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_RESPONSE_MESSAGE_TYPE))
#define IPCAM_RESPONSE_MESSAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_RESPONSE_MESSAGE_TYPE, IpcamResponseMessageClass))

typedef struct _IpcamResponseMessage IpcamResponseMessage;
typedef struct _IpcamResponseMessageClass IpcamResponseMessageClass;

struct _IpcamResponseMessage
{
    IpcamMessage parent;
};

struct _IpcamResponseMessageClass
{
    IpcamMessageClass parent_class;
};

GType ipcam_response_message_get_type(void);
gboolean ipcam_response_message_has_error(IpcamResponseMessage *response_message);
const gchar *ipcam_response_message_get_action(IpcamResponseMessage *response_message);
const gchar *ipcam_response_message_get_id(IpcamResponseMessage *response_message);
const gchar *ipcam_response_message_get_code(IpcamResponseMessage *response_message);

#endif /* __RESPONSE_MESSAGE_H__ */
