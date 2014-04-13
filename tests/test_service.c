#include "service.h"

int main(int argc, char* argv[])
{
    IpcamService *service = g_object_new(IPCAM_SERVICE_TYPE, "service-name", "encoder-service", NULL);
    
    return 0;
}

