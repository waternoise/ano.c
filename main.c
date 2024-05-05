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
	db_insert_item(manager, "ligo green", "sardinas na green", 25);
	db_insert_item(manager, "ligo red", "sardinas na red", 28);
	db_insert_item(manager, "safeguard big", "sabon na white", 45.32);
	db_list_all_items(manager);
	db_get_item_by_name(manager, "ligo red");
	db_get_item_by_name(manager, "ligo green");
	db_get_item_by_name(manager, "null delata");
	db_close(manager);
	return 0;
}

