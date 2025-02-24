/*
 * $Id: remote.h,v 1.1 2023/07/26 23:59:40 tomo Exp $
 * TCP/IP経由で測定器を制御する関数(SCPI)
 * 美しない(特にデッドタイムの処理)なので真似をしないこと！！
 * ちゃんと理解して、自分なりに変更すべし！
 * TOmo, Jun 29, 2023 (alpha)
 * July 27, 2023 rev 1.0
 */

#ifndef	REMOTE_H
#define	REMOTE_H
#include <time.h>

#define	NEW_LINE		0x0a			// End of line (SCPI)

/*
 * Wellknown ports
 */
#define SCPI_TERM_PORT  5024			// hp standard SCPI port (Terminal mode)
#define SCPI_RAW_PORT	5025			// hp standard SCPI port (without echo back)

/*
 * Global variable
 */
extern	struct timespec dead_time ;		// Interval

/*
 * Proto types
 */
int	connect_equip(const char *equip_addr, const int port) ;
int	send_to_equipment(const int socket_fd, const char *msg ) ;
int	query_to_equipment(const int socket_fd, const char *query,
					                        char *response, int len, bool binary) ;

#endif	// of REMOTE_H
