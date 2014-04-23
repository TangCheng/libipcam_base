#ifndef __POLLER_H__
#define __POLLER_H__

#include <glib.h>
#include <glib-object.h>

#define IPCAM_POLLER_TYPE (ipcam_poller_get_type())
#define IPCAM_POLLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_POLLER_TYPE, IpcamPoller))
#define IPCAM_POLLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_POLLER_TYPE, IpcamPollerClass))
#define IPCAM_IS_POLLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_POLLER_TYPE))
#define IPCAM_IS_POLLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_POLLER_TYPE))
#define IPCAM_POLLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_POLLER_TYPE, IpcamPollerClass))

typedef struct _IpcamPoller IpcamPoller;
typedef struct _IpcamPollerClass IpcamPollerClass;

struct _IpcamPoller
{
    GObject parent;
};

struct _IpcamPollerClass
{
    GObjectClass parent_class;
};

GType ipcam_poller_get_type(void);
void ipcam_poller_register(IpcamPoller *poller, void *mq_socket, gint event);
void ipcam_poller_unregister(IpcamPoller *poller, void *mq_socket);
gint ipcam_poller_poll(IpcamPoller *poller);
guint ipcam_poller_get_sockets(IpcamPoller *poller, void *mq_sockets[]);

#endif /* __POLLER_H__ */
