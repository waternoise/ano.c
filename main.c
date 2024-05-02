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
	db_close(manager);
	return 0;
}

