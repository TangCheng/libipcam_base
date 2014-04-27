
#include "base_app.h"
#include "timer_manager.h"
#include "message_manager.h"

#define IPCAM_TIMER_CLIENT_NAME "_timer_client"

typedef struct _IpcamBaseAppPrivate
{
    IpcamTimerManager *timer_manager;
    IpcamMessageManager *msg_manager;
} IpcamBaseAppPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamBaseApp, ipcam_base_app, IPCAM_SERVICE_TYPE);

static GObject *ipcam_base_app_constructor(GType self_type,
                                           guint n_properties,
                                           GObjectConstructParam *properties)
{
    GObject *obj;
    
    obj = G_OBJECT_CLASS(ipcam_base_app_parent_class)->constructor(self_type, n_properties, properties);

    return obj;
}
static void ipcam_base_app_dispose(GObject *self)
{
    static gboolean first_run = TRUE;

    if (first_run)
    {
        first_run = FALSE;
        G_OBJECT_CLASS(ipcam_base_app_parent_class)->dispose(self);
    }
}
static void ipcam_base_app_finalize(GObject *self)
{
    G_OBJECT_CLASS(ipcam_base_app_parent_class)->finalize(self);
}
static void ipcam_base_app_init(IpcamBaseApp *self)
{
}
static void ipcam_base_app_class_init(IpcamBaseAppClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    
    this_class->constructor = &ipcam_base_app_constructor;
    this_class->dispose = &ipcam_base_app_dispose;
    this_class->finalize = &ipcam_base_app_finalize;
}
