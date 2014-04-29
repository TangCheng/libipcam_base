#include "event_handler.h"
#include "service.h"

enum
{
    PROP_0,
    
    IPCAM_EVENT_HANDLER_SERVICE = 1,
    
    N_PROPERTIES
};

typedef struct _IpcamEventHandlerPrivate
{
    IpcamService *service;
} IpcamEventHandlerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamEventHandler, ipcam_event_handler, G_TYPE_OBJECT);

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static void ipcam_event_handler_get_property(GObject *object,
                                             guint property_id,
                                             GValue *value,
                                             GParamSpec *pspec)
{
    IpcamEventHandler *self = IPCAM_EVENT_HANDLER(object);
    switch(property_id)
    {
    case IPCAM_EVENT_HANDLER_SERVICE:
        {
            IpcamEventHandlerPrivate *priv = ipcam_event_handler_get_instance_private(self);
            g_value_set_object(value, priv->service);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_event_handler_set_property(GObject *object,
                                             guint property_id,
                                             const GValue *value,
                                             GParamSpec *pspec)
{
    IpcamEventHandler *self = IPCAM_EVENT_HANDLER(object);
    switch(property_id)
    {
    case IPCAM_EVENT_HANDLER_SERVICE:
        {
            IpcamEventHandlerPrivate *priv = ipcam_event_handler_get_instance_private(self);
            priv->service = g_value_get_object(value);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_event_handler_init(IpcamEventHandler *self)
{
}
static void ipcam_event_handler_class_init(IpcamEventHandlerClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    this_class->set_property = &ipcam_event_handler_set_property;
    this_class->get_property = &ipcam_event_handler_get_property;

    obj_properties[IPCAM_EVENT_HANDLER_SERVICE] =
        g_param_spec_object("service",
                            "Handler owner",
                            "The service which owned this event handler",
                            IPCAM_SERVICE_TYPE, // default value
                            G_PARAM_READWRITE);
    
    g_object_class_install_properties(this_class, N_PROPERTIES, obj_properties);

    klass->run = NULL;
}

void ipcam_event_handler_run(IpcamEventHandler *event_handler,
                             IpcamMessage *message)
{
    g_return_if_fail(IPCAM_IS_EVENT_HANDLER(event_handler));
    if (IPCAM_EVENT_HANDLER_GET_CLASS(event_handler)->run != NULL)
        IPCAM_EVENT_HANDLER_GET_CLASS(event_handler)->run(event_handler, message);
    else
        g_warning ("Class '%s' does not override the mandatory "
                   "IpcamEventHandler.run() virtual function.",
                   G_OBJECT_TYPE_NAME(event_handler));
}
