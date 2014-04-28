#ifndef __SERVICE_H__
#define __SERVICE_H__

#include "base_service.h"

#define IPCAM_SERVICE_TYPE (ipcam_service_get_type())
#define IPCAM_SERVICE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_SERVICE_TYPE, IpcamService))
#define IPCAM_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_SERVICE_TYPE, IpcamServiceClass))
#define IPCAM_IS_SERVICE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_SERVICE_TYPE))
#define IPCAM_IS_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_SERVICE_TYPE))
#define IPCAM_SERVICE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_SERVICE_TYPE, IpcamServiceClass))

typedef struct _IpcamService IpcamService;
typedef struct _IpcamServiceClass IpcamServiceClass;

struct _IpcamService {
    IpcamBaseService parent;
    //
};

struct _IpcamServiceClass {
    IpcamBaseServiceClass parent_class;
    //
    void (*server_receive_string)(IpcamService *self, const gchar *name, const gchar *client_id, const gchar *string);
    void (*client_receive_string)(IpcamService *self, const gchar *name, const gchar *string);
};

GType ipcam_service_get_type(void);
gboolean ipcam_service_send_strings(IpcamService *service,
                                    const gchar *name,
                                    const gchar *strings[],
                                    const gchar *client_id);
gboolean ipcam_service_is_server(IpcamService *service, const gchar *name);
gboolean ipcam_service_is_client(IpcamService *service, const gchar *name);
gboolean ipcam_service_connect_by_name(IpcamService *service,
                                       const gchar *name,
                                       const gchar *address,
                                       const gchar *client_id);
gboolean ipcam_service_bind_by_name(IpcamService *service, const gchar *name, const gchar *address);

#endif /* __SERVICE_H__*/
