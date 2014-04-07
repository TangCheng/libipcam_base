#include "service.h"

static void service_init(Service *service);
static void service_class_init(ServiceClass *serviceclass);

GType service_get_type(void)
{
	static GType service_type = 0;
	if (!service_type)
	{
		static const GTypeInfo service_info = {
			sizeof(ServiceClass),
			NULL, NULL,
			(GClassInitFunc)service_class_init,
			NULL,NULL,
			sizeof(Service),
			0,
			(GInstanceInitFunc)service_init
		};
		service_type = g_type_register_static(G_TYPE_OBJECT, "Service", &service_info, 0);
	}
	return service_type;
}

static void service_init(Service *service)
{
	service->name = "none";
}
static void service_class_init(ServiceClass *serviceclass)
{
	return;
}
Service *service_new(void)
{
	Service *service;
	service = g_object_new(SERVICE_TYPE, NULL);
	return service;
}
char *service_get_name(Service *service)
{
	return service->name;
}
void service_set_name(Service *service, char *name)
{
	service->name = name;
}
Service*  service_new_with_name(gchar *name)
{
	Service* service;
	service = service_new();
	service_set_name(service, name);
	return service;
}
void  service_info(Service *service)
{
	g_print("The Service name is %s\n", service->name);
}