#include "service.h"
#include <zmq.h>
#include <assert.h>

G_DEFINE_TYPE(IpcamService, ipcam_service, IPCAM_BASE_SERVICE_TYPE);

static GObject *ipcam_service_constructor(GType self_type,
                                          guint n_properties,
                                          GObjectConstructParam *properties)
{
    GObject *obj;
    
    obj = G_OBJECT_CLASS(ipcam_service_parent_class)->constructor(self_type, n_properties, properties);

    return obj;
}
static void ipcam_service_dispose(GObject *self)
{
    static gboolean first_run = TRUE;

    if (first_run)
    {
        first_run = FALSE;
        G_OBJECT_CLASS(ipcam_service_parent_class)->dispose(self);
    }
}
static void ipcam_service_finalize(GObject *self)
{
    G_OBJECT_CLASS(ipcam_service_parent_class)->finalize(self);
}
static void ipcam_service_init(IpcamService *self)
{
}
static void ipcam_service_class_init(IpcamServiceClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    
    this_class->constructor = &ipcam_service_constructor;
    this_class->dispose = &ipcam_service_dispose;
    this_class->finalize = &ipcam_service_finalize;
}
