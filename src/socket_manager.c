#include "socket_manager.h"
#include <czmq.h>
#include <assert.h>
#include <stdlib.h>

typedef struct _IpcamSocketManagerHashValue
{
    gchar *name;
    void *mq_socket;
    gint type;
} IpcamSocketManagerHashValue;

typedef struct _IpcamSocketManagerPrivate
{
    GHashTable *socket_hash;
	GMutex mutex;
} IpcamSocketManagerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamSocketManager, ipcam_socket_manager, G_TYPE_OBJECT);

static GObject *ipcam_socket_manager_constructor(GType self_type,
                                                 guint n_properties,
                                                 GObjectConstructParam *properties)
{
    GObject *obj;
    obj = G_OBJECT_CLASS(ipcam_socket_manager_parent_class)->constructor(self_type, n_properties, properties);
    return obj;
}
static void ipcam_socket_manager_dispose(GObject *self)
{
    static gboolean first_run = TRUE;

    if (first_run)
    {
        first_run = FALSE;
        G_OBJECT_CLASS(ipcam_socket_manager_parent_class)->dispose(self);
    }
}
static void ipcam_socket_manager_finalize(GObject *self)
{
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(IPCAM_SOCKET_MANAGER(self));

    g_mutex_lock(&priv->mutex);
    g_hash_table_destroy(priv->socket_hash);
    g_mutex_unlock(&priv->mutex);
    g_mutex_clear(&priv->mutex);

    G_OBJECT_CLASS(ipcam_socket_manager_parent_class)->finalize(self);
}
static void destroy_notify(gpointer data)
{
    IpcamSocketManagerHashValue *value = (IpcamSocketManagerHashValue *)data;
    g_free(value->name);
    g_free(value);
}
static void ipcam_socket_manager_init(IpcamSocketManager *self)
{
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(self);
    priv->socket_hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
                                              (GDestroyNotify)destroy_notify);
    assert(priv->socket_hash);
	g_mutex_init(&priv->mutex);
}
static void ipcam_socket_manager_class_init(IpcamSocketManagerClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    this_class->constructor = &ipcam_socket_manager_constructor;
    this_class->dispose = &ipcam_socket_manager_dispose;
    this_class->finalize = &ipcam_socket_manager_finalize;
}
gboolean ipcam_socket_manager_add(IpcamSocketManager *socket_manager,
                                  const gchar *name,
                                  const int type,
                                  const void *mq_socket)
{
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);
    IpcamSocketManagerHashValue *value = g_new(IpcamSocketManagerHashValue, 1);
    g_return_val_if_fail(value, FALSE);
    value->name = g_strdup(name);
    value->mq_socket = (void *)mq_socket;
    value->type = type;

    g_mutex_lock(&priv->mutex);
    g_hash_table_insert(priv->socket_hash, g_strdup(name), (gpointer)value);
    g_mutex_unlock(&priv->mutex);

    return TRUE;
}
static gboolean finder(gpointer key, gpointer value, gpointer user_data)
{
    IpcamSocketManagerHashValue *hash_value = (IpcamSocketManagerHashValue *)value;
    return (hash_value->mq_socket == (void *)user_data);
}
gboolean ipcam_socket_manager_delete_by_socket(IpcamSocketManager *socket_manager, const void *mq_socket)
{
	int ret;
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);

    g_mutex_lock(&priv->mutex);
    ret = g_hash_table_foreach_remove(priv->socket_hash, (GHRFunc)finder, (gpointer)mq_socket);
    g_mutex_unlock(&priv->mutex);

	return ret;
}
gboolean ipcam_socket_manager_delete_by_name(IpcamSocketManager *socket_manager, const gchar *name)
{
	gboolean ret;
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);

    g_mutex_lock(&priv->mutex);
    ret =  g_hash_table_remove(priv->socket_hash, name);
    g_mutex_unlock(&priv->mutex);

    return ret;
}
gboolean ipcam_socket_manager_has_name(IpcamSocketManager *socket_manager, const gchar *name)
{
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);

    g_mutex_lock(&priv->mutex);
    gboolean ret =  g_hash_table_contains(priv->socket_hash, name);
    g_mutex_unlock(&priv->mutex);

    return ret;
}
gboolean ipcam_socket_manager_has_socket(IpcamSocketManager *socket_manager, const void *mq_socket)
{
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);

    g_mutex_lock(&priv->mutex);
    gpointer ret = g_hash_table_find(priv->socket_hash, (GHRFunc)finder, (gpointer)mq_socket);  
    g_mutex_unlock(&priv->mutex);

    return NULL != ret;
}
gboolean ipcam_socket_manager_get_by_name(IpcamSocketManager *socket_manager,
                                          const gchar *name,
                                          int *type,
                                          void **mq_socket)
{
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    gboolean ret = FALSE;
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);

    g_mutex_lock(&priv->mutex);
    IpcamSocketManagerHashValue *value =
        (IpcamSocketManagerHashValue *)g_hash_table_lookup(priv->socket_hash, name);
    if (NULL != value)
    {
        *type = value->type;
        *mq_socket = value->mq_socket;
        ret = TRUE;
    }
    g_mutex_unlock(&priv->mutex);

    return ret;
}
gboolean ipcam_socket_manager_get_by_socket(IpcamSocketManager *socket_manager,
                                            const void *mq_socket,
                                            gchar **name,
                                            int *type)
{
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    gboolean ret = FALSE;
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);

    g_mutex_lock(&priv->mutex);
    IpcamSocketManagerHashValue *value =
        g_hash_table_find(priv->socket_hash, (GHRFunc)finder, (gpointer)mq_socket);
    if (NULL != value)
    {
        *type = value->type;
        *name = g_strdup(value->name);
        ret = TRUE;
    }
    g_mutex_unlock(&priv->mutex);

    return ret;
}

void ipcam_socket_manager_close_all_socket(IpcamSocketManager *socket_manager)
{
    g_return_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager));
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);

    g_mutex_lock(&priv->mutex);
    g_hash_table_remove_all(priv->socket_hash);
    g_mutex_unlock(&priv->mutex);
}
