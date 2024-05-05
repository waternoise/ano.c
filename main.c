#include <stdio.h>
#include "db.h"

int main() {
	DBManager *manager = db_open("example.db");
	if (!manager) {
		fprintf(stderr, "Failed to open database\n");
		return 1;
	}
	// Perform database operations...
	// Delete this: db_initialize(manager);
	// Not yet: db_reset(manager);
	// Put here operations, remember to close it after.
	//db_insert_item(manager, "ligo green", "sardinas na green", 25);
	db_insert_item(manager, "green", "sardinas na green", 25);
	db_insert_item(manager, "red", "sardinas na red", 28);
	db_insert_item(manager, "joy", "blue syrup", 45.32);
	db_list_all_items(manager);
	db_get_item_by_name(manager, "red");
	db_get_item_by_name(manager, "green");
	db_get_item_by_name(manager, "lamp");
	db_modify_item(manager, "red", NULL, NULL, 145.34);
	db_modify_item(manager, "joy", "Joy Antibac", "dishwashing solution", 16);
	db_modify_item(manager, "green", "Ligo Green", "sardinas na green", 30.24);
	//db_modify_item(manager, "lol", NULL, NULL, -1);
	db_list_all_items(manager);
	db_close(manager);
	return 0;
}

