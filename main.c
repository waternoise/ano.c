#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "db.h"

// Function initializations...
DBManager* start(); // Returns a manager to a DB file

int main(int argc, char *argv[]) {
	// Get a manager if SQLite3 can be used.
	DBManager *manager = start();
	// Put parsing things here for testing
	int opt;
	// Expected values to be given by the user
	char *itemName = NULL;
	char *newItemName = NULL;
	char *newItemDescription = NULL;
	double newItemPrice = 0.0;
	// Flags
	int listAllFlag = 0;
	int resetFlag = 0;
	int changeFlag = 0;
	int insertFlag = 0;
	int deleteFlag = 0;

	// Process command-line arguments
	while ((opt = getopt(argc, argv, "lm:ri:d:")) != -1) {
		switch (opt) {
			case 'l':
				// wants to list all items
				listAllFlag = 1;
				break;
			case 'm':
				// wants to modify an item
				itemName = optarg; // Original item name?
				newItemName = argv[optind++];
				newItemDescription = argv[optind++];
				newItemPrice = strtod(argv[optind++], NULL);
				changeFlag = 1;
				break;
			case 'i':
				// wants to insert a new item
				newItemName = optarg;
				newItemDescription = argv[optind++];
				newItemPrice = strtod(argv[optind++], NULL);
				insertFlag = 1;
				break;
			case 'd':
				// wants to delete an item
				itemName = optarg;
				deleteFlag = 1;
				break;
			case 'r':
				// wants to reset the database table
				resetFlag = 1;
				break;
			case '?':
				// Handle unknown options or missing arguments
				printf("Unknown option: %c\n", optopt);
				db_close(manager);
				break;
			default:
				// Handle any errors
				db_close(manager);
				break;
		}
	}

	// Check first if there are no arguments given except for the item name to be searched for
	if (optind < argc) {
		itemName = argv[optind];
		db_get_item_by_name(manager, itemName);
	}
	// Call the appropriate functions based on the flags
	if (listAllFlag) {
		db_list_all_items(manager);
	}
	if (resetFlag) {
		db_reset(manager);
	}
	if (changeFlag) {
		db_modify_item(manager, itemName, newItemName, newItemDescription, newItemPrice);
	}
	if (insertFlag) {
		db_insert_item(manager, newItemName, newItemDescription, newItemPrice);
	}
	if (deleteFlag) {
		db_delete_item(manager, itemName);
	}

	// CLOSE THE DB CONNECTION AFTER EXITING THE PROGRAM
	// ... where else should I put this? Is this safe?
	db_close(manager);
	return 0;
}


// Function returns a DB manager, be sure to close it no matter what happens.
DBManager* start() {
	DBManager *manager = db_open("items.db");
	if (!manager) {
		fprintf(stderr, "Failed to open database\n");
		exit(EXIT_FAILURE);
	}
	return manager; // Indicate success.
}
