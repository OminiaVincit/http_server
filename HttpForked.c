/*
 * Network computing
 * 		Assignment 2: A HTTP forked server program
 *      Written by Tran Quoc Hoan
 *			programmed by milestones method (step by step)
 * 		Usage: ./http_forked serverIP
*/

#include "ServerUtils.h"

/* Signal handler for zoombie processs */
static void wait_for_child( int sig ) {
	while ( waitpid( -1, NULL, WNOHANG ) > 0 );
}

int main ( int argc, char* argv[] ) {
	/* Usage */
	if ( argc != 2 ) {
		fprintf ( stderr, "Usage: %s <Server IPaddress>\n", argv[0] );
		return 0;
	}
	
	/* address information */
	int listenfd;
	struct sigaction sa;
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
	
	/* Listen */
	if ( listen( listenfd, LISTENQ ) == -1 ) 
		err_handle( "listen" );
	
	freeaddrinfo( rcv );
	
	/* Set up the signal handler */
	sa.sa_handler = wait_for_child;
	sigemptyset( &sa.sa_mask );
	sa.sa_flags = SA_RESTART;
	if ( sigaction( SIGCHLD, &sa, NULL) == -1 ) 
		err_handle ( "sigaction" );

	/* Main loop */
	while (1) {
	  struct sockaddr_in comming_addr;
	  size_t size = sizeof( struct sockaddr_in );
	  int confd = accept( listenfd, (struct sockaddr*)&comming_addr, &size );
	  pid_t pid;
	  
	  if ( confd == -1 ) err_handle( "accept" );

	  printf("Comming connection from %s on port %d \r\n", inet_ntoa(comming_addr.sin_addr), htons(comming_addr.sin_port));

	  pid = fork();
	  if ( pid == 0 ) {
	    /* Child process */
	    close( listenfd );
	    sock_handle( confd );
	    close( confd );
	    return 1;
	  }
	  else {
	    /* Parent process */
	    if ( pid == -1 ) err_handle( "fork" );
	    close( confd );
	  }
	}
	close( listenfd );
	return 1;
}
