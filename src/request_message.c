#include "request_message.h"
#include "response_message.h"
#include "utils.h"

enum
{
    PROP_0,
    
    IPCAM_REQUEST_MESSAGE_ACTION = 1,
    IPCAM_REQUEST_MESSAGE_ID = 2,
    
    N_PROPERTIES
};

typedef struct _IpcamRequestMessagePrivate
{
    gchar *action;
    gchar *id;
} IpcamRequestMessagePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamRequestMessage, ipcam_request_message, IPCAM_MESSAGE_TYPE);

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static void ipcam_request_message_get_property(GObject *object,
                                               guint property_id,
                                               GValue *value,
                                               GParamSpec *pspec)
{
    IpcamRequestMessage *self = IPCAM_REQUEST_MESSAGE(object);
    IpcamRequestMessagePrivate *priv = ipcam_request_message_get_instance_private(self);
    switch(property_id)
    {
    case IPCAM_REQUEST_MESSAGE_ACTION:
        {
            g_value_set_string(value, priv->action);
        }
        break;
    case IPCAM_REQUEST_MESSAGE_ID:
        {
            g_value_set_string(value, priv->id);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_request_message_set_property(GObject *object,
                                               guint property_id,
                                               const GValue *value,
                                               GParamSpec *pspec)
{
    IpcamRequestMessage *self = IPCAM_REQUEST_MESSAGE(object);
    IpcamRequestMessagePrivate *priv = ipcam_request_message_get_instance_private(self);
    switch(property_id)
    {
    case IPCAM_REQUEST_MESSAGE_ACTION:
        {
            g_free(priv->action);
            priv->action = g_value_dup_string(value);
            g_print("ipcam request message action: %s\n", priv->action);
        }
        break;
    case IPCAM_REQUEST_MESSAGE_ID:
        {
            g_free(priv->id);
            priv->id = g_value_dup_string(value);
            g_print("ipcam request message id: %s\n", priv->id);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_request_message_init(IpcamRequestMessage *self)
{
    gchar message_id[17] = {0};
    GValue val = {0, };
    g_value_init(&val, G_TYPE_STRING);
    g_value_set_string(&val, "request");
    g_object_set_property(G_OBJECT(self), "message-type", &val);
    ipcam_get_rand_str(message_id, 16);
    g_value_set_string(&val, message_id);
    g_object_set_property(G_OBJECT(self), "message-id", &val);
}
static void ipcam_request_message_class_init(IpcamRequestMessageClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    
    this_class->get_property = &ipcam_request_message_get_property;
    this_class->set_property = &ipcam_request_message_set_property;

    obj_properties[IPCAM_REQUEST_MESSAGE_ACTION] =
        g_param_spec_string("message-action",
                            "Request message action",
                            "Set request message action",
                            "", // default value
                            G_PARAM_READWRITE);
    obj_properties[IPCAM_REQUEST_MESSAGE_ID] =
        g_param_spec_string("message-id",
                            "Request message id",
                            "Set request message id",
                            "", // default value
                            G_PARAM_READWRITE);
    
    g_object_class_install_properties(this_class, N_PROPERTIES, obj_properties);
}
IpcamMessage *ipcam_request_message_get_response_message(IpcamRequestMessage *request_message, const gchar *code)
{
    IpcamRequestMessagePrivate *priv = ipcam_request_message_get_instance_private(request_message);
    IpcamMessage *message = g_object_new(IPCAM_RESPONSE_MESSAGE_TYPE,
                                         "message-action", priv->action,
                                         "message-id", priv->id,
                                         "message-code", code,
                                         NULL);
    return message;
}
