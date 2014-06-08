/*
 * Network computing
 * 		Assignment 2: A HTTP select server program
 *      Written by Tran Quoc Hoan
 *			programmed by milestones method (step by step)
 * 		Usage: ./HttpSelect serverIP
*/

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
#include <fcntl.h>

#include "ServerUtils.h"

#define MAXCLIENTS 50

void set_non_blocking( int sock ) {
	int flags;
	flags = fcntl( sock, F_GETFL );
	if ( flags < 0 ) err_handle( "fcntl(F_GETFL)" );
	
	flags = ( flags | O_NONBLOCK );
	if ( fcntl( sock, F_SETFL, flags ) < 0 ) err_handle( "fcntl(F_SETFL)" );
	return ;
}

int main ( int argc, char* argv[] ) {
	/* Usage */
	if ( argc != 2 ) {
		fprintf ( stderr, "Usage: %s <Server Name or IPaddress>\n", argv[0] );
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
	
	/* Set socket nonblocking */
	set_non_blocking( listenfd );
	
	/* Bind to the address */
	if ( bind( listenfd, rcv->ai_addr, rcv->ai_addrlen ) == -1 ) 
		err_handle( "bind" );
	
	freeaddrinfo( rcv );
	
	/* Listen */
	if ( listen( listenfd, LISTENQ ) == -1 ) 
		err_handle( "listen" );
	
	// initialize all client_socket
	int c_socks[ MAXCLIENTS + 1];
	int max_clients = MAXCLIENTS;			
	memset( (int *) &c_socks, 0, sizeof( c_socks ) );
	
	struct timeval waitval;
	maxfd = listenfd;
	
	/* Main loop */
	while (1) {
		/* Setup the fd_set */
		FD_ZERO( &readfds);
		FD_SET( listenfd, &readfds );
		
		// add child sockets to set
		for ( i = 0; i < max_clients; i++ ) {
			fd = c_socks[i];
			if ( fd > 0 ) FD_SET( fd, &readfds );
			if ( fd > maxfd ) maxfd = fd;
		}
		
		// wait for an activity on one of the sockets
		waitval.tv_sec = 2;
		waitval.tv_usec = 500;
		
		n = select( maxfd + 1, &readfds, NULL, NULL, &waitval ) ;
		if ( n < 0 ) err_handle( "select error" );
		/* if something happened on listenfd (master socket)
		* then new comming connection
		*/
		if ( FD_ISSET( listenfd, &readfds )) {
			struct sockaddr_in comming_addr;
		  	size_t size = sizeof( struct sockaddr_in );
			confd = accept( listenfd, (struct sockaddr*)&comming_addr, &size );
			if ( confd == -1 ) err_handle( "accept" );
			printf("Comming connection from %s on port %d, socket %d \r\n", 
				inet_ntoa(comming_addr.sin_addr), htons(comming_addr.sin_port), confd );
			
			set_non_blocking( confd );
			// Add new connected socket to array of client sockets
			for ( i = 0; ( i < max_clients ) && ( confd != -1 ); i++ ) {
				if ( c_socks[i] == 0) {
					c_socks[i] = confd;
					printf(" Adding socket %d to list of sockets \n ", confd );
					confd = -1;
				}
			}
				
			if ( confd != -1 ) {
				printf( "No space left for new client! \n " );
				close ( confd );
			}				 
		}
			
		// Operation on other socket
		for ( i = 0; i < max_clients; ++i ) {
			fd = c_socks[i];
			if ( FD_ISSET( fd, &readfds ) ) {
				if ( sock_handle ( fd ) == 0 ) {
					close ( fd );
					c_socks[i] = 0;
				}	
			}	 
		}
	}
	close( listenfd );
	return 1;
}
