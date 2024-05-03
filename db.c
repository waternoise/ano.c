#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

// Static functions are like private functions, not intended to be called by the user
static void db_initialize (DBManager *manager) {
	char *sql = "CREATE TABLE IF NOT EXISTS items ("
		"id INTEGER PRIMARY KEY,"
		"name TEXT NOT NULL UNIQUE,"
		"price REAL NOT NULL,"
		"description TEXT"
		");";

	int rc = sqlite3_exec(manager->db, sql, NULL, NULL, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error: Failed to create table %s\n", sqlite3_errmsg(manager->db));
	}
	else {
		printf("Table 'items' created successfully.\n");
	}
}
