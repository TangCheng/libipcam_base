#include "messages.h"
#include <json-glib/json-glib.h>
#include <string.h>
#include <assert.h>

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
    JsonNode *body;
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
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(IPCAM_MESSAGE(self));
    g_free(priv->type);
    g_free(priv->token);
    g_free(priv->version);
    if (priv->body)
    {
        json_node_free(priv->body);
    }
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
            g_value_set_pointer(value, priv->body);
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
            json_node_free(priv->body);
            priv->body = g_value_get_pointer(value);
            g_print("ipcam message body: %p\n", priv->body);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_message_init(IpcamMessage *self)
{
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(self);
    priv->type = g_strdup("");
    priv->token = g_strdup("");
    priv->version = g_strdup("1.0");
    priv->body = NULL;
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
        g_param_spec_string("type",
                            "Message type",
                            "Set message type",
                            "", // default value
                            G_PARAM_READWRITE);
    obj_properties[IPCAM_MESSAGE_TOKEN] =
        g_param_spec_string("token",
                            "Message token",
                            "Set message token",
                            "", // default value
                            G_PARAM_READWRITE);
    obj_properties[IPCAM_MESSAGE_VERSION] =
        g_param_spec_string("version",
                            "Message version",
                            "Set message version",
                            "1.0", // default value
                            G_PARAM_READWRITE);
    obj_properties[IPCAM_MESSAGE_BODY] =
        g_param_spec_pointer("body",
                             "Message body",
                             "Set message body",
                             G_PARAM_READWRITE);
    
    g_object_class_install_properties(this_class, N_PROPERTIES, obj_properties);
}

static gboolean ipcam_message_validate_message(JsonNode *json)
{
    // ToDo
    return TRUE;
}

IpcamMessage *ipcam_message_parse_from_string(const gchar *json_str)
{
    IpcamMessage *message = NULL;
    JsonParser *parser = json_parser_new();
    JsonNode *jmsg;

    if (!json_parser_load_from_data(parser, json_str, -1, NULL))
    {
        g_object_unref(parser);
        return NULL;
    }
    jmsg = json_parser_get_root(parser);
    if (!ipcam_message_validate_message(jmsg))
    {
        g_object_unref(parser);
        return NULL;
    }

    JsonObject *head = json_object_get_object_member(json_node_get_object(jmsg), "head");
    const gchar *type = json_object_get_string_member(head, "type");
    const gchar *action = json_object_get_string_member(head, "action");
    const gchar *id = json_object_get_string_member(head, "id");
    const gchar *code = json_object_get_string_member(head, "code");
    const gchar *event = json_object_get_string_member(head, "event");
    const gchar *token = json_object_get_string_member(head, "token");
    const gchar *version = json_object_get_string_member(head, "version");

    if (0 == strcmp(type, "request"))
    {
        message = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE, "action", action, "id", id, NULL);
    }
    else if (0 == strcmp(type, "response"))
    {
        message = g_object_new(IPCAM_RESPONSE_MESSAGE_TYPE, "action", action, "id", id, "code", code, NULL);
    }
    else if (0 == strcmp(type, "notice"))
    {
        message = g_object_new(IPCAM_NOTICE_MESSAGE_TYPE, "event", event, NULL);
    }
    if (NULL != message)
    {
        JsonNode *body = json_object_get_member(json_node_get_object(jmsg), "body");
        g_object_set(G_OBJECT(message),
                     "token", token,
                     "version", version,
                     "body", json_node_copy(body),
                     NULL);
    }

    g_object_unref(parser);

    return message;
}

gboolean ipcam_message_is_request(IpcamMessage *message)
{
    g_return_val_if_fail(IPCAM_IS_MESSAGE(message), FALSE);
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(message);
    return (0 == strcmp(priv->type, "request"));
}
gboolean ipcam_message_is_response(IpcamMessage *message)
{
    g_return_val_if_fail(IPCAM_IS_MESSAGE(message), FALSE);
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(message);
    return (0 == strcmp(priv->type, "response"));
}
gboolean ipcam_message_is_notice(IpcamMessage *message)
{
    g_return_val_if_fail(IPCAM_IS_MESSAGE(message), FALSE);
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(message);
    return (0 == strcmp(priv->type, "notice"));
}

const gchar *ipcam_message_to_string(IpcamMessage *message)
{
    g_return_val_if_fail(IPCAM_IS_MESSAGE(message), NULL);

    JsonBuilder *builder = json_builder_new();
    assert(builder);
    JsonGenerator *generator = json_generator_new();
    assert(generator);

    gchar *strval = NULL;
    IpcamMessagePrivate *priv = ipcam_message_get_instance_private(message);

    json_builder_begin_object(builder);     /* begin root */
    json_builder_set_member_name(builder, "head");
    json_builder_begin_object(builder);     /* begin head */
    json_builder_set_member_name(builder, "type");
    json_builder_add_string_value(builder, priv->type);
    json_builder_set_member_name(builder, "token");
    json_builder_add_string_value(builder, priv->token);
    json_builder_set_member_name(builder, "version");
    json_builder_add_string_value(builder, priv->version);
    
    if (ipcam_message_is_notice(message))
    {
        g_object_get(G_OBJECT(message), "event", &strval, NULL);
        json_builder_set_member_name(builder, "event");
        json_builder_add_string_value(builder, strval);
        g_free(strval);
    }
    else if (ipcam_message_is_request(message))
    {
        g_object_get(G_OBJECT(message), "action", &strval, NULL);
        json_builder_set_member_name(builder, "action");
        json_builder_add_string_value(builder, strval);
        g_free(strval);
        g_object_get(G_OBJECT(message), "id", &strval, NULL);
        json_builder_set_member_name(builder, "id");
        json_builder_add_string_value(builder, strval);
        g_free(strval);
    }
    else if (ipcam_message_is_response(message))
    {
        g_object_get(G_OBJECT(message), "action", &strval, NULL);
        json_builder_set_member_name(builder, "action");
        json_builder_add_string_value(builder, strval);
        g_free(strval);
        g_object_get(G_OBJECT(message), "id", &strval, NULL);
        json_builder_set_member_name(builder, "id");
        json_builder_add_string_value(builder, strval);
        g_free(strval);
        g_object_get(G_OBJECT(message), "code", &strval, NULL);
        json_builder_set_member_name(builder, "code");
        json_builder_add_string_value(builder, strval);
        g_free(strval);
    }
    else
    {
        g_warning ("Class '%s' does not have a valid message type",
                   G_OBJECT_TYPE_NAME(message));
    }
    json_builder_end_object(builder);               /* end head */
    json_builder_set_member_name(builder, "body");  /* begin body */
    if (priv->body)
    {
        json_builder_add_value(builder, json_node_copy(priv->body));
    }
    else
    {
        json_builder_add_null_value(builder);
    }
    json_builder_end_object(builder);               /* end root */

    JsonNode *msg_root = json_builder_get_root(builder);
    json_generator_set_root(generator, msg_root);
    json_generator_set_pretty(generator, TRUE);
    
    const gchar *string = json_generator_to_data(generator, NULL);
    json_node_free(msg_root);
    g_object_unref(generator);
    g_object_unref(builder);
    
    return string;
}
