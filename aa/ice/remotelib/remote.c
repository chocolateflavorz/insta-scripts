/*
 * $Id: remote.c,v 1.1 2023/07/26 23:58:18 tomo Exp $
 * TCP/IP経由で測定器を制御する関数(SCPI)
 * 美しない(特にデッドタイムの処理)なので真似をしないこと！！
 * ちゃんと理解して、自分なりに変更すべし！
 * TOmo, Jun 29, 2023 (alpha)
 * July 27, 2023 rev 1.0
 */

#ifdef	WINDOWS
 #include	<winsock2.h>
#else
 #include	<sys/types.h>
 #include	<sys/socket.h>
 #include	<netinet/in.h>
 #include	<arpa/inet.h>
 #define	INVALID_SOCKET	-1
#endif

#include	<stdio.h>
#include	<stdlib.h>
#include	<stdbool.h>
#include	<string.h>
#include	<time.h>

#include	"remote.h"					// Local defs.

/*
 * メッセージ間の待ち時間。美しくない実装なので、要変更
 * 相手側機器の状況を確認してから次のメッセージを送るようにするのが普通
 * sleep() : 1秒単位。Windowsには存在しない
 * Sleep() : 1ms単位。unistdには存在しない
 * usloop(): 1us単位。POSIXではない（使用非推奨)
 */
struct timespec dead_time = { 0, 500000000L };	// 5.0e8 nano-second delay

/*******************************************************************************/

/*
 * Create socket handler and connect to specified equipment
 */
int	connect_equip(const char *equip_addr, const int port)
{
	int			socket_fd ;
	unsigned	address_size ;
	struct sockaddr_in		address ;
	
	address_size = sizeof(address) ;
	
	/*
	 * Make a socket as a client
	 * IPv4, Full duplex (stream), default implementation
	 */
	if( ( socket_fd = socket(PF_INET, SOCK_STREAM,0) ) == INVALID_SOCKET ){
		fprintf(stderr, "Couldn't create socket for %s\n", equip_addr) ;
		return(-1) ;
	}
	
	/*
	 * Assign attribute
	 */
	address.sin_family = AF_INET ;
	address.sin_addr.s_addr = inet_addr(equip_addr) ;
	address.sin_port = htons(port) ;
	if( connect( socket_fd, (struct sockaddr *) &address, address_size) != 0 ){
		fprintf(stderr, "Couldn't connect to %s\n", equip_addr );
		return(-1) ;
	}
	
	return( socket_fd ) ;
}

/*******************************************************************************/

/*
 * Send a message to a equipment
 */
int	send_to_equipment(const int socket_fd, const char *msg )
{
	int		msg_len ;
	char	*buffer ;
	
	msg_len = strlen(msg) ;
	
	/*
	 * Add End of Line ( <NL>= 0x0a )
	 */
	if( ( buffer = (char *)calloc((msg_len+2), sizeof(char)) ) == NULL ){
		fprintf(stderr, "Memory allocation error.\n" );
		return(-1) ;
	}
	strcpy(buffer, msg) ;
	*(buffer + msg_len ) = NEW_LINE ;
	
	/*
	 * Send a message
	 */
	msg_len = send( socket_fd, buffer, strlen(buffer), 0 ) ;
	nanosleep( &dead_time, NULL ) ;
	
#ifdef	DEBUG
	fprintf(stderr, "[DEBUG] Socket= %d : Send message: %s", socket_fd, buffer) ;
#endif
	
	free(buffer) ;
	
	return(msg_len) ;
}

/*******************************************************************************/

/*
 * Query (Send query message and Receive response
 */
int	query_to_equipment(const int socket_fd, const char *query,
					                        char *response, int len, bool binary)
{
	int		res_length ;

	/*
	 * Send Query command (Terminatated by '?' ) and receive a response
	 */
	send_to_equipment( socket_fd, query ) ;			// Send query command
	if ( binary ){
		res_length = recv( socket_fd, response, len, 0) ;
	} else {
		res_length = recv( socket_fd, response, (len - 1), 0) ;
		*(response + res_length) = 0x00 ;			// EOT
	} ;

#ifdef	DEBUG
	fprintf(stderr, "[DEBUG] Socket = %d, Response: %s", socket_fd, response ) ;
#endif
	
	return(res_length) ;
}

/*******************************************************************************/

