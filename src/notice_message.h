#ifndef __NOTICE_MESSAGE_H__
#define __NOTICE_MESSAGE_H__

#include "message.h"

#define IPCAM_NOTICE_MESSAGE_TYPE (ipcam_notice_message_get_type())
#define IPCAM_NOTICE_MESSAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_NOTICE_MESSAGE_TYPE, IpcamNoticeMessage))
#define IPCAM_NOTICE_MESSAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_NOTICE_MESSAGE_TYPE, IpcamNoticeMessageClass))
#define IPCAM_IS_NOTICE_MESSAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_NOTICE_MESSAGE_TYPE))
#define IPCAM_IS_NOTICE_MESSAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_NOTICE_MESSAGE_TYPE))
#define IPCAM_NOTICE_MESSAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_NOTICE_MESSAGE_TYPE, IpcamNoticeMessageClass))

typedef struct _IpcamNoticeMessage IpcamNoticeMessage;
typedef struct _IpcamNoticeMessageClass IpcamNoticeMessageClass;

struct _IpcamNoticeMessage
{
    IpcamMessage parent;
};

struct _IpcamNoticeMessageClass
{
    IpcamMessageClass parent_class;
};

GType ipcam_notice_message_get_type(void);

#endif /* __NOTICE_MESSAGE_H__ */
