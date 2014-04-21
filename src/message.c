#include "message.h"

typedef struct _IpcamMessagePrivate
{
} IpcamMessagePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamMessage, ipcam_message, G_TYPE_OBJECT);

static GObject *ipcam_message_constructor(GType self_type,
                                          guint n_properties,
                                          GObjectConstructParam *properties)
{
    GObject *obj;
    obj = G_OBJECT_CLASS(ipcam_message_parent_class)->constructor(self_type, n_properties, properties);
    return obj;
}
static void ipcam_message_dispose(GObject *self)
{
    static gboolean first_run = TRUE;
    if (first_run)
    {
        first_run = FALSE;
        G_OBJECT_CLASS(ipcam_message_parent_class)->dispose(self);
    }
}
static void ipcam_message_finalize(GObject *self)
{
    G_OBJECT_CLASS(ipcam_message_parent_class)->finalize(self);
}
static void ipcam_message_init(IpcamMessage *self)
{
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(self);
}
static void ipcam_message_class_init(IpcamMessageClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    
    this_class->constructor = &ipcam_message_constructor;
    this_class->dispose = &ipcam_message_dispose;
    this_class->finalize = &ipcam_message_finalize;
}

