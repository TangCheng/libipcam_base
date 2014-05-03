#include "notice_message.h"

int main(int argc, char* argv[])
{
    IpcamNoticeMessage *notice_message = g_object_new(IPCAM_NOTICE_MESSAGE_TYPE, NULL);
    g_print("%s\n", ipcam_message_to_string(IPCAM_MESSAGE(notice_message)));
    return 0;
}
