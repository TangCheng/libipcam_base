#include "timer_pump.h"

int main(int argc, char* argv[])
{
    IpcamTimerPump *timer_pump = g_object_new(IPCAM_TIMER_PUMP_TYPE, "name", "timer-service", NULL);
    ipcam_base_service_start(IPCAM_BASE_SERVICE(timer_pump));
    return 0;
}
