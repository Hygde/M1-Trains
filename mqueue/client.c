#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>

// compiler avec  $ gcc -Wall mqueue_exemple.c -lrt

#define QUEUE_NAME  "/test_queue"
#define MAX_SIZE    1024
#define MSG_STOP    "exit"

int main() {

    mqd_t mq;
    struct mq_attr attr;
    // char buffer[MAX_SIZE+1];
    char message[MAX_SIZE+1] = "Testy";

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* open/create the mail queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    assert((mqd_t)-1 != mq);

    // memset(message, 0, MAX_SIZE);
    // fgets(message, MAX_SIZE, stdin);
    // TODO : fix : the message has to be send 2 times ???
    assert(0 <= mq_send(mq, message, MAX_SIZE, 0));
    assert(0 <= mq_send(mq, message, MAX_SIZE, 0));

    /* cleanup */
    assert(mq_close(mq) != (mqd_t)-1);

    return 0;
}
