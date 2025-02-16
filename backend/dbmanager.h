#ifndef DBMANAGER_H
#define DBMANAGER_H

/**
 * SQLite Database Connection
 * Soley used to create, delete Drones in SQLite.
 * C++ Drone class will store in memory for real-time updates
 * Loads Database from startup -- currently Drones will load from SQLite
 *
 * Will work with DroneModel? / UI File
 * @author Gian David Marquez
 */

#include <QString> // Mostly to avoid having to bind stuff on database, mostly cause all the examples are using QString
#include <QSqlDatabase> // Connection Stuff
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DBManager : public QObject {
    Q_OBJECT
public: 
    // Constructor
    /* Informal Comment, not usual format for comments
     * Input: database file name
     * DBManager recieves DB file name and determines
     * where to store the file, explicitly being a data file.
     *
     * Issuses: data is stored in the whatever build Qt runs the application on. [Currently on debug]
     */
    // Delete with parent (so dies after close window)
    explicit DBManager(QObject *parent = nullptr);

    // Destructor
    ~DBManager();

    // Set Booleans as SQL operations should signify sucess or failure in it's access.

    // Connections
    bool isOpen() const;

    // CRUD
    bool addDrone(const QString& name, const QString& type = QString(),
                  const QString& xbeeID = QString(), const QString& xbeeAddress = QString());
    bool deleteDrone(int drone_id);
    void printDroneList(); // essentially Reading the drone, has some basecode 

private:
    QSqlDatabase gcs_db_connection;
    bool createDroneTable();
    bool initDB(); // Intialize database

};

#endif // DBMANAGER_H
