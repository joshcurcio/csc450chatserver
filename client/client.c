#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

void test (){
    puts("client connected SIGNALLL");
}
void processChat(int sockfd, char* message)
{
    char* command;
    char* who;
    char* msg;

    scanf("%ms", &command);
    if(strcmp("tell", command) == 0)
    {
        scanf("%ms %m[^\n]", &who, &msg);
        printf("Sending private message (%s) to: %s\n", msg, who);
        send(sockfd, message , strlen(message) , 0);
    }
    else
    {
        scanf("%m[^\n]", &msg);
        char* destination = malloc(sizeof(command) + sizeof(msg));
        strcpy(destination, command);
        strcat(destination, msg);
        printf("Broadcasting: %s\n", destination);
        send(sockfd, message , strlen(message) , 0);
    }
    //puts(command);
    //puts(msg);
}

int main(int argc, char** argv)
{
    uint16_t port = 4000;
    int error;
    struct sockaddr_in* server = malloc(sizeof(struct sockaddr_in));
    server->sin_addr.s_addr = inet_addr("127.0.0.1");
    server->sin_family = AF_INET;
    server->sin_port = htons( port );
    //memset(server->sin_zero, '\0', sizeof server->sin_zero);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    error = connect(sockfd, (struct sockaddr *)server, sizeof(struct sockaddr));
    if(error < 0)
    {
        puts("Problem with connecting...\n");
    }
    else
    {
        puts("We are Connected");
        //Receive a reply from the server
        int MAX_SIZE = 2000 * sizeof(char);
        char* server_reply = malloc(MAX_SIZE);
        while(1)
        {
            
            pthread_signal(SIGCONT, test);
            error = recv(sockfd, server_reply, MAX_SIZE, 0);
            if(error < 0)
            {
                puts("recv failed");
            }
            else
            {
                puts("Reply received\n");
                puts(server_reply);
                
            }
            
            char* message = "i fork children";
            processChat(sockfd, message);
        }
    }
    free(server);
    return 0;
}