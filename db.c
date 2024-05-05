#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static void db_initialize(DBManager *manager);

DBManager *db_open(const char *filename) {
	DBManager *manager = malloc(sizeof(DBManager));
	if (!manager) {
		fprintf(stderr, "Error: Memory allocation failed\n");
		return NULL;
	}

	int file_exists = access(filename, F_OK);
	int rc = sqlite3_open(filename, &manager->db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error: Can't open database: %s\n", sqlite3_errmsg(manager->db));
		free(manager);
		return NULL;
	}

	// If database didn't exist, initialize its tables
	if (file_exists == -1) {
		db_initialize(manager);
	}

	return manager;
}

void db_close(DBManager *manager) {
	if (manager) {
		sqlite3_close(manager->db);
		free(manager);
	}
}

int db_reset(DBManager *manager) {
	char *sql = "DELETE FROM items;";

	int rc = sqlite3_exec(manager->db, sql, NULL, NULL, NULL);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error: Failed to reset database: %s\n", sqlite3_errmsg(manager->db));
		return 0; // indicate failure
	}
	else {
		printf("Database reset successfully.\n");
		return 1; // indicate success
	}
}

int db_insert_item(DBManager *manager, const char *name, const char *description, double price) {
	char *sql = "INSERT INTO items (name, description, price) VALUES (?, ?, ?);";

	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(manager->db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error: Failed to prepare SQL statement: %s\n", sqlite3_errmsg(manager->db));
		return 0;
	}

	sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, description, -1, SQLITE_STATIC);
	sqlite3_bind_double(stmt, 3, price);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Error: Failed to insert item: %s\n", sqlite3_errmsg(manager->db));
		sqlite3_finalize(stmt);
		return 0;
	}

	sqlite3_finalize(stmt);
	printf("Item inserted successfully.\n");
	return 1;
}

int db_delete_item(DBManager *manager, const char *name) {
	char *sql = "DELETE FROM items WHERE name = ?;";

	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(manager->db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error: Failed to prepare SQL statement: %s\n", sqlite3_errmsg(manager->db));
		return 0;
	}

	sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Error: Failed to delete item: %s\n", sqlite3_errmsg(manager->db));
		sqlite3_finalize(stmt);
		return 0;
	}

	sqlite3_finalize(stmt);

	if (sqlite3_changes(manager->db) == 0) {
		printf("Item %s not found in the database.\n", name);
		return 0;
	}

	printf("Item %s deleted successfully.\n", name);
	return 1;
}

void db_list_all_items(DBManager *manager) {
	char *sql = "SELECT * FROM items;";

	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(manager->db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error: Failed to prepare SQL statement: %s\n", sqlite3_errmsg(manager->db));
		return;
	}

	printf("List of all items:\n");
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		printf("Name: %s, Description: %s, Price: %.2f\n",
				sqlite3_column_text(stmt, 1),
				sqlite3_column_text(stmt, 2),
				sqlite3_column_double(stmt, 3));
	}

	sqlite3_finalize(stmt);
}

// todo: Change this to specific item lister
void db_get_item_by_name(DBManager *manager, const char *name) {
	char *sql = "SELECT * FROM items WHERE name = ?;";

	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(manager->db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error: Failed to prepare SQL statement: %s\n", sqlite3_errmsg(manager->db));
		return;
	}

	sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

	printf("Item information for %s:\n", name);
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		printf("Name: %s, Description: %s, Price: %.2f\n",
				sqlite3_column_text(stmt, 1),
				sqlite3_column_text(stmt, 2),
				sqlite3_column_double(stmt, 3));
	}
	else {
		printf("Item '%s' not found.\n", name);
	}

	sqlite3_finalize(stmt);
}

// Function for modifying item descriptors
int db_modify_item(DBManager *manager, const char *name, const char *new_name, const char *new_description, double new_price) {
	
	// Allocate memory for the SQL statement
	int MAX_SQL_LENGTH = 1024;
	char *sql = malloc(MAX_SQL_LENGTH);
	if (sql == NULL) {
		fprintf(stderr, "Error: Memory allocation failed\n");
		return 0;
	}

	// Initialize the SQL statement
	strcpy(sql, "UPDATE items SET ");

	// Concatenate the SET clauses based on the provided parameters
	int set_clause_count = 0;
	if (new_name != NULL) {
		strcat(sql, "name = ?, ");
		set_clause_count++;
	}
	if (new_description != NULL) {
		strcat(sql, "description = ?, ");
		set_clause_count++;
	}
	if (new_price != -1) {
		strcat(sql, "price = ?, ");
		set_clause_count++;
	}

	// Check if there's any SET clauses
	if (set_clause_count == 0) {
		free(sql);
		return 1;
	}

	// Remove the trailing comma and space " ,"
	sql[strlen(sql) - 2] = '\0';
	// Append the WHERE clause
	strcat(sql, " WHERE name = ?;");

	// Prepare the SQL statement
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(manager->db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error: Failed to prepare SQL statement: %s\n", sqlite3_errmsg(manager->db));
		free(sql);
		return 0;
	}

	// Cutterpillar the arguments if needed
	int paramIndex = 1;
	if (new_name != NULL) {
		sqlite3_bind_text(stmt, paramIndex++, new_name, -1, SQLITE_STATIC);
	}
	if (new_description != NULL) {
		sqlite3_bind_text(stmt, paramIndex++, new_description, -1, SQLITE_STATIC);
	}
	if (new_price != -1) {
		sqlite3_bind_double(stmt, paramIndex++, new_price);
	}
	// For the WHERE clause bindig...
	sqlite3_bind_text(stmt, paramIndex, name, -1, SQLITE_STATIC);
	
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Error: Failed to modify item: %s\n", sqlite3_errmsg(manager->db));
		sqlite3_finalize(stmt);
		free(sql);
		return 0;
	}

	sqlite3_finalize(stmt);
	free(sql);

	printf("Item '%s' modified successfully.\n", name);
	return 1;
}


// Static functions are like private functions, not intended to be called by the user
static void db_initialize (DBManager *manager) {
	char *sql = "CREATE TABLE IF NOT EXISTS items ("
		"id INTEGER PRIMARY KEY,"
		"name TEXT NOT NULL UNIQUE,"
		"description TEXT,"
		"price REAL NOT NULL"
		");";

	int rc = sqlite3_exec(manager->db, sql, NULL, NULL, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error: Failed to create table %s\n", sqlite3_errmsg(manager->db));
	}
	else {
		printf("Table 'items' created successfully.\n");
	}
}
