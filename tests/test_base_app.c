#include "app.h"

int main(int argc, char* argv[])
{
    IpcamApp *app = g_object_new(IPCAM_APP_TYPE, NULL);
    ipcam_base_service_start(IPCAM_BASE_SERVICE(app));
    g_object_unref(app);
    
    return 0;
}
