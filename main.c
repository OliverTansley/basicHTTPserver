#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 8080



int main(int argc, char const *argv[])
{

    int server_fd;
    struct sockaddr_in address;

    /**
     * cretes a socket which uses: 
     * IP (AF_INET), 
     * virtual circuit service (SOCK_STREAM), 
     * TCP (the only protocol for this address family therefore 0)
    */
    if((server_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("Cannot create socket");
        return 0;
    }
    
    /**
     * declare an address struct to store data about where the socket is running and what it is running
    */
    memset((char *)&address, 0, sizeof(address)); 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = htonl(INADDR_ANY); /* htonl converts a long integer (e.g. address) to a network representation */ 
    address.sin_port = htons(PORT);  /* htons converts a short integer (e.g. port) to a network representation */ 


    /**
     * bind the socket to the address created
    */
    if (bind(server_fd,(struct sockaddr *)&address,sizeof(address)) < 0) 
    { 
        perror("bind failed"); 
        return 0; 
    }

    /**
     * use the created socket to listen for incoming connections
    */
    if(listen(server_fd,3) < 0){
        perror("error in listen");
        return 0;
    }

    while(1){

        FILE * f;
        if((f = fopen("index.html","rb")) == NULL){
            perror("error accessing file");
        }
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

        char *file_text = malloc(fsize + 1);
        fread(file_text, fsize, 1, f);
        fclose(f);

        file_text[fsize] = 0;

        /**
        * accept incoming connections on a new socket
        */
        int new_socket;
        struct sockaddr* new_addr;
        if((new_socket = accept(server_fd,new_addr,(socklen_t*)&new_addr)) < 0){
            perror("error in accept");
            return 0;
        }
        /**
         * send http response using new_socket then close
        */
        char* response[fsize+ 200];
        sprintf(response,"HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: %ld\n\n%s",fsize,file_text);
        
        write(new_socket,response,strlen(response));
        close(new_socket);
    }
    return 0;
}
