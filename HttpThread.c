/*
 * Network computing
 * 		Assignment 2: A HTTP thread server program
 *      Written by Tran Quoc Hoan
 *			programmed by milestones method (step by step)
 * 		Usage: ./HttpThread serverIP
*/

#include <pthread.h>
#include <stdio.h>
#include <string.h>		/* for memset() function */
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <netdb.h>

#include "ServerUtils.h"

void *send_recv_thread( void *arg ) {
	pthread_detach ( pthread_self() );
	int acc = (int) arg;
	
	sock_handle( acc );
	close( acc );
	pthread_exit((void*) 0);
}

int main ( int argc, char* argv[] ) {
	/* Usage */
	if ( argc != 2 ) {
		fprintf ( stderr, "Usage: %s <Server Name or IPaddress>\n", argv[0] );
		return 0;
	}
	
	/* address information */
	int listenfd;
	pthread_t thread;
	struct addrinfo servaddr, *rcv;
	int reuseaddr = 1; // true
	
	/* Get the address info */
	memset( &servaddr, 0, sizeof( struct addrinfo ) );
	servaddr.ai_family = AF_INET;
	servaddr.ai_socktype = SOCK_STREAM;
		
	if ( getaddrinfo( argv[1], PORT, &servaddr, &rcv ) != 0 ) 
		err_handle( "getaddrinfo" );
	
	/* Create the socket */
	listenfd = socket( rcv->ai_family, rcv->ai_socktype, rcv->ai_protocol );
	if ( listenfd == -1 ) err_handle( "socket" );
	
	/* Enable the socket to reuse the address immediately */
	if ( setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof( int ) ) == -1 ) 
		err_handle( "setsockopt" );
	
	/* Bind to the address */
	if ( bind( listenfd, rcv->ai_addr, rcv->ai_addrlen ) == -1 ) 
		err_handle( "bind" );
	
	freeaddrinfo( rcv );
	
	/* Listen */
	if ( listen( listenfd, LISTENQ ) == -1 ) 
		err_handle( "listen" );
	
	/* Main loop */
	while (1) {
	  struct sockaddr_in comming_addr;
	  size_t size = sizeof( struct sockaddr_in );
	  int confd = accept( listenfd, (struct sockaddr*)&comming_addr, &size );
	  
	  if ( confd == -1 ) err_handle( "accept" );

	  printf("Comming connection from %s on port %d \r\n", inet_ntoa(comming_addr.sin_addr), htons(comming_addr.sin_port));
	  
	  if ( pthread_create( &thread, NULL, send_recv_thread, (void *)confd ) != 0 ) {
		  perror( "pthread_create" );
	  }
	}
	close( listenfd );
	return 1;
}
