#ifndef __SERVICE_H__
#define __SERVICE_H__
#include <glib-object.h>

#define SERVICE_TYPE (service_get_type())
#define SERVICE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SERVICE_TYPE, Service))

typedef struct _Service Service;
typedef struct _ServiceClass ServiceClass;

struct _Service {
GObject parent;
//
gchar *name;
};

struct _ServiceClass {
GObjectClass parent_class;
//
};

GType service_get_type(void);
Service*  service_new(void);
char* service_get_name(Service *service);
void	service_set_name(Service *service, char *name);
Service*  Service_new_with_name(gchar *name);
void  Service_info(Service *service);
#endif /* __SERVICE_H__*/