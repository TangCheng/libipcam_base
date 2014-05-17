#ifndef __CONFIG_MANAGER_H__
#define __CONFIG_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#define IPCAM_CONFIG_MANAGER_TYPE (ipcam_config_manager_get_type())
#define IPCAM_CONFIG_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_CONFIG_MANAGER_TYPE, IpcamConfigManager))
#define IPCAM_CONFIG_MANAGER_CLASS(klass) (G_TYEP_CHECK_CLASS_CAST((klass), IPCAM_CONFIG_MANAGER_TYPE, IpcamConfigManagerClass))
#define IPCAM_IS_CONFIG_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_CONFIG_MANAGER_TYPE))
#define IPCAM_IS_CONFIG_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_CONFIG_MANAGER_TYPE))
#define IPCAM_CONFIG_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_CONFIG_MANAGER_TYPE, IpcamConfigManagerClass))

typedef struct _IpcamConfigManager IpcamConfigManager;
typedef struct _IpcamConfigManagerClass IpcamConfigManagerClass;

struct _IpcamConfigManager
{
    GObject parent;
};

struct _IpcamConfigManagerClass
{
    GObjectClass parent_class;
};

GType ipcam_config_manager_get_type(void);
gboolean ipcam_config_manager_load_config(IpcamConfigManager *config_manager, const gchar *file_path);
void ipcam_config_manager_merge(IpcamConfigManager *config_manager, const gchar *conf_name, const gchar *conf_value);
gchar *ipcam_config_manager_get(IpcamConfigManager *config_manager, const gchar *conf_name);
GHashTable *ipcam_config_manager_get_collection(IpcamConfigManager *config_manager, const gchar *conf_name);

#endif /* __CONFIG_MANAGER_H__ */
