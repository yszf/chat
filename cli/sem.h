#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/sem.h>


union semun
{
	int val;
};

void sem_init(int n,int a[]);

void sem_p(int index);

void sem_v(int index);

void sem_destroy();
