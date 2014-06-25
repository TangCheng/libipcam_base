#include <json-glib/json-glib.h>
#include "test_event_handler.h"
#include "messages.h"
#include "app1.h"

G_DEFINE_TYPE(IpcamTestEventHandler, ipcam_test_event_handler, IPCAM_EVENT_HANDLER_TYPE);

static void ipcam_test_event_handler_run_impl(IpcamTestEventHandler *test_event_handler,
                                               IpcamMessage *message);

static void ipcam_test_event_handler_init(IpcamTestEventHandler *self)
{
}
static void ipcam_test_event_handler_class_init(IpcamTestEventHandlerClass *klass)
{
    IpcamEventHandlerClass *event_handler_class = IPCAM_EVENT_HANDLER_CLASS(klass);
    event_handler_class->run = &ipcam_test_event_handler_run_impl;
}
static void ipcam_test_event_handler_run_impl(IpcamTestEventHandler *test_event_handler,
                                               IpcamMessage *message)
{
    gpointer service;
    g_object_get(G_OBJECT(test_event_handler), "service", &service, NULL);
    JsonNode *body;
    JsonGenerator *generator = json_generator_new();

    g_object_get(G_OBJECT(message), "body", &body, NULL);

    json_generator_set_root(generator, body);
    json_generator_set_pretty (generator, TRUE);
    gchar *str = json_generator_to_data (generator, NULL);
    
    g_printf("receive notice, body=\n%s\n", str);

    g_free(str);
    g_object_unref(generator);
}
