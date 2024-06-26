#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <sqlite3.h>

typedef struct {
    sqlite3 *db;
} DBManager;

// Function declarations
DBManager *db_open(const char *filename);
void db_close(DBManager *manager);
int db_reset(DBManager *manager);
int db_insert_item(DBManager *manager, const char *name, const char *description, double price);
int db_delete_item(DBManager *manager, const char *name);
void db_list_all_items(DBManager *manager);
void db_get_item_by_name(DBManager *manager, const char *name);
int db_modify_item(DBManager *manager, const char *name, const char *new_name, const char *new_description, double new_price);
// void db_initialize(DBManager *manager);


#endif
