#include "client_header.h"

void parse_join(char* join_check, const char* buffer)
{
    int i = 0;
    while(buffer[i] != ' ' && buffer[i] != '\0' && i < 5)
    {
        join_check[i] = buffer[i];
        i++;
    }
    join_check[i] = '\0';
}

void* read_thr(void* arg)
{
    char buffer[BUFFER_SIZE];
    int fd = *(int*)arg;
    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0)
        {
            printf("Server disconnected or error occurred\n");
            _exit(0);
        }
        puts(buffer);
    }
    return NULL;
}

void* write_thr(void* arg)
{
    char buffer[BUFFER_SIZE];
    int fd = *(int*)arg;
    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);
        send(fd, buffer, strlen(buffer), 0);
    }
    return NULL;
}
