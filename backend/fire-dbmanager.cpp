#include "fire-dbmanager.h"

#include <QSqlQuery>
#include <QCoreApplication>
#include <QSqlError>
#include <QDir>

FireDBManager::FireDBManager(QObject *parent) : QObject(parent) {
    gcs_fire_db_connection = QSqlDatabase::addDatabase("QSQLITE"); // Signals for Qt that the DB will be in SQLite

    // Set database file path directly to gcs.db
    QString dbName = "gcsFire.db";

    // Store in data directory, except it does it under build !!!
    QDir dataDir(QCoreApplication::applicationDirPath() + "/data");
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
    }
    QString dbPath = dataDir.filePath(dbName);

    gcs_fire_db_connection.setDatabaseName(dbPath);

    if (!gcs_fire_db_connection.open()) {
        qCritical() << "Database connection failed:" << gcs_fire_db_connection.lastError().text();
        // this is lowkey impossible
    } else {
        qDebug() << "Database connected at:" << dbPath;
    }
}


// Destructor: Close database connection
FireDBManager::~FireDBManager() {
    if (gcs_fire_db_connection.isOpen()) {
        gcs_fire_db_connection.close();
        qDebug() << "Database connection closed.";
    }
}

// Initialize Database (Check if DB exists, create if not)
void FireDBManager::initDB() {
    if (!gcs_fire_db_connection.isOpen()) {
        qCritical() << "Database is not open!";
    }

    if (!createFireTable()) {
        qCritical() << "Table creation failed!";
    }
}



bool FireDBManager::createFireTable() {
    if (!gcs_fire_db_connection.isOpen()) {
        qCritical() << "Database is not open! Cannot create table.";
        return false;
    }

    // Create the table if it does not exist
    QSqlQuery query(gcs_fire_db_connection);
    QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS fires (
            fire_longitude REAL NOT NULL,
            fire_latitude REAL NOT NULL,
            time_of_detection INTEGER NOT NULL,
            drone_id INTEGER NOT NULL
        );
    )";

    if (!query.exec(createTableQuery)) {
        qCritical() << "Failed to create table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Fire table created successfully.";
    return true;
}



bool FireDBManager::isOpen() const {
    return gcs_fire_db_connection.isOpen();
}



// CRUD ME

bool FireDBManager::addFire(const double fireLongitude, const double fireLatitude, const int timeOfDetection, const int droneId) {
    if (!gcs_fire_db_connection.isOpen()) {
        qCritical() << "Database is not open! Cannot add fire.";
        return false;
    }
    QSqlQuery insertQuery;

    // I don't believe that a duplicate check is needed for fires

    // Insert new fire
    insertQuery.prepare(R"(
        INSERT INTO fires (fire_longitude, fire_latitude, time_of_detection, drone_id)
        VALUES (:fireLongitude, :fireLatitude, :timeOfDetection, :droneId);
    )");

    insertQuery.bindValue(":fireLongitude", fireLongitude);
    insertQuery.bindValue(":fireLatitude", fireLatitude);
    insertQuery.bindValue(":timeOfDetection", timeOfDetection);
    insertQuery.bindValue(":droneId", droneId);

    if (!insertQuery.exec()) {
        qCritical() << "Failed to add fire:" << insertQuery.lastError().text();
        return false;
    }

    qDebug() << "Fire added successfully: " << fireLongitude << ", " << fireLatitude;
    return true;
}



bool FireDBManager::removeFire(const double fireLongitude, const double fireLatitude) {
    if (!gcs_fire_db_connection.isOpen()) {
        qCritical() << "Database is not open! Cannot remove fire.";
        return false;
    }

    qCritical() << "Function not implemented!";
    return false; // lol
}



// Testing funtion, to print on console
// Shows how to query to fetch all fires, useful for fires list
void FireDBManager::printFireList() {
    if (!gcs_fire_db_connection.isOpen()) {
        qCritical() << "Database is not open! Cannot fetch fires.";
    }

    QSqlQuery query("SELECT fire_latitude, fire_longitude, time_of_detection, drone_id FROM fires", gcs_fire_db_connection);

    qDebug() << "---- Fires List ----";
    bool hasData = false;

    while (query.next()) {
        hasData = true;
        double longitude = query.value(0).toDouble();
        double latitude = query.value(1).toDouble();
        int time = query.value(2).toInt();
        int droneId = query.value(3).toInt();

        qDebug() << "Longitude:" << longitude 
                 << "| Latitude:" << latitude
                 << "| Time:" << time 
                 << "| Drone ID:" << droneId;
    }

    if (!hasData) {
        qDebug() << "No drones found.";
    }
}


