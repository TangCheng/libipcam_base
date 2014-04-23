#ifndef __BASE_SERVICE_H__
#define __BASE_SERVICE_H__

#include <glib.h>
#include <glib-object.h>

#define IPCAM_BASE_SERVICE_TYPE (ipcam_base_service_get_type())
#define IPCAM_BASE_SERVICE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_BASE_SERVICE_TYPE, IpcamBaseService))
#define IPCAM_BASE_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_BASE_SERVICE_TYPE, IpcamBaseServiceClass))
#define IPCAM_IS_BASE_SERVICE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_BASE_SERVICE_TYPE))
#define IPCAM_IS_BASE_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_BASE_SERVICE_TYPE))
#define IPCAM_BASE_SERVICE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_BASE_SERVICE_TYPE, IpcamBaseServiceClass))

typedef struct _IpcamBaseService IpcamBaseService;
typedef struct _IpcamBaseServiceClass IpcamBaseServiceClass;

struct _IpcamBaseService {
    GObject parent;
    //
};

struct _IpcamBaseServiceClass {
    GObjectClass parent_class;
    // public virtual function
    void (*start)(IpcamBaseService *self);
    void (*stop)(IpcamBaseService *self);
    void *(*bind)(IpcamBaseService *self, gchar *address);
    void *(*connect)(IpcamBaseService *self, gchar *identity, gchar *address);
    // public pure virtual function
    void (*before)(IpcamBaseService *self);
    void (*in_loop)(IpcamBaseService *self);
    void (*on_read)(IpcamBaseService *self, void *mq_socket);
};

GType ipcam_base_service_get_type(void);
void ipcam_base_service_start(IpcamBaseService *base_service);
void ipcam_base_service_stop(IpcamBaseService *base_service);
void* ipcam_base_service_bind(IpcamBaseService *base_service, gchar *address);
void* ipcam_base_service_connect(IpcamBaseService *base_service, gchar *identity, gchar *address);

#endif /* __BASE_SERVICE_H__*/
