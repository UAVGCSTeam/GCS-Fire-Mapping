#ifndef FIREDBMANAGER_H
#define FIREDBMANAGER_H

/**
 * SQLite Database Connection
 * Soley used to store fire locations, time, and number of ML detections
 * Loads Database from startup -- currently fires will load from SQLite
 *
 * @author Gian David Marquez (translated to store wildfire information by Carlos Vargas)
 */

#include <QString> // Mostly to avoid having to bind stuff on database, mostly cause all the examples are using QString
#include <QSqlDatabase> // Connection Stuff
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class FireDBManager : public QObject {
    Q_OBJECT
public: 
    // Constructor
    /* Informal Comment, not usual format for comments
     * Input: database file name
     * DBManager recieves DB file name and determines
     * where to store the file, explicitly being a data file.
     *
     * Issues: data is stored in the whatever build Qt runs the application on. [Currently on debug]
     */
    // Delete with parent (so dies after close window)
    explicit FireDBManager(QObject *parent = nullptr);

    // Destructor
    ~FireDBManager();

    // Set Booleans as SQL operations should signify success or failure in it's access.

    // Connections
    bool isOpen() const;
    void initDB(); // Intialize database


    // CRUD Operations
    bool addFire(const double fireLongitude, const double fireLatitude, const int timeOfDetection, const int droneId);
    bool removeFire(const double fireLongitude, const double fireLatitude); // This function needs an implementation
    int getFireDetectionTime(double fire_longitude, double fire_latitude); 
    qreal getFireCoordinates(); // Maybe replace the double array with a QGeoCoodinate type 
    void printFireList(); 
    int getDroneID(double fire_longitude, double fire_latitude); // Returns the ID of the drone that detected this fire

private:
    QSqlDatabase gcs_fire_db_connection;
    bool createFireTable();

};

#endif // FIREDBMANAGER_H
