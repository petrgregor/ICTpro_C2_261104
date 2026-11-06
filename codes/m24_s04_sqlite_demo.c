/**
 * @file 50_sqlite_demo.c
 * @brief Creating a database and a table using SQLite.
 */
#include <sqlite3.h>
#include <stdio.h>

int main(void) {
    printf("--- SQLite Demo ---\n");

    sqlite3 *db;
    char *err_msg = nullptr;

    // Open a database file (or create it if it doesn't exist)
    int rc = sqlite3_open("test.db", &db);
    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    printf("Successfully connected to test.db\n");

    // SQL statement to create a table and insert dummy data
    const char *sql =
            "CREATE TABLE IF NOT EXISTS Users(Id INT, Name TEXT);"
            "INSERT INTO Users VALUES(1, 'Alice');";

    // Execute the SQL statement
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Table created and data inserted successfully.\n");
    }

    // Close the connection
    sqlite3_close(db);
    return 0;
}
