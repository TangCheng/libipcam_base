#include "socket_manager.h"
#include <zmq.h>
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
        IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(IPCAM_SOCKET_MANAGER(self));
        g_hash_table_destroy(priv->socket_hash);
    }
}
static void ipcam_socket_manager_finalize(GObject *self)
{
    G_OBJECT_CLASS(ipcam_socket_manager_parent_class)->finalize(self);
}
static void destroy_notify(gpointer data)
{
    IpcamSocketManagerHashValue *value = (IpcamSocketManagerHashValue *)data;
    g_free(value->name);
    free(value);
}
static void ipcam_socket_manager_init(IpcamSocketManager *self)
{
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(self);
    priv->socket_hash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (GDestroyNotify)destroy_notify);
    assert(priv->socket_hash);
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
    IpcamSocketManagerHashValue *value = (IpcamSocketManagerHashValue *)malloc(sizeof(IpcamSocketManagerHashValue));
    g_return_val_if_fail(value, FALSE);
    value->name = g_strdup(name);
    value->mq_socket = (void *)mq_socket;
    value->type = type;
    g_hash_table_insert(priv->socket_hash, (gpointer)name, (gpointer)value);
    return TRUE;
}
static gboolean finder(gpointer key, gpointer value, gpointer user_data)
{
    gboolean ret = FALSE;
    IpcamSocketManagerHashValue *hash_value = (IpcamSocketManagerHashValue *)value;
    if (hash_value->mq_socket == (void *)user_data)
    {
        ret = TRUE;
    }
    return ret;
}
gboolean ipcam_socket_manager_delete_by_socket(IpcamSocketManager *socket_manager, const void *mq_socket)
{
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);
    return 0 != g_hash_table_foreach_remove(priv->socket_hash, (GHRFunc)finder, (gpointer)mq_socket);
}
gboolean ipcam_socket_manager_delete_by_name(IpcamSocketManager *socket_manager, const gchar *name)
{
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);
    return g_hash_table_remove(priv->socket_hash, name);
}
gboolean ipcam_socket_manager_has_name(IpcamSocketManager *socket_manager, const gchar *name)
{
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);
    return g_hash_table_contains(priv->socket_hash, name);
}
gboolean ipcam_socket_manager_has_socket(IpcamSocketManager *socket_manager, const void *mq_socket)
{
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);
    return NULL != g_hash_table_find(priv->socket_hash, (GHRFunc)finder, (gpointer)mq_socket);   
}
gboolean ipcam_socket_manager_get_by_name(IpcamSocketManager *socket_manager,
                                          const gchar *name,
                                          int *type,
                                          void **mq_socket)
{
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    gboolean ret = FALSE;
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);
    IpcamSocketManagerHashValue *value =
        (IpcamSocketManagerHashValue *)g_hash_table_lookup(priv->socket_hash, name);
    if (NULL != value)
    {
        *type = value->type;
        *mq_socket = value->mq_socket;
        ret = TRUE;
    }
    return ret;
}
gboolean ipcam_socket_manager_get_by_socket(IpcamSocketManager *socket_manager,
                                            const void *mq_socket,
                                            gchar *name,
                                            int *type)
{
    g_return_val_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager), FALSE);
    gboolean ret = FALSE;
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);
    IpcamSocketManagerHashValue *value =
        g_hash_table_find(priv->socket_hash, (GHRFunc)finder, (gpointer)mq_socket);
    if (NULL != value)
    {
        *type = value->type;
        name = g_strdup(value->name);
        ret = TRUE;
    }
    return ret;
}
static void iterator(gpointer key, gpointer value, gpointer user_data)
{
    IpcamSocketManagerHashValue *item = (IpcamSocketManagerHashValue *)value;
    zmq_close(item->mq_socket);
}
void ipcam_socket_manager_close_all_socket(IpcamSocketManager *socket_manager)
{
    g_return_if_fail(IPCAM_IS_SOCKET_MANAGER(socket_manager));
    IpcamSocketManagerPrivate *priv = ipcam_socket_manager_get_instance_private(socket_manager);
    g_hash_table_foreach(priv->socket_hash, (GHFunc)iterator, NULL);
    g_hash_table_remove_all(priv->socket_hash);
}
