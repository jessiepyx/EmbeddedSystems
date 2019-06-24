#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

static int callback(void *data, int argc, char **argv, char **azColName)
{
	int i;
	printf("%s: ", (const char*)data);
	for (i = 0; i < argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int main(int argc, char* argv[])
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;
	const char* data = "Callback function called";

	// open database
	rc = sqlite3_open("test.db", &db);
	if (rc)
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	
	printf("Opened database successfully\n");

	// create sql statement
	sql = "SELECT * from LOG";

	// execute sql statement
	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		printf("Operation done successfully\n");
	}
	
	sqlite3_close(db);
	return 0;
}

