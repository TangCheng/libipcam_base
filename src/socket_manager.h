#ifndef __SOCKET_MANAGER_H__
#define __SOCKET_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

enum
{
     IPCAM_SOCKET_TYPE_SERVER = 0,
     IPCAM_SOCKET_TYPE_CLIENT = 1,
};
     
#define IPCAM_SOCKET_MANAGER_TYPE (ipcam_socket_manager_get_type())
#define IPCAM_SOCKET_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_SOCKET_MANAGER_TYPE, IpcamSocketManager))
#define IPCAM_SOCKET_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_SOCKET_MANAGER_TYPE, IpcamSocketManagerClass))
#define IPCAM_IS_SOCKET_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_SOCKET_MANAGER_TYPE))
#define IPCAM_IS_SOCKET_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_SOCKET_MANAGER_TYPE))
#define IPCAM_SOCKET_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_SOCKET_MANAGER_TYPE, IpcamSocketManagerClass))

typedef struct _IpcamSocketManager IpcamSocketManager;
typedef struct _IpcamSocketManagerClass IpcamSocketManagerClass;

struct _IpcamSocketManager
{
    GObject parent;
};

struct _IpcamSocketManagerClass
{
    GObjectClass parent_class;
};

GType ipcam_socket_manager_get_type(void);
gboolean ipcam_socket_manager_add(IpcamSocketManager *socket_manager, const gchar *name, const int type, const void *mq_socket);
gboolean ipcam_socket_manager_delete_by_socket(IpcamSocketManager *socket_manager, const void *mq_socket);
gboolean ipcam_socket_manager_delete_by_name(IpcamSocketManager *socket_manager, const gchar *name);
gboolean ipcam_socket_manager_has_name(IpcamSocketManager *socket_manager, const gchar *name);
gboolean ipcam_socket_manager_has_socket(IpcamSocketManager *socket_manager, const void *mq_socket);
gboolean ipcam_socket_manager_get_by_name(IpcamSocketManager *socket_manager, const gchar *name, int *type, void **mq_socket);
gboolean ipcam_socket_manager_get_by_socket(IpcamSocketManager *socket_manager, const void *mq_socket, gchar *name, int *type);
void ipcam_socket_manager_close_all_socket(IpcamSocketManager *socket_manager);

#endif /* __SOCKET_MANAGER_H__ */
