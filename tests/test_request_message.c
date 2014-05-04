#include "request_message.h"
#include <assert.h>
#include <string.h>

int main(int argc, char* argv[])
{
    IpcamRequestMessage *request_message = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE, NULL);
    g_object_set(G_OBJECT(request_message), "action", "test", NULL);
    IpcamMessage *message = ipcam_request_message_get_response_message(request_message, "-1");
    gchar *string;
    string = ipcam_message_to_string(IPCAM_MESSAGE(request_message));
    g_print("%s\n", string);
    IpcamMessage *msg = ipcam_message_parse_from_string(string);
    g_free(string);
    string = ipcam_message_to_string(msg);
    g_print("%s\n", string);
    g_free(string);
    g_object_unref(IPCAM_REQUEST_MESSAGE(msg));
    string = ipcam_message_to_string(IPCAM_MESSAGE(message));
    g_print("%s\n", string);
    g_free(string);
    g_object_unref(request_message);
    g_object_unref(message);

    request_message = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE, NULL);
    g_object_unref(request_message);
    return 0;
}
