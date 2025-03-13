/*
 * lookup7 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Unix TCP Sockets
 * The name of the socket is passed as resource.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int sockfd;
	static struct sockaddr_un server;
	static int first_time = 1;
	int n;

	if (first_time) {     /* connect to socket ; resource is socket name */
		first_time = 0;

		/* Set up destination address.
		 * Fill in code. */
		memset(&server, 0, sizeof(struct sockaddr_un));
		server.sun_family = AF_UNIX;
		strcpy(server.sun_path,resource);	//使用server端創立的名字

		/* Allocate socket. */
		if( (sockfd = socket(AF_UNIX,SOCK_STREAM,0)) ==-1){
			perror("lookup7 socket\n");
			exit(1);
		}

		/* Connect to the server.
		 * Fill in code. */
		if(connect(sockfd,(struct sockaddr *)&server,sizeof(server))==-1){
			perror("lookup7 connect\n");
			exit(1);
		}
	}

	/* write query on socket ; await reply
	 * Fill in code. */
	if(send(sockfd,sought,sizeof(Dictrec),0)==-1){	//直接傳sought才不會亂碼，因為已經是指標型別
		perror("lookup7 send\n");
		exit(1);
	}
	if(recv(sockfd,sought,sizeof(Dictrec),0)==-1){
		perror("lookup7 recv\n");
		exit(1);
	}

	if (strcmp(sought->text,"XXXX") != 0) {
		// printf("FOUND lookup7: %s,text: %s\n",sought->word,sought->text);
		return FOUND;
	}
	// printf("lookup7 notfound\n");
	return NOTFOUND;
}
