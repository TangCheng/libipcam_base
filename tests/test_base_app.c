#include "app.h"

int main(int argc, char* argv[])
{
    while (TRUE)
    {
        IpcamApp *app = g_object_new(IPCAM_APP_TYPE, NULL);
        g_object_unref(app);
    }
    
    return 0;
}
