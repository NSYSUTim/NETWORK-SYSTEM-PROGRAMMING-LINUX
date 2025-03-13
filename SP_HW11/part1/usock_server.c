/*
 * usock_server : listen on a Unix socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "dict.h"

int main(int argc, char **argv) {
    struct sockaddr_un server, client;
    int sd,cd,n;
    Dictrec tryit;	//在dict.h裡有定義

    if (argc != 3) {
      fprintf(stderr,"Usage : %s <dictionary source>"
          "<Socket name>\n",argv[0]);
      exit(errno);
    }

    /* Setup socket.
     * Fill in code. */
    if( (sd = socket(AF_UNIX,SOCK_STREAM,0)) ==-1){
		perror("socket\n");
		exit(1);
	}

    /* Initialize address.
     * Fill in code. */
	server.sun_family = AF_UNIX;		
	strcpy(server.sun_path, argv[2]);	//可隨意取名
	
	unlink(argv[2]);	//如已存在先刪除
	if( bind(sd,(struct sockaddr *)&server,sizeof(server)) ==-1){
		perror("bind\n");
		exit(1);
	}

    /* Name and activate the socket.
     * Fill in code. */
	if(listen(sd,10)==-1){
		perror("listen\n");
		exit(1);
	}

    /* main loop : accept connection; fork a child to have dialogue */
    for (;;) {
		/* Wait for a connection.
		 * Fill in code. */
		int clientsize = sizeof(client);
		if( (cd = accept(sd,(struct sockaddr *)&client,&clientsize)) ==-1){
			perror("accept\n");
			exit(1);
		}

		/* Handle new client in a subprocess. */
		switch (fork()) {
			case -1 : 
				DIE("fork");
			case 0 :
				close (sd);	/* Rendezvous socket is for parent only. */
				/* Get next request.
				 * Fill in code. */
				while (recv(cd,(void *)&tryit,sizeof(tryit),0) > 0 ) {
					//printf("server recieved word: %s\n",tryit.word);
					/* Lookup request. */
					switch(lookup(&tryit,argv[1]) ) {
						/* Write response back to client. */
						case FOUND: 
							/* Fill in code. */
							// printf("server found: %s",tryit.text);
							if(send(cd,(void *)&tryit,sizeof(tryit),0)==-1){
								perror("send fond\n");
								exit(1);
							}
							break;
						case NOTFOUND: 
							/* Fill in code. */
							//printf("server notfound\n");
							strcpy(tryit.text,"XXXX");
							if(send(cd,(void *)&tryit,sizeof(tryit),0)==-1){
								perror("send not found\n");
								exit(1);
							}
							break;
						case UNAVAIL:
							DIE(argv[1]);
					} /* end lookup switch */

				} /* end of client dialog */

				/* Terminate child process.  It is done. */
				exit(0);

			/* Parent continues here. */
			default :
				close(cd);
				break;
		} /* end fork switch */
    } /* end forever loop */
} /* end main */
