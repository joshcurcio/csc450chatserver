#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>

void broadcast(char* msg, int* clients, int numClients);

int main(int argc, char** argv)
{
    int error;
    char* message = "";
    int listenfd;
    socklen_t serverSize;
    int clientfd;
    int MAX_CLIENTS = 1000;
    int* clients = malloc(MAX_CLIENTS * sizeof(int));
    int numberOfConnectedClients = 0;
    int sockfd;
    struct sockaddr_in* server;
   
   
    int shmid;
    int shmid2;
    key_t key;
    key_t key2;
    int *shm;
    int *shm2;
    key = 123456;
    if ((shmid = shmget(key, 100*sizeof(int), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return -1;
    }
    if ((shm = shmat(shmid, NULL, 0)) == (int *) -1) {
        perror("shmat");
        return -1;
    }
    
    key2 = 654321;
    if ((shmid2 = shmget(key2, 100*sizeof(int), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return -1;
    }
    if ((shm2 = shmat(shmid2, NULL, 0)) == (int *) -1) {
        perror("shmat");
        return -1;
    }
        
        
        
    uint16_t port = 3000;
    server = malloc(sizeof(struct sockaddr_in));
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        puts("Bad Socket!!!!");
    }
    
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = inet_addr("127.0.0.1");
    server->sin_port = htons( port );
    memset(server->sin_zero, '\0', sizeof server->sin_zero);

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    serverSize = sizeof(server);
    
    int bindfd = bind(sockfd, (struct sockaddr *)server, sizeof(struct sockaddr));
    if(bindfd < 0)
    {
        puts("Problem with binding...\n");
    }
    
    
    message = "Welcome to the Server";
    
    
    while(1)
    {
        listenfd = listen(sockfd, 100);
    
        printf("Listening....\n");
        clientfd = accept(sockfd ,  (struct sockaddr *)server , &serverSize);
        printf("Client Connected.... %d\n", clientfd);
        clients[numberOfConnectedClients++] = clientfd;
        shm2[0] = numberOfConnectedClients;
        shm = clients;
        int pid = fork();
        if(pid == 0) {
        printf("Child Loop Start");
        send(clientfd, message , strlen(message) , 0);
        
        while(1)
        {
            char* client_reply = malloc(12000 * sizeof(char)); 
            error = recv(clientfd, client_reply, sizeof(client_reply), 0);
            if(error < 0)
            {
                puts("recv failed");
                break;
            }
            else
            {
                puts("Reply received:");
                puts(client_reply);
               broadcast(client_reply, shm, shm2[0]);
            }
        }
        break;
        }
    }
    //write(clientfd, buffer, sizeof(buffer));
}

void broadcast(char* msg, int* clients, int numClients)
{
    int i;
    for(i = 0; i < numClients; i++)
    {
        printf("Trying to send to client with FD: %d\n", clients[i]);
        //printf("Trying to send to client with FD: %d\n", *(clients + (i * sizeof(int))));
        
        send(clients[i] , msg , strlen(msg) , 0);
        puts("sent");
    }
}