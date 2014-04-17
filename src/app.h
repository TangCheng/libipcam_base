#ifndef __APP_H__
#define __APP_H__

#include "base_app.h"

#define IPCAM_APP_TYPE (ipcam_base_service_get_type())
#define IPCAM_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_APP_TYPE, IpcamApp))
#define IPCAM_APP_CLASS(klass) (GTYPE_CHECK_CLASS_CAST((klass), IPCAM_APP_TYPE, IpcamAppClass))
#define IPCAM_IS_APP(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_APP_TYPE))
#define IPCAM_IS_APP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_APP_TYPE))
#define IPCAM_APP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_APP_TYPE, IpcamAppClass))

typedef struct _IpcamApp IpcamApp;
typedef struct _IpcamAppClass IpcamAppClass;

struct _IpcamApp {
    IpcamBaseApp parent;
    //
};

struct _IpcamAppClass {
    IpcamBaseAppClass parent_class;
    //
};

GType ipcam_app_get_type(void);

#endif /* __APP_H__*/
