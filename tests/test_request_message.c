#include "request_message.h"

int main(int argc, char* argv[])
{
    IpcamRequestMessage *request_message = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE, NULL);
    g_object_set(G_OBJECT(request_message), "action", "test", NULL);
    IpcamMessage *message = ipcam_request_message_get_response_message(request_message, "-1");
    g_object_unref(request_message);
    g_object_unref(message);

    request_message = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE, NULL);
    g_object_unref(request_message);
    return 0;
}
