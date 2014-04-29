#ifndef __EVENT_HANDLER_H__
#define __ACTION_HANDLER_H__

#include <glib.h>
#include <glib-object.h>
#include "service.h"
#include "message.h"

#define IPCAM_EVENT_HANDLER_TYPE (ipcam_event_handler_get_type())
#define IPCAM_EVENT_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_EVENT_HANDLER_TYPE, IpcamEventHandler))
#define IPCAM_EVENT_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_EVENT_HANDLER_TYPE, IpcamEventHandlerClass))
#define IPCAM_IS_EVENT_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_EVENT_HANDLER_TYPE))
#define IPCAM_IS_EVENT_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_EVENT_HANDLER_TYPE))
#define IPCAM_EVENT_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_EVENT_HANDLER_TYPE, IpcamEventHandlerClass))

typedef struct _IpcamEventHandler IpcamEventHandler;
typedef struct _IpcamEventHandlerClass IpcamEventHandlerClass;

struct _IpcamEventHandler
{
    GObject parent;
};

struct _IpcamEventHandlerClass
{
    GObjectClass parent_class;
    void (*run)(IpcamEventHandler *event_handler, IpcamMessage *message);
};

GType ipcam_event_handler_get_type(void);
void ipcam_event_handler_run(IpcamEventHandler *event_handler,
                             IpcamMessage *message);

#endif /* __EVENT_HANDLER_H__ */
