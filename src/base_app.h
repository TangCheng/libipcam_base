#ifndef __BASE_APP_H__
#define __BASE_APP_H__

#include "service.h"

#define IPCAM_BASE_APP_TYPE (ipcam_service_get_type())
#define IPCAM_BASE_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_BASE_APP_TYPE, IpcamBaseApp))
#define IPCAM_BASE_APP_CLASS(klass) (GTYPE_CHECK_CLASS_CAST((klass), IPCAM_BASE_APP_TYPE, IpcamBaseAppClass))
#define IPCAM_IS_BASE_APP(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_BASE_APP_TYPE))
#define IPCAM_IS_BASE_APP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_BASE_APP_TYPE))
#define IPCAM_BASE_APP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_BASE_APP_TYPE, IpcamBaseAppClass))

typedef struct _IpcamBaseApp IpcamBaseApp;
typedef struct _IpcamBaseAppClass IpcamBaseAppClass;

struct _IpcamBaseApp {
    IpcamService parent;
    //
};

struct _IpcamBaseAppClass {
    IpcamServiceClass parent_class;
    //
};

GType ipcam_base_app_get_type(void);
void ipcam_base_app_register_handler(IpcamBaseApp *base_app,
                                     const gchar *handler_name,
                                     GType handler_class_type);

#endif /* __BASE_APP_H__*/
