/*
 ============================================================================
 Name        : ServerTCP.c
 Author      : Agata Dul
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc , char *argv[]) {

	int master_socket, new_socket;
	struct sockaddr_in server , client;
    int read_size, socket_bound;
    socklen_t server_addr_length;
    int opt=1;


    if((master_socket = socket(AF_INET , SOCK_STREAM , 0))==-1){
    	perror("Errors occured during socket creating process ");
    }
    else{
    	printf("Socket created successfully\n ");
    }

    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {
            perror("Errors occured during setting socket options");
            exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    server_addr_length=sizeof(server);

    if((socket_bound=bind(master_socket,(struct sockaddr *)&server ,server_addr_length)) == -1){
    	perror("Errors occured during socket binding process ");
    }
    else{
    	printf("Socket bound successfully, port number:%d\n",atoi(argv[1]));
    }

	return EXIT_SUCCESS;
}
