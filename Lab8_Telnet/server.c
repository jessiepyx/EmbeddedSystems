#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define ADDR "0.0.0.0"
#define QUEUE 20

#define BUFF_SIZE 2048

int main()
{
	int fd = open("/dev/matrix", O_WRONLY | O_NONBLOCK);
	if (fd < 0)
	{
		printf("ERROE: Could not open matrix device!\n");
		return -1;
	}

	// establish server
	int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(ADDR);
	serverAddr.sin_port = htons(PORT);

	// bind ip and port
	if (bind(server, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
		printf("ERROR: Could not bind %s:%d\n", ADDR, PORT);
		return -1;
	}

	if (listen(server, QUEUE) == -1)
	{
		printf("ERROR: Listening failed!\n");
		return -1;
	}

	printf("Server running at %s:%d\n", ADDR, PORT);

	while(1)
	{
		struct sockaddr_in clientAddr;
		socklen_t length = sizeof(clientAddr);

		// handle connection
		int conn = accept(server, (struct sockaddr*)&clientAddr, &length);
		if (conn < 0)
		{
			printf("ERROR: Connection failed!\n");
			return -1;
		}

		printf("A new connection from %s:%d\n", inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);

		// handle character
		while(1)
		{
			char receiveBuf[BUFF_SIZE];
			int cnt;
			memset(receiveBuf, 0, sizeof(receiveBuf));

			// receive
			cnt = recv(conn, receiveBuf, sizeof(receiveBuf), 0);

			// leave
			if (cnt == 0)
			{
				close(conn);
				break;
			}

			// display
			write(fd, receiveBuf, cnt);
			printf("%d characters written\n", cnt);
		}
	}
	close(server);
	return 0;
}
