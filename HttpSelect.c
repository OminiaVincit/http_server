/*
 * Network computing
 * 		Assignment 2: A HTTP forked server program
 *      Written by Tran Quoc Hoan
 *			programmed by milestones method (step by step)
 * 		Usage: ./HttpSelect serverIP
*/

#include <errno.h>
#include "ServerUtils.h"

#define MAXCLIENTS 50

int main ( int argc, char* argv[] ) {
	/* Usage */
	if ( argc != 2 ) {
		fprintf ( stderr, "Usage: %s <Server IPaddress>\n", argv[0] );
		return 0;
	}
	
	/* address information */
	fd_set readfds;
	int maxfd, i, fd, n, listenfd, confd;
	
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
	
#ifdef DEBUG
				printf( "listenfd = %d\n", listenfd );
#endif
	
	// initialize all client_socket
	int c_socks[ MAXCLIENTS ];
	int max_clients = MAXCLIENTS;
	for ( i = 0; i < max_clients; i++ ) {
		c_socks[i] = 0;
	}				
	
	struct timeval waitval;
	waitval.tv_sec = 2;
	waitval.tv_usec = 500;
	
	/* Main loop */
	while (1) {
		/* Setup the fd_set */
		FD_ZERO( &readfds);
		FD_SET( listenfd, &readfds );
		maxfd = listenfd;
		
		// add child sockets to set
		for ( i = 0; i < max_clients; i++ ) {
			fd = c_socks[i];
			if ( fd > 0 ) FD_SET( fd, &readfds );
			if ( fd > maxfd ) maxfd = fd;
		}
		
		// wait for an activity on one of the sockets
		n = select( maxfd + 1, &readfds, NULL, NULL, &waitval ) ;
		if ( n < 0 && (errno != EINTR )) perror( "select error" );
		
		/* if something happened on listenfd (master socket)
		   then new comming connection
		*/
		if ( FD_ISSET( listenfd, &readfds )) {
			struct sockaddr_in comming_addr;
		  	size_t size = sizeof( struct sockaddr_in );
			confd = accept( listenfd, (struct sockaddr*)&comming_addr, &size );
			if ( confd == -1 ) err_handle( "accept" );
			printf("Comming connection from %s on port %d, socket %d \r\n", 
				inet_ntoa(comming_addr.sin_addr), htons(comming_addr.sin_port), confd );
			
			// Add new connected socket to array of client sockets
			for ( i = 0; i < max_clients; i++ ) {
				if ( c_socks[i] == 0) {
					c_socks[i] = confd;
					printf(" Adding socket %d to list of sockets ", confd );
					break;
				}
			}	
			  			 
		}
		
		// Operation on other socket
		for ( i = 0; i < max_clients; i++ ) {
			fd = c_socks[i];
			
			if ( FD_ISSET( fd, &readfds ) ) {
				if ( !sock_handle( fd) ) {
					close ( fd );
					c_socks[i] = 0;
				}
			} 
		}
	}
	close( listenfd );
	return 1;
}
