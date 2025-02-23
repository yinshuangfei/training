#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

#include "comm.h"

/*
 共享内存函数由shmget、shmat、shmdt、shmctl四个函数组成
 * shmdt(断开共享内存连接)
 * shmat(把共享内存区对象映射到调用进程的地址空间)
 * shmget(得到一个共享内存标识符或创建一个共享内存对象)
 * shmctl(共享内存管理)
*/

int main(int argc, char *argv[]) 
{
	key_t myftok = ftok(tmp_path, project_id);
	if (myftok == -1) {
		printf("file key error (%d), exit.\n", myftok);
	}

	int shmid = shmget(myftok, sizeof(person), IPC_CREAT | 0777);
	printf("shamid :%d\n", shmid);

	/* get shm address */
	person *feifei;
	feifei = (person *)shmat(shmid, NULL, SHM_W);
	printf("raw from share mem ==> age:%d, name:%s\n", feifei->age, feifei->name);

	/* set shm value */
	feifei->age = 29;
	strcpy(feifei->name, "good boy");

	return 0;
}


