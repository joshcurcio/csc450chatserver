#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>

void setupServerSocket();
void broadcast(char* msg, int* clients, int numClients);
void sendMessage(char* msg, int client);
void childStuff(int clientfd);
void receiveMessage(int clientfd);

struct sockaddr_in* server;
uint16_t port = 3000;
int sockfd;
socklen_t serverSize;

int main(int argc, char** argv)
{
    server = malloc(sizeof(struct sockaddr_in));
    int clientfd;
    int MAX_CLIENTS = 1000;
    int* clients = malloc(MAX_CLIENTS * sizeof(int));
    int numberOfConnectedClients = 0;
    sem_t block;
    sem_init(&block, 0, 1);
    
    char* message = "hello";
    int pid;
    //setup the socket server
    setupServerSocket();
    
   while(1)
   {
    printf("Listening....\n");
    clientfd = accept(sockfd ,  (struct sockaddr *)server , &serverSize);
    printf("Client Connected.... %d\n", clientfd);
    
    //add this client to our array of clients
    sem_wait(&block);
    clients[numberOfConnectedClients++] = clientfd;
    sem_post(&block);
    
    //broadcast(message, clients, numberOfConnectedClients);
    
    pid = fork();
    
    if(pid == 0)
    {
        break;
    }
   }
   
   //only do this if we are the child and have broken out of the infinite loop
   if(pid == 0)
   {
       childStuff(clientfd);
   }
}


void childStuff(int clientfd)
{
    printf("Child Stuff\n");
    sendMessage("Hello from the child.  Do you have candy?", clientfd);
    receiveMessage(clientfd);
}

void receiveMessage(int clientfd)
{
    int MAX_SIZE = 2000 * sizeof(char);
    char* server_reply = malloc(MAX_SIZE);
    int error = recv(clientfd, server_reply, MAX_SIZE, 0);
    puts(server_reply);
}

void setupServerSocket()
{
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
	
	int error = bind(sockfd, (struct sockaddr *)server, sizeof(struct sockaddr));
    if(error < 0)
    {
        puts("Problem with binding...\n");
    }
    
    error = listen(sockfd, 100);
}

void sendMessage(char* msg, int client)
{
    puts("Sending a message");
    printf("Size of msg is: %d\n", (int)sizeof(*msg));
    printf("Size of msg is: %d\n", (int)strlen(msg));
    send(client , msg , strlen(msg) , 0);
    puts("Message Sent");
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