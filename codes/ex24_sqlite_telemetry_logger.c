/**
 * @file ex24_sqlite_telemetry_logger.c
 * @brief Reference solution for Section 24 Exercise: In-Memory SQLite Sensor Logger.
 */
#include <sqlite3.h>
#include <stdio.h>

// Callback invoked by sqlite3_exec for every row matching the SELECT query
static int query_row_callback(void *user_data, int argc, char **argv, char **col_names) {
    int *row_count = (int *)user_data;
    (*row_count)++;

    printf("  Row #%d -> ", *row_count);
    for (int i = 0; i < argc; ++i) {
        printf("%s: %s  ", col_names[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0; // Returning 0 tells SQLite to continue iteration
}

int main(void) {
    printf("=== Exercise 24: In-Memory SQLite Sensor Logger ===\n\n");

    sqlite3 *db = nullptr;
    char *err_msg = nullptr;

    // 1. Open an in-memory database
    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        printf("Cannot open in-memory database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // 2. Create table and insert sample sensor readings
    const char *setup_sql =
            "CREATE TABLE Sensors(Id INT, Location TEXT, Temp REAL);"
            "INSERT INTO Sensors VALUES(101, 'Boiler_Room', 68.5);"
            "INSERT INTO Sensors VALUES(102, 'Outdoor_North', 14.2);"
            "INSERT INTO Sensors VALUES(103, 'Server_Rack', 31.8);";

    if (sqlite3_exec(db, setup_sql, nullptr, nullptr, &err_msg) != SQLITE_OK) {
        printf("SQL setup error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    // 3. Query sensors exceeding 25.0 C using our C callback
    printf("Querying sensors with Temp > 25.0 C:\n");
    int matched_rows = 0;
    const char *query_sql = "SELECT Id, Location, Temp FROM Sensors WHERE Temp > 25.0;";

    if (sqlite3_exec(db, query_sql, query_row_callback, &matched_rows, &err_msg) != SQLITE_OK) {
        printf("SQL query error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    printf("\nTotal high-temperature sensors found: %d\n", matched_rows);
    sqlite3_close(db);
    return 0;
}
