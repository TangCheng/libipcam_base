#include "action_handler.h"
#include "service.h"

enum
{
    PROP_0,
    
    IPCAM_ACTION_HANDLER_SERVICE = 1,
    
    N_PROPERTIES
};

typedef struct _IpcamActionHandlerPrivate
{
    IpcamService *service;
} IpcamActionHandlerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamActionHandler, ipcam_action_handler, G_TYPE_OBJECT);

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static void ipcam_action_handler_get_property(GObject *object,
                                              guint property_id,
                                              GValue *value,
                                              GParamSpec *pspec)
{
    IpcamActionHandler *self = IPCAM_ACTION_HANDLER(object);
    switch(property_id)
    {
    case IPCAM_ACTION_HANDLER_SERVICE:
        {
            IpcamActionHandlerPrivate *priv = ipcam_action_handler_get_instance_private(self);
            g_value_set_object(value, priv->service);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_action_handler_set_property(GObject *object,
                                              guint property_id,
                                              const GValue *value,
                                              GParamSpec *pspec)
{
    IpcamActionHandler *self = IPCAM_ACTION_HANDLER(object);
    switch(property_id)
    {
    case IPCAM_ACTION_HANDLER_SERVICE:
        {
            IpcamActionHandlerPrivate *priv = ipcam_action_handler_get_instance_private(self);
            priv->service = g_value_get_object(value);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_action_handler_init(IpcamActionHandler *self)
{
}
static void ipcam_action_handler_class_init(IpcamActionHandlerClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    this_class->set_property = &ipcam_action_handler_set_property;
    this_class->get_property = &ipcam_action_handler_get_property;

    obj_properties[IPCAM_ACTION_HANDLER_SERVICE] =
        g_param_spec_object("service",
                            "Handler owner",
                            "The service which owned this action handler",
                            IPCAM_SERVICE_TYPE, // default value
                            G_PARAM_READWRITE);
    
    g_object_class_install_properties(this_class, N_PROPERTIES, obj_properties);

    klass->run = NULL;
}

void ipcam_action_handler_run(IpcamActionHandler *action_handler,
                              IpcamMessage *message)
{
    g_return_if_fail(IPCAM_IS_ACTION_HANDLER(action_handler));
    if (IPCAM_ACTION_HANDLER_GET_CLASS(action_handler)->run != NULL)
        IPCAM_ACTION_HANDLER_GET_CLASS(action_handler)->run(action_handler, message);
    else
        g_warning ("Class '%s' does not override the mandatory "
                   "IpcamActionHandler.run() virtual function.",
                   G_OBJECT_TYPE_NAME(action_handler));
}
