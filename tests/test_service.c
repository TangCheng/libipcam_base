#include "service.h"

int main(int argc, char* argv[])
{
    IpcamService *service = g_object_new(IPCAM_SERVICE_TYPE, "service-name", "test-service", NULL);
    ipcam_base_service_start(IPCAM_BASE_SERVICE(service));
    return 0;
}

