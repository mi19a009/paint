/*
Copyright 2025 Taichi Murakami.
Main module.
*/

#include "stdafx.h"
#define RESOURCEFORMATSTRING    "%s%s"

int format_resource_name (char *buffer, size_t length, const char *name)
{
	return snprintf(buffer, length, RESOURCEFORMATSTRING, paint_application_prefix, name);
}

int main(int argc, char **argv)
{
	GApplication *application;
	int result;
	application = paint_application_new();

	if (application)
	{
		result = g_application_run(application, argc, argv);
		g_object_unref(application);
	}
	else
	{
		result = -1;
	}

	return result;
}
