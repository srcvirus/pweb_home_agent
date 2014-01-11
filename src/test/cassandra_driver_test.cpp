#include <stdio.h>
#include "../database/cassandra_db.h"
#include "../interfaces/database_driver.h"

/**
 * Schema:
 * 	create table users (
 * 		id int primary key,
 * 		name text
 * 	);
 */

int main(int argc, char *argv[])
{
	CassandraDBDriver* database = (CassandraDBDriver*)CassandraDBDriver::getDatabaseDriverObject();
	database->openConnection();
	database->selectKeySpace("pweb");
	boost::shared_future <cql::cql_future_result_t> results = database->executeQuery("select * from users");

	int nRows = (*(results.get().result)).row_count();
	cql::cql_result_t& rows = (*(results.get().result));

	printf("%d rows returned\n", nRows);
	int rowIndex = 0;

	while(rows.next())
	{
		printf("[Row %d]\t", rowIndex);

		int id;
		string name;

		rows.get_string("name", name);
		rows.get_int("id", id);

		printf("Id = %d\tName = %s\n", id, name.c_str());
		rowIndex++;
	}

	delete database;
	return 0;
}
