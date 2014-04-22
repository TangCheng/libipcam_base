#ifndef __REQUEST_MESSAGE_H__
#define __REQUEST_MESSAGE_H__

#include "message.h"

#define IPCAM_REQUEST_MESSAGE_TYPE (ipcam_request_message_get_type())
#define IPCAM_REQUEST_MESSAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_REQUEST_MESSAGE_TYPE, IpcamRequestMessage))
#define IPCAM_REQUEST_MESSAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_REQUEST_MESSAGE_TYPE, IpcamRequestMessageClass))
#define IPCAM_IS_REQUEST_MESSAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_REQUEST_MESSAGE_TYPE))
#define IPCAM_IS_REQUEST_MESSAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_REQUEST_MESSAGE_TYPE))
#define IPCAM_REQUEST_MESSAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_REQUEST_MESSAGE_TYPE, IpcamRequestMessageClass))

typedef struct _IpcamRequestMessage IpcamRequestMessage;
typedef struct _IpcamRequestMessageClass IpcamRequestMessageClass;

struct _IpcamRequestMessage
{
    IpcamMessage parent;
};

struct _IpcamRequestMessageClass
{
    IpcamMessageClass parent_class;
};

GType ipcam_request_message_get_type(void);
IpcamMessage *ipcam_request_message_get_response_message(IpcamRequestMessage *request_message, const gchar *code);

#endif /* __REQUEST_MESSAGE_H__ */
