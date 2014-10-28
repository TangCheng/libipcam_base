#ifndef __TIMER_PUMP_H__
#define __TIMER_PUMP_H__

#include "service.h"

#define IPCAM_TIMER_PUMP_ADDRESS "ipc:///tmp/ipcam_timer_pump.socket"

#define IPCAM_TIMER_PUMP_TYPE (ipcam_timer_pump_get_type())
#define IPCAM_TIMER_PUMP(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_TIMER_PUMP_TYPE, IpcamTimerPump))
#define IPCAM_TIMER_PUMP_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_TIMER_PUMP_TYPE, IpcamTimerPumpClass))
#define IPCAM_IS_TIMER_PUMP(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_TIMER_PUMP_TYPE))
#define IPCAM_IS_TIMER_PUMP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_TIMER_PUMP_TYPE))
#define IPCAM_GET_TIMER_PUMP_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_TIMER_PUMP_TYPE, IpcamTimerPumpClass))

typedef struct _IpcamTimerPump IpcamTimerPump;
typedef struct _IpcamTimerPumpClass IpcamTimerPumpClass;

struct _IpcamTimerPump
{
    IpcamBaseService parent;
};

struct _IpcamTimerPumpClass
{
    IpcamBaseServiceClass parent_class;
};

GType ipcam_timer_pump_get_type(void);

#endif /* __TIMER_PUMP_H__ */
