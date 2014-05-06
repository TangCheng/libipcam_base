#ifndef __APP1_H__
#define __APP1_H__

#include "base_app.h"

#define IPCAM_APP1_TYPE (ipcam_app1_get_type())
#define IPCAM_APP1(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_APP1_TYPE, IpcamApp))
#define IPCAM_APP1_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_APP1_TYPE, IpcamAppClass))
#define IPCAM_IS_APP1(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_APP1_TYPE))
#define IPCAM_IS_APP1_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_APP1_TYPE))
#define IPCAM_APP1_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_APP1_TYPE, IpcamAppClass))

typedef struct _IpcamApp1 IpcamApp1;
typedef struct _IpcamApp1Class IpcamApp1Class;

struct _IpcamApp1
{
    IpcamBaseApp parent;
};

struct _IpcamApp1Class
{
    IpcamBaseAppClass parent_class;
};

GType ipcam_app1_get_type(void);

#endif /* __APP1_H__ */
