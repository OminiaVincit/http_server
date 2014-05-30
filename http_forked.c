/*
 * Network computing
 * 		Assignment 2: A HTTP forked server program
 *      Written by Tran Quoc Hoan
 *			programmed by milestones method (step by step)
*/

#include <stdio.h>
#include <string.h>		/* for memset() function */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <netdb.h>

#define PORT 22609  /* My port to listen on, in test server = uuid + 21600 */
#define BACKLOG 10  /* Default backlog pass to listen */

/* Signal handler for zoombie processs */
static void wait_for_child( int sig ) {
	while ( waitpid( -1, NULL, WNOHANG ) > 0 );
}

/* Handle for each socket */
void handle( int newsock ) {
	/* recv(), send(), close() */
}
int main ( int argc, char* argv[] ) {
	/* address information */
	int sock;
	struct sigaction sa;
	struct addrinfo servaddr, *rcv;
	int reuseaddr = 1; // true
	
	/* Get the address info */
	memset( &servaddr, 0, sizeof( struct addrinfo ) );
	servaddr.ai_family = AF_INET;
	servaddr.ai_socktype = SOCK_STREAM;
	
	if ( getaddrinfo( NULL, PORT, &servaddr, &rcv ) != 0 ) {
		perror( "getaddrinfo" );
		return 1;
	}
	
	/* Create the socket */
	sock = socket( res->ai_family, res->ai_socktype, res->ai_protocol );
	if ( sock == -1 ) {
		perror( "socket" );
		return 1;
	}
	
	/* Enable the socket to reuse the address immediately */
	if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof( int ) ) == -1 ) {
		perror( " setsockopt" );
		return 1;
	}
	
	/* Bind to the address */
	if ( bind( sock, res->ai_addr, res->ai_addrlen ) == -1 ) {
		perror( "listen" );
		return 1;
	}
	
	/* Listen */
	if ( listen( sock, BACKLOG ) == -1 ) {
		perror( "listen" );
		return 1;
	}
	
	freeaddrinfo( res );
	
	/* Set up the signal handler */
	sa.sa_handler = wait_for_child;
	
	/* Main loop */
	return 1;
}