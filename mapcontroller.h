#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QObject>
#include <QVariant>
#include <QPair>
#include <QVector>
#include <ctime>
#include <QTimer> //for demonstration
#include "droneclass.h"
#include "markerclass.h"
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
    Q_INVOKABLE void createDrone(const QString &input_name);


public slots:
    void setCenterPosition(const QVariant &lat, const QVariant &lon);
    void setLocationMarking(const QVariant &lat, const QVariant &lon, const QString &type);
    void changeMapType(int typeIndex);

    Q_INVOKABLE void addDrone(DroneClass* drone);
    Q_INVOKABLE void addMarker(MarkerClass* marker);
    Q_INVOKABLE void updateMarker(MarkerClass* marker,int index);
    Q_INVOKABLE QVariantList getAllDrones() const;

    Q_INVOKABLE QAbstractListModel* markersModel() const {return m_markersModel;};
    void droneDemo();
signals:
    void centerPositionChanged(const QVariant &lat, const QVariant &lon);
    //void locationMarked(MarkerClass* marker);
    //void markerUpdated(MarkerClass* marker);
    void mapTypeChanged(int typeIndex);

private:
    QPair<double, double> m_center;
    int m_currentMapType;
    int m_supportedMapTypesCount;

    QVector<DroneClass*> m_drones;
    MarkersModel* m_markersModel;

    void updateCenter(const QPair<double, double> &center);

    QTimer* m_droneTimer; //for demonstration
    double m_angle;

};

#endif // MAPCONTROLLER_H
