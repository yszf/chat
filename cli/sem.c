#include "sem.h"

static int semid = -1;
void sem_init(int n,int a[])
{
	assert(n >= 1 && a != NULL);
	semid = semget((key_t)1234,n,IPC_CREAT | IPC_EXCL | 0600);
	if(semid == -1)
	{
		semid = semget((key_t)1234,n,IPC_CREAT | 0600);
		if(semid == -1)
		{
			perror("sem_get error");
		}

		union semun b;
		int i = 0;
		for(;i<n;++i)
		{
			b.val = a[i];
			if(semctl(semid,i,SETVAL,b)== -1)
			{
				perror("semctl error");
			}
		}
	}
}

void sem_p(int index)
{
	struct sembuf buf;
	buf.sem_num = index;
	buf.sem_op = -1;
	buf.sem_flg = SEM_UNDO;
	if(semop(semid,&buf,1)== -1)
	{
		perror("semop p error");
	}
}

void sem_v(int index)
{
	struct sembuf buf;
	buf.sem_num = index;
	buf.sem_op = 1;
	buf.sem_flg = SEM_UNDO;
	if(semop(semid,&buf,1)==-1)
	{
		perror("semop v error");
	}
}

void sem_destroy()
{
	if(semctl(semid,1,IPC_RMID)==-1)
	{
		perror("semctl rm error");
	}
}
