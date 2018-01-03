#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>

// compiler avec  $ gcc -Wall mqueue.c -lrt

#define QUEUE_NAME  "/test_queue"
#define MAX_SIZE    1024
#define MSG_STOP    "exit"


int main(int argc, char* argv[]) {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE+1];

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* open/create the mail queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    assert((mqd_t)-1 != mq);
    int must_stop = 0;
    do {
        mq_receive(mq, buffer, MAX_SIZE, NULL);
        ssize_t bytes_read;
        /* receive the message */
        bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
        assert(bytes_read >= 0);

        buffer[bytes_read] = '\0';
        if (!strncmp(buffer, MSG_STOP, strlen(MSG_STOP))) {
            must_stop = 1;
        } else {
            printf("%s\n", buffer);
            fflush(stdout);
            sleep(1);
        }
    } while (!must_stop);

    /* cleanup */
    assert(mq_close(mq) != (mqd_t)-1);
    assert(mq_unlink(QUEUE_NAME) != (mqd_t)-1);

    return 0;
}
