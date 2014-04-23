#include "message.h"
#include "notice_message.h"
#include "request_message.h"
#include "response_message.h"
#include <json.h>
#include <string.h>

enum
{
    PROP_0,
    
    IPCAM_MESSAGE_MSGTYPE = 1,
    IPCAM_MESSAGE_TOKEN = 2,
    IPCAM_MESSAGE_VERSION = 3,
    IPCAM_MESSAGE_BODY = 4,
    
    N_PROPERTIES
};

typedef struct _IpcamMessagePrivate
{
    gchar *type;
    gchar *token;
    gchar *version;
    gchar *body;
} IpcamMessagePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamMessage, ipcam_message, G_TYPE_OBJECT);

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

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
static void ipcam_message_get_property(GObject *object,
                                       guint property_id,
                                       GValue *value,
                                       GParamSpec *pspec)
{
    IpcamMessage *self = IPCAM_MESSAGE(object);
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(self);
    switch(property_id)
    {
    case IPCAM_MESSAGE_MSGTYPE:
        {
            g_value_set_string(value, priv->type);
        }
        break;
    case IPCAM_MESSAGE_TOKEN:
        {
            g_value_set_string(value, priv->token);
        }
        break;
    case IPCAM_MESSAGE_VERSION:
        {
            g_value_set_string(value, priv->version);
        }
        break;
    case IPCAM_MESSAGE_BODY:
        {
            g_value_set_string(value, priv->body);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_message_set_property(GObject *object,
                                       guint property_id,
                                       const GValue *value,
                                       GParamSpec *pspec)
{
    IpcamMessage *self = IPCAM_MESSAGE(object);
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(self);
    switch(property_id)
    {
    case IPCAM_MESSAGE_MSGTYPE:
        {
            g_free(priv->type);
            priv->type = g_value_dup_string(value);
            g_print("ipcam message type: %s\n", priv->type);
        }
        break;
    case IPCAM_MESSAGE_TOKEN:
        {
            g_free(priv->token);
            priv->token = g_value_dup_string(value);
            g_print("ipcam message token: %s\n", priv->token);
        }
        break;
    case IPCAM_MESSAGE_VERSION:
        {
            g_free(priv->version);
            priv->version = g_value_dup_string(value);
            g_print("ipcam message version: %s\n", priv->version);
        }
        break;
    case IPCAM_MESSAGE_BODY:
        {
            g_free(priv->body);
            priv->body = g_value_dup_string(value);
            g_print("ipcam message body: %s\n", priv->body);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_message_init(IpcamMessage *self)
{
}
static void ipcam_message_class_init(IpcamMessageClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    
    this_class->constructor = &ipcam_message_constructor;
    this_class->dispose = &ipcam_message_dispose;
    this_class->finalize = &ipcam_message_finalize;
    this_class->get_property = &ipcam_message_get_property;
    this_class->set_property = &ipcam_message_set_property;

    obj_properties[IPCAM_MESSAGE_MSGTYPE] =
        g_param_spec_string("message-type",
                            "Message type",
                            "Set message type",
                            "", // default value
                            G_PARAM_READWRITE);
    obj_properties[IPCAM_MESSAGE_TOKEN] =
        g_param_spec_string("message-token",
                            "Message token",
                            "Set message token",
                            "", // default value
                            G_PARAM_READWRITE);
    obj_properties[IPCAM_MESSAGE_VERSION] =
        g_param_spec_string("message-version",
                            "Message version",
                            "Set message version",
                            "1.0", // default value
                            G_PARAM_READWRITE);
    obj_properties[IPCAM_MESSAGE_BODY] =
        g_param_spec_string("message-body",
                            "Message body",
                            "Set message body",
                            "", // default value
                            G_PARAM_READWRITE);
    
    g_object_class_install_properties(this_class, N_PROPERTIES, obj_properties);
}
static const gchar *ipcam_message_get_head_attr(json_object *json, const gchar *attr)
{
    json_object *jobj;
    g_return_val_if_fail(json_object_object_get_ex(json, "head", &jobj), NULL);
    g_return_val_if_fail(json_object_object_get_ex(jobj, attr, &jobj), NULL);
    return json_object_get_string(jobj);
}
static const gchar *ipcam_message_get_body(json_object *json)
{
    json_object *jobj;
    g_return_val_if_fail(json_object_object_get_ex(json, "body", &jobj), NULL);
    return json_object_get_string(jobj);
}
static gboolean ipcam_message_validate_message(json_object *json)
{
    // ToDo
    return TRUE;
}
IpcamMessage *ipcam_message_parse_from_string(const gchar *json_str)
{
    IpcamMessage *message = NULL;
    struct json_object *json = json_tokener_parse(json_str);
    g_return_val_if_fail(json, NULL);
    g_return_val_if_fail(ipcam_message_validate_message(json), NULL);
    
    const gchar *type = ipcam_message_get_head_attr(json, "type");
    if (0 == strcmp(type, "request"))
    {
        message = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE, NULL);
        g_object_set(G_OBJECT(message),
                     "message-action", ipcam_message_get_head_attr(json, "action"),
                     "message-id", ipcam_message_get_head_attr(json, "id"),
                     NULL);
    }
    else if (0 == strcmp(type, "response"))
    {
        message = g_object_new(IPCAM_RESPONSE_MESSAGE_TYPE, NULL);
        g_object_set(G_OBJECT(message),
                     "message-action", ipcam_message_get_head_attr(json, "action"),
                     "message-id", ipcam_message_get_head_attr(json, "id"),
                     "message-code", ipcam_message_get_head_attr(json, "code"),
                     NULL);
    }
    else if (0 == strcmp(type, "notice"))
    {
        message = g_object_new(IPCAM_NOTICE_MESSAGE_TYPE, NULL);
        g_object_set(G_OBJECT(message), "message-event", ipcam_message_get_head_attr(json, "event"), NULL);
    }
    if (NULL != message)
    {
        g_object_set(G_OBJECT(message),
                     "message-token", ipcam_message_get_head_attr(json, "token"),
                     "message-version", ipcam_message_get_head_attr(json, "version"),
                     "message-body", ipcam_message_get_body(json),
                     NULL);
    }
    json_object_put(json);
    return message;
}
gboolean ipcam_message_is_request(IpcamMessage *message)
{
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(message);
    return (0 == strcmp(priv->type, "request"));
}
gboolean ipcam_message_is_response(IpcamMessage *message)
{
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(message);
    return (0 == strcmp(priv->type, "response"));
}
gboolean ipcam_message_is_notice(IpcamMessage *message)
{
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(message);
    return (0 == strcmp(priv->type, "notice"));
}
const gchar *ipcam_message_to_string(IpcamMessage *message)
{
    // ToDo
    return NULL;
}
