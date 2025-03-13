/*
 * lookup8 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet TCP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <sys/types.h>

#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int sockfd;
	static struct sockaddr_in server;
	struct hostent *host;
	static int first_time = 1;

	if (first_time) {        /* connect to socket ; resource is server name */	//resource範例是localhost
		first_time = 0;

		/* Set up destination address. */
		server.sin_family = AF_INET;
		/* Fill in code. */
		server.sin_port = htons(PORT);
		server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
		// struct utsname name;
		// if (uname(&name) == -1)
		// 	DIE("uname");
		// if ((host = gethostbyname(name.nodename)) == NULL)
		// 	DIE("host\n");
		// memcpy( (char *) &server.sin_addr, host->h_addr, host->h_length);

		/* Allocate socket.
		 * Fill in code. */
		if ( (sockfd=socket(AF_INET,SOCK_STREAM,0)) ==-1)
			DIE("socket\n");

		/* Connect to the server.
		 * Fill in code. */
		if(connect(sockfd,(struct sockaddr *)&server,sizeof(server)) ==-1)
			DIE("connect\n");
	}

	/* write query on socket ; await reply
	 * Fill in code. */
	if(send(sockfd,sought,sizeof(Dictrec),0)==-1)
		DIE("lookup8 send\n");

	if(recv(sockfd,sought,sizeof(Dictrec),0)==-1)
		DIE("lookup8 recv\n");

	if (strcmp(sought->text,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
