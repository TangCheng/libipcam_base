#ifndef __BASE_SERVICE_H__
#define __BASE_SERVICE_H__

#include <glib.h>
#include <glib-object.h>

#define IPCAM_BASE_SERVICE_TYPE (ipcam_base_service_get_type())
#define IPCAM_BASE_SERVICE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_BASE_SERVICE_TYPE, IpcamBaseService))
#define IPCAM_BASE_SERVICE_CLASS(klass) (GTYPE_CHECK_CLASS_CAST((klass), IPCAM_BASE_SERVICE_TYPE, IpcamBaseServiceClass))
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
    //
};

GType ipcam_base_service_get_type(void);

#endif /* __BASE_SERVICE_H__*/
