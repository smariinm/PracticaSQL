#include <iostream>
#include "sqlite3.h"  // Incluir el encabezado de SQLite

using namespace std;

// Función para mostrar los valores en la tabla
int showValues(sqlite3* db) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, value FROM example_table";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    cout << "Valores en la tabla:" << endl;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        cout << "ID: " << id << ", Valor: " << value << endl;
    }

    sqlite3_finalize(stmt);
    return 0;
}

// Función para modificar un valor en la tabla
int modifyValue(sqlite3* db, int id, const string& newValue) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE example_table SET value = ? WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    sqlite3_bind_text(stmt, 1, newValue.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "Error executing statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return 1;
    }

    cout << "Valor actualizado correctamente." << endl;
    sqlite3_finalize(stmt);
    return 0;
}

// Función para crear la tabla si no existe
int createTableIfNotExists(sqlite3* db) {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS example_table ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "value TEXT NOT NULL);";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        cerr << "Error creating table: " << errMsg << endl;
        sqlite3_free(errMsg);
        return 1;
    }

    return 0;
}

// Función para insertar valores iniciales
int insertInitialValues(sqlite3* db) {
    const char* sql =
        "INSERT INTO example_table (value) VALUES ('Valor Inicial 1');"
        "INSERT INTO example_table (value) VALUES ('Valor Inicial 2');";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        cerr << "Error inserting values: " << errMsg << endl;
        sqlite3_free(errMsg);
        return 1;
    }

    return 0;
}

int main() {
    sqlite3* db;
    int rc = sqlite3_open("example.db", &db);  // Crear o abrir la base de datos

    if (rc) {
        cerr << "No se puede abrir la base de datos: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    // Crear la tabla si no existe
    if (createTableIfNotExists(db) != 0) {
        sqlite3_close(db);
        return 1;
    }

    // Insertar valores iniciales
    if (insertInitialValues(db) != 0) {
        sqlite3_close(db);
        return 1;
    }

    // Mostrar los valores en la tabla
    showValues(db);

    // Modificar un valor
    int idToUpdate;
    string newValue;
    cout << "Introduce el ID para actualizar: ";
    cin >> idToUpdate;
    cout << "Introduce el nuevo valor: ";
    cin >> newValue;

    modifyValue(db, idToUpdate, newValue);  // Actualizar el valor

    // Mostrar los valores nuevamente
    showValues(db);

    sqlite3_close(db);  // Cerrar la conexión
    return 0;
}
