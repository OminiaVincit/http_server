/*
 * Network computing
 * 		Assignment 2: A HTTP program
 *      Written by Tran Quoc Hoan
 *			programmed by milestones method (step by step)
 *		
 *		ServerUtils.c		
 * 			Connect socket, socket handle
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

#include "ServerUtils.h"

#define TRUE 1
#define FALSE 0

/* Error handling */
void err_handle( char * msg ) {
	perror( msg );
	exit ( EXIT_FAILURE );
}

/* Send home page */
void send_home( int sock ) {
	char outbuf[ MAXBUF + 1];
	memset( &outbuf, 0, sizeof(outbuf) );
	
	// get time now
	time_t timer = time( NULL );
	struct tm *date = localtime( &timer );
    
	snprintf( outbuf, sizeof(outbuf), 
  	    "HTTP/1.0 200 OK\r\n"
  	    "Content-Type:text/html\r\n"
		"Date: %s\r\n"
		"<font color=red><h1>HELLO! Redirect to index.html for more information.</h1></font>\r\n", 
		asctime( date ) );
    send( sock, outbuf, (int)strlen(outbuf), 0);
#ifdef DEBUG
	printf("Finish send_home \n");
#endif
	return;
}

/* Send html header */
void send_header( int sock ) {
	char outbuf[ MAXBUF + 1];
	memset( &outbuf, 0, sizeof(outbuf) );
	
	// get time now
	time_t timer = time( NULL );
	struct tm *date = localtime( &timer );
    
	snprintf( outbuf, sizeof(outbuf), 
  	    "HTTP/1.0 200 OK\r\n"
  	    "Content-Type:text/html\r\n"
		"Date: %s\r\n", asctime( date ) );
    send( sock, outbuf, (int)strlen(outbuf), 0);
#ifdef DEBUG
	printf("Finish send_header \n");
#endif
	return;
}

/* Send html data */
void send_data( int sock, FILE* fp ) {
	int len;
	char outbuf[ MAXBUF + 1];
	memset( &outbuf, 0, sizeof(outbuf) );
	
	while ( !feof(fp) ) {
		len = fread( outbuf, 1, sizeof( outbuf ), fp );
		outbuf[len] ='\0';
		send ( sock, outbuf, len, 0 );
	}
#ifdef DEBUG
	printf("Finish send_data \n");
#endif
	return;
}

/* Send html file */
void send_html( int sock, const char* FILENAME ) {
	FILE* fp;
	fp = fopen( FILENAME, "r" ); 
	if ( fp == NULL ) {
		// send html header only
		send_home( sock );
	} else {
		send_header( sock );
		send_data( sock, fp );
		fclose( fp );
	}
#ifdef DEBUG
	printf("Finish send_html \n");
#endif
	return;
}

/* Handle for each socket */
int sock_handle( int newsock ) {
  	/* recv(), send(), close() */

  	// recv()
	char inbuf[ MAXBUF + 1];
    memset( &inbuf, 0, sizeof(inbuf) );
    if ( recv( newsock, inbuf, sizeof(inbuf), 0 ) < 0 ) {
    	close( newsock );
		perror("recv");
		return FALSE;
    };

#ifdef DEBUG
    printf( "%s", inbuf );
#endif
	
	// send()
	char method[256], url[256], http_ver[256];
	char *request_file;
	
	memset( &method, 0, sizeof( method ) );
	memset( &url, 0, sizeof( url ) );
	memset( &http_ver, 0, sizeof( http_ver ) );
	
	sscanf( inbuf, "%s %s %s", method, url, http_ver );
	// get method
	if ( !strncasecmp( method, "GET", strlen( "GET" ) ) ) {
		// request page is slash "/"
		if ( !strncmp( url, "/", strlen("/") ) && ( strlen(url) == 1 ) ) {
			// sending index file
			request_file = "index.html";
		}
		else {
			// exclude "/"
			request_file = url + 1;
		}
		send_html( newsock, request_file );
	}
	
  return TRUE;
}


