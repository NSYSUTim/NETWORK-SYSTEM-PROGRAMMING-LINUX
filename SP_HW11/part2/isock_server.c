/*
 * isock_server : listen on an internet socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include "dict.h"

int main(int argc, char **argv) {
	static struct sockaddr_in server,client;
	int sd,cd,n;
	Dictrec tryit;
	struct hostent *host;
	struct utsname name;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(1);
	}

	/* Create the socket.
	 * Fill in code. */
	if( (sd=socket(AF_INET,SOCK_STREAM,0)) ==-1)
		DIE("socket\n");
	
	/* Initialize address.
	 * Fill in code. */
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	int opt = 1;
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {	//設置可重複使用地址和port
		DIE("setsockopt");
	}

	/* Name and activate the socket.
	 * Fill in code. */
	if( bind(sd,(struct sockaddr *)&server, sizeof(server)) ==-1)	//要綁定到設定好的地址
		DIE("bind\n");
	if(listen(sd,10)==-1)
		DIE("listen\n");

	/* main loop : accept connection; fork a child to have dialogue */
	for (;;) {

		/* Wait for a connection.
		 * Fill in code. */
		int clientsize = sizeof(client);
		if( (cd=accept(sd,(struct sockaddr *)&client, &clientsize)) ==-1)
			DIE("accept\n");

		/* Handle new client in a subprocess. */
		switch (fork()) {
			case -1 : 
				DIE("fork");
			case 0 :
				close (sd);	/* Rendezvous socket is for parent only. */
				/* Get next request.
				 * Fill in code. */
				while ( recv(cd,(void *)&tryit,sizeof(tryit),0) > 0) {
					/* Lookup the word , handling the different cases appropriately */
					switch(lookup(&tryit,argv[1]) ) {	//argv[1]是localhost
						/* Write response back to the client. */
						case FOUND:
							/* Fill in code. */
							if( send(cd,(void *)&tryit,sizeof(tryit),0) ==-1)
								DIE("send FOUND\n");
							break;
						case NOTFOUND:
							/* Fill in code. */
							strcpy(tryit.text,"XXXX");
							if( send(cd,(void *)&tryit,sizeof(tryit),0) ==-1)
								DIE("send NOTFOUND\n");
							break;
						case UNAVAIL:
							DIE(argv[1]);
					} /* end lookup switch */
				} /* end of client dialog */
				exit(0); /* child does not want to be a parent */

			default :
				close(cd);
				break;
		} /* end fork switch */
	} /* end forever loop */
} /* end main */
