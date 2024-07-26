#include "client_header.h"

int main(int argc, const char* argv[])
{
    char buffer[BUFFER_SIZE];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server;
    socklen_t size = sizeof(server);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    printf("---------------------------WELCOME TO CHAT---------------------------\n");
    printf("JOIN <username>: Register the client with their username and receive chat history.\n"
           "QUIT: Disconnect\n");

    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    if(strcmp(buffer, "QUIT") == 0)
    {
        printf("Byee!\n");
        _exit(0);
    }

    while (1)
    {
        char join_check[6] = {0};
        parse_join(join_check, buffer);
        if (strcmp(join_check, "JOIN") == 0)
        {
            break;
        }
        else
        {
            printf("INVALID INPUT\n");
        }
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
    }

    if (connect(sockfd, (struct sockaddr*)&server, size) < 0)
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    send(sockfd, buffer, BUFFER_SIZE, 0);

    pthread_t read_t, write_t;
    pthread_create(&read_t, NULL, read_thr, &sockfd);
    pthread_detach(read_t);
    pthread_create(&write_t, NULL, write_thr, &sockfd);
    pthread_detach(write_t);

    while (1); // Keep the main thread running

    close(sockfd);
    return 0;
}
