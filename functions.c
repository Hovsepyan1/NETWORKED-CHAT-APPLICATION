#include "header.h"

char* parse_receive(char* buffer)
{
    char* check = (char*)malloc(10 * sizeof(char));
    if(check == NULL)
    {
        perror("Memory allocation is failed");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while(buffer[i] != ' ' && buffer[i] != '\0' && buffer[i] != '\n')
    {
        check[i] = buffer[i];
        i++;
    }
    check[i] = '\0';
    return check;
}

void who_is_connected(char* buffer, USER* user)
{
    char* check = (char*)malloc(NAME_SIZE * sizeof(char));
    if(check == NULL)
    {
        perror("Memory allocation is failed");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while(buffer[i] != ' ') i++;
    while(buffer[i] == ' ') i++;
    int k = 0;
    while(buffer[i] != '\0')
    {
        check[k] = buffer[i];
        i++;
        k++;
    }
    check[k] = '\0';
    strcpy(user->name, check);
    user->name[strcspn(user->name, "\n")] = '\0';
    free(check);
}

int write_in_info_file(USER usr,int fd)
{    
    lseek(fd, 0, SEEK_END);

    if( write(fd,&usr,sizeof(usr)) == -1)
    {
        return -1;
    }

    return 0;
}

int check_if_already_connected(USER user, int fd)
{
    pthread_mutex_lock(&mutex);

    lseek(fd, 0, SEEK_SET);
    USER tmpusr;
    while(read(fd,&tmpusr,sizeof(tmpusr)) > 0)
    {
        if(strcmp(user.name, tmpusr.name) == 0)
        {
            pthread_mutex_unlock(&mutex);
            return 1;
        }
    }

    pthread_mutex_unlock(&mutex);

    return 0;
}

void* handle_client(void * arg)
{
    int index = *(int *)arg;
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "MSG <message>: Send a message to be broadcasted, potentially including user tags.\n"
                                  "DIRECT <username> <message>: Start or continue a private conversation with a specified user.\n"
                                  "QUIT: Disconnect from the server\n"
                                  "MSG <@username>: <message> tag one user and send message");
    send(users[index].fd, buffer, strlen(buffer), 0);
    char* req;
    while(1)
    {
        int bytes_received = recv(users[index].fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0)
        {
            perror("recv failed");
            break;
        }
        else if(bytes_received  == 0)
        {
            printf("Connection lost\n");
            break;
        }

        buffer[bytes_received] = '\0';
        req = parse_receive(buffer);
        
        if(strcmp(req, "JOIN") == 0)
        {
            who_is_connected(buffer, &users[index]);
            broadcast_join(users[index].name, users[index].fd);
            if(check_if_already_connected(users[index], info) == 0)
            {
                pthread_mutex_lock(&mutex);
                if(write_in_info_file(users[index], info) == -1)
                {
                    perror("Can't to write into file");
                    exit(EXIT_FAILURE);    
                }
                pthread_mutex_unlock(&mutex);
            }
            send_to_clients(users[index].fd);
        }
        else if(strcmp(req, "MSG") == 0)
        {
            char new_buffer[BUFFER_SIZE];
            if(*(buffer + strlen(req) + 1) == '@')
            {
                int ret = tag_parse(buffer + strlen(req) + 1, new_buffer, users[index].name);
                if (ret == -1) {
                    strcpy(buffer, "INVALID ARGUMENT\n");
                    send(users[index].fd, buffer, strlen(buffer), 0);
                    continue;
                }
                else if(ret == 0)
                {
                    strcpy(buffer, "NO USER WITH USERNAME\n");
                    send(users[index].fd, buffer, strlen(buffer), 0);
                    continue;
                }
            }
            else 
            {
                snprintf(new_buffer, BUFFER_SIZE, "%s: ", users[index].name);
                strcat(new_buffer, buffer + strlen(req) + 1);
            }

            pthread_mutex_lock(&mutex_history);
            if(write_in_history_file(new_buffer, history) == -1)
            {
                perror("Can't to write in history file");
                exit(EXIT_FAILURE);
            }
            pthread_mutex_unlock(&mutex_history);
            broadcast_message(new_buffer, users[index].fd);
        }
        else if(strcmp(req, "DIRECT") == 0)
        {
            char username[NAME_SIZE];
            char message[BUFFER_SIZE];

            int ret = parse_username_and_send(buffer + strlen(req) + 1, username, message, users[index].name);
            if(ret == -1)
            {
                strcpy(buffer, "INVALID ARGUMENT\n");
                send(users[index].fd, buffer, strlen(buffer), 0);
            }
            else if(ret == 0)
            {
                strcpy(buffer, "NO USRE WITH USERNAME\n");
                send(users[index].fd, buffer, strlen(buffer), 0);
            }
        }
        else if(strcmp(req, "QUIT") == 0)
        {
            printf("%s left the chat\n", users[index].name);
            close(users[index].fd);
            pthread_exit(NULL);
        }
    }
}

void broadcast_message(char* buffer, int fd)
{
    for(int i = 0; i < client_count; i++)
    {
        if(users[i].fd == fd)
        {
            continue;
        }
        send(users[i].fd, buffer, strlen(buffer), 0);
    }
}    

int write_in_history_file(char* buffer,int fd)
{    
    lseek(fd, 0, SEEK_END);

    if( write(fd,buffer,strlen(buffer)) == -1)
    {
        return -1;
    }

    return 0;
}

int send_to_clients(int fd)
{
    struct stat fst;
    if (stat("history.txt", &fst) == -1)
    {
        perror("stat");
        return -1;
    }
    long file_size = fst.st_size;
    
    char *buffer = (char *)malloc(file_size);
    if (buffer == NULL)
    {
        perror("malloc");
        return -1;
    }
    
    lseek(history, 0, SEEK_SET);
    if (read(history, buffer, file_size) == -1)
    {
        perror("read");
        free(buffer);
        return -1;
    }

    int newline_count = 0;
    long position = file_size - 1;

    while (position >= 0 && newline_count < 10)
    {
        if (buffer[position] == '\n')
        {
            newline_count++;
        }
        position--;
    }
    
    long start_pos = (newline_count == 10) ? position + 2 : 0;

    char header[BUFFER_SIZE];
    snprintf(header, BUFFER_SIZE, "\nHISTORY (recent 10 messages)\n");
    send(fd, header, strlen(header), 0);

    send(fd, buffer + start_pos, file_size - start_pos, 0);

    free(buffer);
    return 0;
}

int parse_username_and_send(char* buffer, char* username, char* message, char *name)
{
    if(sscanf(buffer, "%s %1023[^\n]", username, message) != 2)
    {
        return -1;
    }

    char new_buffer[BUFFER_SIZE];
    snprintf(new_buffer, BUFFER_SIZE, "Private message from %s: %s\n", name, message);

    int founded = 0;
    for(int i = 0; i < client_count; i++)
    {
        if(strcmp(users[i].name, username) == 0)
        {
            send(users[i].fd, new_buffer, strlen(new_buffer) + 1, 0);
            founded = 1; 
        }
    }

    if(founded == 0) return 0;

    return 1;
} 

int tag_parse(char* buffer, char* new_buffer, char *source_name)
{
    char dest_name[NAME_SIZE];
    char message[BUFFER_SIZE - 100];
    if(sscanf(buffer, "@%49[^:]: %1023[^\n]", dest_name, message) != 2) {
        return -1;
    }

    int founded = 0;
    for(int i = 0; i < client_count; i++)
    {
        if(strcmp(users[i].name, dest_name) == 0)
        {
            founded = 1;
            break;
        }
    }
    if(founded == 0)
    {
        return 0;
    }
    snprintf(new_buffer, BUFFER_SIZE, "%s to %s: %s\n", source_name, dest_name, message);

    return 1;
}

void broadcast_join(char* name, int fd)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s joined to chat\n", name);
    puts(buffer);
    for(int i = 0; i < client_count; i++)
    {
        if(users[i].fd == fd)
        {
            continue;
        }
        send(users[i].fd, buffer, strlen(buffer), 0);
    }
}
