#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <glib-object.h>

#define IPCAM_SERVICE_TYPE (ipcam_service_get_type())
#define IPCAM_SERVICE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_SERVICE_TYPE, IpcamService))
#define IPCAM_SERVICE_CLASS(klass) (GTYPE_CHECK_CLASS_CAST((klass), IPCAM_SERVICE_TYPE, IpcamServiceClass))
#define IPCAM_IS_SERVICE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_SERVICE_TYPE))
#define IPCAM_IS_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_SERVICE_TYPE))
#define IPCAM_SERVICE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_SERVICE_TYPE, IpcamServiceClass))

typedef struct _IpcamService IpcamService;
typedef struct _IpcamServiceClass IpcamServiceClass;
typedef struct _IpcamServicePriv IpcamServicePriv;

struct _IpcamService {
    GObject parent;
    //
};

struct _IpcamServiceClass {
    GObjectClass parent_class;
    //
};

GType ipcam_service_get_type(void);

#endif /* __SERVICE_H__*/
