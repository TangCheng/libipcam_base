#include "app.h"


G_DEFINE_TYPE(IpcamApp, ipcam_app, G_TYPE_OBJECT);

static GObject *ipcam_app_constructor(GType self_type,
                                      guint n_properties,
                                      GObjectConstructParam *properties)
{
    GObject *obj;
    
    obj = G_OBJECT_CLASS(ipcam_app_parent_class)->constructor(self_type, n_properties, properties);

    return obj;
}
static void ipcam_app_dispose(GObject *self)
{
    static gboolean first_run = TRUE;

    if (first_run)
    {
        first_run = FALSE;
        G_OBJECT_CLASS(ipcam_app_parent_class)->dispose(self);
    }
}
static void ipcam_app_finalize(GObject *self)
{
    G_OBJECT_CLASS(ipcam_app_parent_class)->finalize(self);
}
static void ipcam_app_init(IpcamApp *self)
{
}
static void ipcam_app_class_init(IpcamAppClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    
    this_class->constructor = &ipcam_app_constructor;
    this_class->dispose = &ipcam_app_dispose;
    this_class->finalize = &ipcam_app_finalize;
}
