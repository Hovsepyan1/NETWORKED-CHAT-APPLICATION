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
#include <stdbool.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define NAME_SIZE 50
#define CLIENT_COUNT 100

extern int info;
extern int history;
extern int client_count;

typedef struct 
{
    char name[NAME_SIZE];
    char ip[INET_ADDRSTRLEN];
    int fd;
} USER;

extern USER users[CLIENT_COUNT];
extern pthread_t thread_clients[CLIENT_COUNT];
extern pthread_mutex_t mutex;
extern pthread_mutex_t mutex_history;

char* parse_receive(char* buffer);

int write_in_info_file(USER usr,int fd);
int write_in_history_file(char* buffer,int fd);
int check_if_already_connected(USER user, int fd);
int send_to_clients(int fd);
int parse_username_and_send(char* buffer, char* username, char* message, char *name);
int tag_parse(char* buffer, char* new_buffer, char *source_name);

void who_is_connected(char* buffer, USER* user);
void broadcast_message(char* buffer, int fd);
void broadcast_join(char* name, int fd);
void* handle_client(void * arg);

#endif
