#ifndef __TEST_EVENT_HANDLER_H__
#define __TEST_EVENT_HANDLER_H__

#include "event_handler.h"

#define IPCAM_TEST_EVENT_HANDLER_TYPE (ipcam_test_event_handler_get_type())
#define IPCAM_TEST_EVENT_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_TEST_EVENT_HANDLER_TYPE, IpcamTestEventHandler))
#define IPCAM_TEST_EVENT_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_TEST_EVENT_HANDLER_TYPE, IpcamTestEventHandlerClass))
#define IPCAM_IS_TEST_EVENT_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_TEST_EVENT_HANDLER_TYPE))
#define IPCAM_IS_TEST_EVENT_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_TEST_EVENT_HANDLER_TYPE))
#define IPCAM_TEST_EVENT_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_TEST_EVENT_HANDLER_TYPE, IpcamTestEventHandlerClass))

typedef struct _IpcamTestEventHandler IpcamTestEventHandler;
typedef struct _IpcamTestEventHandlerClass IpcamTestEventHandlerClass;

struct _IpcamTestEventHandler
{
    IpcamEventHandler parent;
};

struct _IpcamTestEventHandlerClass
{
    IpcamEventHandlerClass parent_class;
};

GType ipcam_test_event_handler_get_type(void);

#endif /* __TEST_EVENT_HANDLER_H__ */
