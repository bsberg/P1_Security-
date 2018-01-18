//
//
//
//@author: Brian Berg
//
//
//

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <time.h>


struct addrinfo* r_getaddrinfo(const char* hostname, const char* port){

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints)); // makes sure strcut is empty 
	hints.ai_family = AF_INET; // sets family to IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP socktype

	hints.ai_flags = AI_PASSIVE;
	
	struct addrinfo *result = malloc(sizeof(struct addrinfo)); // place for result 
	int checkError = getaddrinfo(hostname, port, &hints, &result); // if getaddr doesnt fail result goes here

	// check for error
	if(checkError != 0){
		fprintf(stderr, "getaddrinfo %s\n", gai_strerror(checkError));
		exit(EXIT_FAILURE);
	}

	return result; 	
}

//create socket
int r_socket(struct addrinfo *ai){

	int sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if(sock == -1){
		perror("socket(): error");
		exit(EXIT_FAILURE);
	}
	return sock;
}

// Would have used Perror but cluters the output ALOT
int r_connect(int sock, struct addrinfo *ai){
	int r = connect(sock, ai->ai_addr, ai->ai_addrlen);
	if( r == -1){
		//perror("connect(): failure");
		//printf("%d\n", errno);
		//exit(EXIT_FAILURE);
	}
	return r;
}

int main(int argc, char **argv){

	int connectFlag = 1;
	int i = 1025; // starting port
	char * c = (char*)malloc(10);
	int connectReturn;

	// handshake vars
	char handshake[5] = {67, 65, 80, 83, 0};
	char recvList[5];
	ssize_t recvBytes = 0;
	int p;
	//struct pollfd ufds[1]; // didnt end up using but for poll

	// rand vars
	int stillWorking = 1;
	int rando; 
	char *pass= malloc( sizeof(char) * 5 );
	char *recvPass = malloc( sizeof(char) * 5 );
	char found[5] = {"Found"};
	char *final= malloc( sizeof(char) * 5 );
	int j = 0;
	int recvBytesP;
	
	//time/IP vars
	time_t timer;
    char buffer[26];
    struct tm* tm_info;
    


	pass[5] = 0; 
	// tests every server for TCP then gets more specific for CAPS
	while( i < 65536 && connectFlag != 0){

		sprintf(c, "%d", i );

		// takes hostname and port to create socket 
		struct addrinfo *socketaddrinfo = r_getaddrinfo(argv[1], c);
		int sock = r_socket(socketaddrinfo);
		//ufds->fd = sock;// for poll
	
		// connect to socket 
		connectReturn = r_connect(sock, socketaddrinfo);


		//finds TCP ports sends handshake and recieve conformation
		if(connectReturn != -1){
	
			//printf("TCP PORT:%d\n", i);
			
			if(send(sock, handshake, sizeof(int)*5, 0) < 0){
				printf("error\n");
			}
			//p = poll(ufds, 1, 4500);
			recvBytes = recv(sock, recvList, 7, 0);

			//------------------------------------------------------------
			// resposible for finding the CAPS server 
			// sending input one char at a time
			// and checking that input against found[]
			if(recvBytes > 0){
				printf("Recieved: %s on Port: %d\n", recvList, i);

				while(stillWorking != 0 && j < 4){
					//for(j = 0; j < 4; j++){
						rando = rand() % 150 + 0; 
						if( rando >= 97 && rando <= 122){
							rando = rando - 32;
						}
						pass[j] = rando;
					//}
					if(send(sock, pass, sizeof(char)*5, 0) < 0){
						printf("error\n");
					}

					recvBytesP = recv(sock, recvPass, 5, 0);
					
					if(recvPass[j] == found[j] ){
						printf("%d ", rando);
						j++;
					}

				}
			//-------------------------------------------------------------
				connectFlag = 0;
				printf("0\n");
				printf("Correct input for CAPS server on Port: %d\n", i);

			}
			else if( recvBytes == 0){
				fflush(stdout);
			}
			else{
				//printf("Nope %d\n", errno);
				perror("recv:");
			}
		}

		


		freeaddrinfo(socketaddrinfo);
		close(sock);
		i++;

	}

	//time stuff
	time(&timer);
    tm_info = localtime(&timer);

   	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    puts(buffer);
	
	free(c);
	free(pass);
	free(recvPass);
	free(final);

}