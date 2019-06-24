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
#include <sqlite3.h>
#include <time.h>

#define PORT 8080
#define ADDR "0.0.0.0"
#define QUEUE 20

#define BUFF_SIZE 2048

sqlite3 *db;
char *zErrMsg = 0;
int rc;
char *sql;

time_t t;

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for (i = 0; i < argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int init_log()
{
	rc = sqlite3_open("test.db", &db);
	if (rc)
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	printf("Opened database successfully\n");

	char *sql = "CREATE TABLE LOG(" \
		    "ID	INTEGER PRIMARY KEY	AUTOINCREMENT," \
		    "TIME		TEXT	NOT NULL," \
		    "CLIENT_IP	TEXT," \
		    "CLIENT_PORT	INT," \
		    "SERVER_IP	TEXT," \
		    "SERVER_PORT	INT," \
		    "EVENT		TEXT	NOT NULL," \
		    "DETAIL		TEXT);";

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		printf("Table created successfully\n");
	}

	return 0;
}

int write_log()
{
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}
	return 0;
}

int main()
{
	if(init_log() == -1)
	{
		return -1;
	}

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
	
	time(&t);
	char *tmp = "INSERT INTO LOG (TIME, SERVER_IP, SERVER_PORT, EVENT)" \
		    "VALUES (\"%s\", \"%s\", %d, \"LISTEN\");";
	sql = (char *)malloc(sizeof(char) * BUFF_SIZE);
	sprintf(sql, tmp, ctime(&t), ADDR, PORT);
	if (write_log() == -1)
	{
		return -1;
	}

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
		
		char *client_ip = inet_ntoa(clientAddr.sin_addr);
		int client_port = clientAddr.sin_port;

		printf("A new connection from %s:%d\n", client_ip, client_port);

		time(&t);
		tmp = "INSERT INTO LOG (TIME, CLIENT_IP, CLIENT_PORT, SERVER_IP, SERVER_PORT, EVENT)" \
		      "VALUES (\"%s\", \"%s\", %d, \"%s\", %d, \"CONNECT\");";
		sprintf(sql, tmp, ctime(&t), client_ip, client_port, ADDR, PORT);
		if (write_log() == -1)
		{
			return -1;
		}

		// handle character
		while(1)
		{
			char receiveBuf[BUFF_SIZE];
			int cnt;
			memset(receiveBuf, 0, sizeof(receiveBuf));

			// receive
			cnt = recv(conn, receiveBuf, sizeof(receiveBuf), 0);

			time(&t);
			tmp = "INSERT INTO LOG (TIME, CLIENT_IP, CLIENT_PORT, SERVER_IP, SERVER_PORT, EVENT, DETAIL)" \
			      "VALUES (\"%s\", \"%s\", %d, \"%s\", %d, \"RECEIVE\", \"%s\");";
			sprintf(sql, tmp, ctime(&t), client_ip, client_port, ADDR, PORT, receiveBuf);
			if (write_log() == -1)
			{
				return -1;
			}

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

		time(&t);
	        tmp = "INSERT INTO LOG (TIME, CLIENT_IP, CLIENT_PORT, SERVER_IP, SERVER_PORT, EVENT)" \
		      "VALUES (\"%s\",\"%s\", %d, \"%s\", %d, \"CLOSE\");";
		sprintf(sql, tmp, ctime(&t), client_ip, client_port, ADDR, PORT);
		if (write_log() == -1)
	        {
			return -1;
		}
	}

	close(server);
	return 0;
}
