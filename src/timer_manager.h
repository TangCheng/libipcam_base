#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#define IPCAM_TIMER_MANAGER_TYPE (ipcam_timer_manager_get_type())
#define IPCAM_TIMER_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_TIMER_MANAGER_TYPE, IpcamTimerManager))
#define IPCAM_TIMER_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_TIMER_MANAGER_TYPE, IpcamTimerManagerClass))
#define IPCAM_IS_TIMER_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_TIMER_MANAGER_TYPE))
#define IPCAM_IS_TIMER_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_TIMER_MANAGER_TYPE))
#define IPCAM_TIMER_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_TIMER_MANAGER_TYPE, IpcamTimerManagerClass))

typedef struct _IpcamTimerManager IpcamTimerManager;
typedef struct _IpcamTimerManagerClass IpcamTimerManagerClass;

typedef void (*TCFunc)(GObject *obj);

struct _IpcamTimerManager
{
    GObject parent;
};

struct _IpcamTimerManagerClass
{
    GObjectClass parent_class;
};

GType ipcam_timer_manager_get_type(void);
gboolean ipcam_timer_manager_add_timer(IpcamTimerManager *timer_manager,
                                       const gchar *timer_id,
                                       GObject *object,
                                       TCFunc callback);
void ipcam_timer_manager_del_timer(IpcamTimerManager *timer_manager, const gchar *timer_id);
void ipcam_timer_manager_trig_timer(IpcamTimerManager *timer_manager, const gchar *timer_id);

#endif /* __TIMER_MANAGER_H__ */
