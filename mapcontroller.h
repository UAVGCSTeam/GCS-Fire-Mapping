#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QObject>
#include <QVariant>
#include <QPair>
#include <QVector>
#include <ctime>
#include <QTimer> //for demonstration
#include "droneclass.h"
#include "markersmodel.h"

/*
 * Qt uses Slots and Signals to create responsive UI/GUI applications.
 * It allows for communication between QML and C++.
 * https://doc.qt.io/qt-6/signalsandslots.html
*/

/*
 * Our API to control all map functionality.
 * Everything regarding the map should go here.
 * Ensures separation of different functions.
 * Keeps logic in cpp and QML purely for UI.
*/

class MapController : public QObject
{
    Q_OBJECT

public:
    explicit MapController(QObject *parent = nullptr);
    // Q_INVOKABLE void debugPrintDrones() const;

    //this should probably take an input stream and not a name in the future
    Q_INVOKABLE void createDrone(const QString &input_name);


public slots:
    void setCenterPosition(const QVariant &lat, const QVariant &lon);
    void setLocationMarking(const QVariant &lat, const QVariant &lon, const QString &type);
    void changeMapType(int typeIndex);

    Q_INVOKABLE void addDrone(DroneClass* drone);
    Q_INVOKABLE void addMarker(MarkerClass* marker);
    Q_INVOKABLE void addMarker(DroneClass* drone);
    Q_INVOKABLE void removeMarker(MarkerClass* marker);
    Q_INVOKABLE void removeMarker(int index);
    Q_INVOKABLE void updateMarker(MarkerClass* marker, double lat, double lon);
    Q_INVOKABLE void updateMarker(DroneClass* marker, double lat, double lon);
    Q_INVOKABLE void toggleTypeVisibility(const QString &type, bool vis);
    //Q_INVOKABLE QVariantList getAllDrones() const;

    Q_INVOKABLE QAbstractListModel* markersModel() const {return m_markersModel;};
    void droneDemo();
signals:
    void centerPositionChanged(const QVariant &lat, const QVariant &lon);
    void mapTypeChanged(int typeIndex);
    //void locationMarked(MarkerClass* marker);
    //void markerUpdated(MarkerClass* marker);

private:
    QPair<double, double> m_center;
    int m_currentMapType;
    int m_supportedMapTypesCount;

    QVector<DroneClass*> m_drones;
    MarkersModel* m_markersModel;

    void updateCenter(const QPair<double, double> &center);

    //for demo
    QTimer* m_droneTimer;
    double m_angle;
};

#endif // MAPCONTROLLER_H
