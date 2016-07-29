#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXMSG (8192 + sizeof(long))

typedef struct {
	long mtype;          /* тип сообщения */
	char mchar[MAXMSG];  /* данные */
} m_msgbuf;

typedef struct {
	long mtype;     /* тип сообщения */
	char mchar[2];  /* данные */
} m_msg;

/* длинна сообщения sizeof(Message) - sizeof(long) */

int main()
{
	int msglen;
	struct msqid_ds info;
	m_msgbuf *buff;

	m_msg mmsg; 
	ssize_t n_bytes;
	int mqid;
	key_t semkey;

	/* получение уникального ключа для очереди сообщений */
	if ((semkey = ftok("/tmp", 'a')) == (key_t) -1) 
    	printf("%s\n", strerror(errno)); 

    printf("semkey = %d\n", semkey); 

    mqid = msgget(semkey, 0666 | IPC_CREAT);
	if (mqid == -1)
		printf("%s\n", strerror(errno));

	buff = malloc(MAXMSG);

	/* считывание сообщения из очереди */
	n_bytes = msgrcv(mqid, buff, MAXMSG, 0, 0);
	if (n_bytes == -1)
		printf("%s\n", strerror(errno));
	printf("read %ld bytes, mqid->mchar: %s\n", n_bytes, buff->mchar);

	mmsg.mtype = 1;
	strcpy(mmsg.mchar, "Hi");
	msglen = sizeof(mmsg) - sizeof(long);

	/* отправка сообщения в очередь */
	if ((msgsnd(mqid, &mmsg, msglen, 0)) == -1)
		printf("%s\n", strerror(errno));

	if (msgctl(mqid, IPC_STAT, &info) == -1)
		printf("%s\n", strerror(errno));

	printf("cbytes = %lu, qnum = %lu, qbytes = %lu\n", 
			(unsigned long) info.msg_cbytes,   
			(unsigned long) info.msg_qnum,     
			(unsigned long) info.msg_qbytes); 

	system("ipcs -q");

	/* удаление очереди */
	msgctl(mqid, IPC_RMID, NULL);
	exit(0);
}