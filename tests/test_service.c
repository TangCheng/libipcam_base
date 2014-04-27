#include "service.h"

int main(int argc, char* argv[])
{
    IpcamService *service = g_object_new(IPCAM_SERVICE_TYPE, "service-name", "test-service", NULL);
    ipcam_service_bind_by_name(service, "test", "tcp://127.0.0.1:8000");
    ipcam_service_connect_by_name(service, "test1", "tcp://127.0.0.1:8000", "test1");
    ipcam_base_service_start(IPCAM_BASE_SERVICE(service));
    return 0;
}

