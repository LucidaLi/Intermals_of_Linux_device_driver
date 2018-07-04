#include <stdio.h>
#include <stdlib.h>

struct subent{
	int a;
	int *p;
};

struct kobject{
	int a;
	int b;
	struct subent *sd;
};

void npfs_remove_dir(struct kobject *kobj)
{
	struct subent *sd = kobj->sd;
	kobj->sd = NULL;
}
