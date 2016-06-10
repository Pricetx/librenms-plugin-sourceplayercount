#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define NAGIOSOK 0
#define NAGIOSWARN 1
#define NAGIOSCRIT 2
#define NAGIOSUNKNOWN 3

#define SOURCEPACKETSIZE 1400

// The below header is required for Source Engine ServerQuery packets
const char header[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0x54 };

// This tells the server to respond with A2S_INFO
const char* msg = "Source Engine Query\0";

int
main(int argc, char *argv[])
{
	int s = 0;
	int status = 0;
	long warnplayers = 1;
	long critplayers = 4;
	struct addrinfo hints;
	struct addrinfo *res;
	char buf[SOURCEPACKETSIZE];
	struct timeval tv;
	tv.tv_sec = 30;


	// Parse command line args
	if (argc != 5) {
		fprintf(stderr, "Invalid number of arguments, expected (host,port,warn,crit)\n");
		exit(NAGIOSCRIT);
	}

	warnplayers = strtol(argv[3], NULL, 10);
	critplayers = strtol(argv[4], NULL, 10);

	// Set the properties for the connection to the source server
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET;     // Force IPv4
	hints.ai_socktype = SOCK_DGRAM; // Set to UDP

	// Initialise addrinfo
	if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
    		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    		exit(NAGIOSCRIT);
	}

	// Create the socket
	if ((s = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		exit(NAGIOSCRIT);
	}

	// Set the socket timeout
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	// Open a connection to the remote server
	if ((status = connect(s, res->ai_addr, res->ai_addrlen)) != 0) {
		fprintf(stderr, "connect error: %s\n", strerror(errno));
		exit(NAGIOSCRIT);
	}

	// The full message will be the length of the message + the 5 byte header
	char* fullmsg = malloc(strlen(msg) + 5);

	// Copy the first 5 bytes of the header. We don't want the 6th byte (ETX)
	strncpy(fullmsg, header, 5);
	strcat(fullmsg, msg);

	int len = strlen(fullmsg);

	// Send the message to the remote server
	if ((status = send(s, fullmsg, len+1, 0)) == -1) {
		fprintf(stderr, "send error: %s\n", strerror(errno));
		exit(NAGIOSCRIT);
	}

	// Get the response
	if ((status = recv(s, buf, SOURCEPACKETSIZE, 0)) == -1) {
		fprintf(stderr, "recv error: %s\n", strerror(errno));
		exit(NAGIOSCRIT);
	}

	// The player count is located after 4 variables length strings
	// Get a pointer to the end of the 4th string. From there it's
	// a fixed offset.
	int pos = 0;
	int count = 0;
	for(int i = 0; i < sizeof(buf); i++) {
		pos = i;

		if (buf[i] == '\0') {
			count++;
		}

		if(count >= 4) {
			break;
		}
	}

	// This is the actual playercount figure
	int players = buf[pos + 3];

	if (players < warnplayers) {
		printf("Player Count OK - %d player(s) online|players=%d;%ld;%ld\n", players, players, warnplayers, critplayers);
		exit(NAGIOSOK);
	} else if (players >= warnplayers && players < critplayers) {
		printf("Player Count WARN - %d player(s) online|players=%d;%ld;%ld\n", players, players, warnplayers, critplayers);
		exit(NAGIOSWARN);
	} else if (players >= critplayers) {
		printf("Player Count CRITICAL - %d player(s) online|players=%d;%ld;%ld\n", players, players, warnplayers, critplayers);
		exit(NAGIOSCRIT);
	} else {
		fprintf(stderr, "UNKNOWN: An unknown error has occured\n");
		exit(NAGIOSUNKNOWN);
	}

	close(s);
	freeaddrinfo(res);
}
