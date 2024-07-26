#include "header.h"

int info;
int history;
int client_count = 0;
USER users[CLIENT_COUNT];
pthread_t thread_clients[CLIENT_COUNT];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_history = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, const char* argv[])
{
    info = open("info.txt", O_RDWR | O_APPEND );
    if(info == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    history = open("history.txt", O_RDWR | O_APPEND);
    if(history == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    char buffer[BUFFER_SIZE];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
   
    struct sockaddr_in server, client;
    socklen_t size = sizeof(server);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    int opt = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) 
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, (struct sockaddr*)&server, size) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, CLIENT_COUNT) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    snprintf(buffer, BUFFER_SIZE, "----------------------------WELCOME TO CHAT----------------------------\n");
    puts(buffer);

    char* req;

    int index = -1;
    while (1)
    {
        memset(&client, 0, sizeof(client));

        int new_fd = accept(sockfd, (struct sockaddr*)&client, &size);
        if (new_fd < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
     
        pthread_mutex_lock(&mutex);
        int index = client_count++;
        users[index].fd = new_fd;
        inet_ntop(AF_INET, &client.sin_addr, users[index].ip, sizeof(users[index].ip));
        pthread_mutex_unlock(&mutex);
     
        users[index].fd = new_fd; 

        if (pthread_create(&thread_clients[index], NULL, handle_client, (void *)&index) < 0)
        {
            printf("Failed to create thread");
            break;
        }

        pthread_detach(thread_clients[index]);

    }
    free(req);
    close(sockfd);
    return 0;
}
