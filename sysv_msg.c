#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define MSGLEN 11

typedef struct {
	long mtype;     /* тип сообщения */
	char mchar[MSGLEN];  /* данные */
} m_msgbuf;

/* длинна сообщения sizeof(Message) - sizeof(long) */

int main()
{
	int msgid;
	int msglen;
	struct msqid_ds info;
	m_msgbuf message;
	key_t semkey;

	/* получение уникального ключа для очереди сообщений */
	if ((semkey = ftok("/tmp", 'a')) == (key_t) -1) 
    	printf("%s\n", strerror(errno)); 
    
    printf("semkey = %d\n", semkey); 
	
	/* создание очереди */
	msgid = msgget(semkey, 0666 | IPC_CREAT);
	if (msgid == -1)
		printf("msgget error: %s\n", strerror(errno));

	message.mtype = 1;
	strcpy(message.mchar, "Hello world");
	msglen = sizeof(message) - sizeof(long);

	/* отправка сообщения в очередь */
	if (msgsnd(msgid, &message, msglen, 0) == -1)
		printf("msgsnd error: %s\n", strerror(errno));

	if (msgctl(msgid, IPC_STAT, &info) == -1)
		printf("msgctl error: %s\n", strerror(errno));

	printf("cbytes = %lu, qnum = %lu, qbytes = %lu\n", 
			(unsigned long) info.msg_cbytes,   /* cbytes - размер очереди в байтах */
			(unsigned long) info.msg_qnum,     /* qnum - количество сообщений в очереди */
			(unsigned long) info.msg_qbytes);  /* qbytes - максимальный размер очереди в байтах */

	system("ipcs -q");

	//msgctl(msgid, IPC_RMID, NULL);
	exit(0);
}