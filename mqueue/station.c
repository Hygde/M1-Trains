#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>

// compiler avec  $ gcc -Wall mqueue.c -lrt

// #define QUEUE_NAME  "/test_queue"
#define MAX_SIZE    1024
#define MSG_STOP    "exit"
#define A           "/station_a"
#define B           "/station_b"
#define C           "/station_c"
#define D           "/station_d"
#define E           "/station_e"

void usage(char* n) {
    fprintf(stderr, "usage : %s [A|B|C|D|E]\n", n);
    exit(1);
}

int main(int argc, char* argv[]) {
    char station[10];

    if (argc < 2) {
        usage(argv[0]);
    } else {
        if (!strcmp(argv[1], "A")) {
            printf("AFFICHAGE GARE A\n");fflush(stdout);
            strcpy(station, A);
        }
        else if (!strcmp(argv[1], "B")) {
            printf("AFFICHAGE GARE B\n");fflush(stdout);
            strcpy(station, B);
        }
        else if (!strcmp(argv[1], "C")) {
            printf("AFFICHAGE GARE C\n");fflush(stdout);
            strcpy(station, C);
        }
        else if (!strcmp(argv[1], "D")) {
            printf("AFFICHAGE GARE D\n");fflush(stdout);
            strcpy(station, D);
        }
        else if (!strcmp(argv[1], "E")) {
            printf("AFFICHAGE GARE E\n");fflush(stdout);
            strcpy(station, E);
        }
        else
            usage(argv[0]);
    }

    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE+1];

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* open/create the mail queue */
    mq = mq_open(station, O_CREAT | O_RDONLY, 0644, &attr);
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
    assert(mq_unlink(station) != (mqd_t)-1);

    return 0;
}
