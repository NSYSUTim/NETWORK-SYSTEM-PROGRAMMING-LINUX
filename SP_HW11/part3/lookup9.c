/*
 * lookup9 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet UDP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int sockfd;
	static struct sockaddr_in server;
	struct hostent *host;
	static int first_time = 1;

	if (first_time) {  /* Set up server address & create local UDP socket */
		first_time = 0;

		/* Set up destination address.
		 * Fill in code. */
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);
		server.sin_addr.s_addr = inet_addr("127.0.0.1");

		/* Allocate a socket.
		 * Fill in code. */
		if( (sockfd=socket(AF_INET,SOCK_DGRAM,0)) ==-1)
			DIE("client bind\n");
	}

	/* Send a datagram & await reply
	 * Fill in code. */
	
	if(sendto(sockfd,(void *)sought,sizeof(Dictrec),0,(struct sockaddr *)&server,sizeof(server))==-1)
		DIE("sendto\n");
	
	int serverlen = sizeof(server);
	if(recvfrom(sockfd,(void *)sought,sizeof(Dictrec),0,(struct sockaddr *)&server,&serverlen) ==-1)
		DIE("recvfrom\n");

	if (strcmp(sought->text,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
