#ifndef __BASE_APP_H__
#define __BASE_APP_H__

#include "service.h"
#include "message.h"
#include "message_manager.h"
#include "timer_manager.h"

#define IPCAM_BASE_APP_TYPE (ipcam_base_app_get_type())
#define IPCAM_BASE_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_BASE_APP_TYPE, IpcamBaseApp))
#define IPCAM_BASE_APP_CLASS(klass) (GTYPE_CHECK_CLASS_CAST((klass), IPCAM_BASE_APP_TYPE, IpcamBaseAppClass))
#define IPCAM_IS_BASE_APP(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_BASE_APP_TYPE))
#define IPCAM_IS_BASE_APP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_BASE_APP_TYPE))
#define IPCAM_BASE_APP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_BASE_APP_TYPE, IpcamBaseAppClass))

typedef struct _IpcamBaseApp IpcamBaseApp;
typedef struct _IpcamBaseAppClass IpcamBaseAppClass;

struct _IpcamBaseApp {
    IpcamService parent;
    //
};

struct _IpcamBaseAppClass {
    IpcamServiceClass parent_class;
    //
};

GType ipcam_base_app_get_type(void);
void ipcam_base_app_add_timer(IpcamBaseApp *base_app,
                              const gchar *timer_id,
                              const gchar *interval,
                              TCFunc callback);
void ipcam_base_app_register_request_handler(IpcamBaseApp *base_app,
                                             const gchar *handler_name,
                                             GType handler_class_type);
void ipcam_base_app_register_notice_handler(IpcamBaseApp *base_app,
                                            const gchar *handler_name,
                                            GType handler_class_type);
void ipcam_base_app_send_message(IpcamBaseApp *base_app,
                                 IpcamMessage *msg,
                                 const gchar *name,
                                 const gchar *client_id,
                                 MsgHandler callback,
                                 guint timeout);
gboolean ipcam_base_app_wait_response(IpcamBaseApp *base_app,
                                      const char *msg_id,
                                      gint64 timeout_ms,
                                      IpcamMessage **response);
const gchar *ipcam_base_app_get_config(IpcamBaseApp *base_app,
                                       const gchar *config_name);
GHashTable *ipcam_base_app_get_configs(IpcamBaseApp *base_app,
                                       const gchar *config_name);
#endif /* __BASE_APP_H__*/
