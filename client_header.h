#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024
#define PORT 8080

void parse_join(char* join_check, const char* buffer);
void* read_thr(void* arg);
void* write_thr(void* arg);

#endif
