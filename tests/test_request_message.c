#include "request_message.h"

int main(int argc, char* argv[])
{
    IpcamRequestMessage *request_message = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE, NULL);
    IpcamMessage *message = ipcam_request_message_get_response_message(request_message, "-1");

    request_message = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE, NULL);
    return 0;
}
