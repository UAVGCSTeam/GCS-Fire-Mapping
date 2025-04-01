#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QObject>
#include <QVariant>
#include <QPair>
#include <QVector>
#include <ctime>
#include <unordered_map>
#include <functional>
#include "droneclass.h"
#include "markersmodel.h"
#include <QTimer> //for demonstration

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
struct QPairHash{
    std::size_t operator()(const QPair<double, double> &pair) const{
        std::size_t h1 = std::hash<double>{}(pair.first);
        std::size_t h2 = std::hash<double>{}(pair.second);
        return h1 ^ (h2 << 1);
    }
};
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
    Q_INVOKABLE void addMarker(MarkerClass* marker, bool hitDeconflictEnabled = false);
    Q_INVOKABLE void removeMarker(MarkerClass* marker, bool hitDeconflictRecover = false);
    Q_INVOKABLE void removeMarker(int index);
    Q_INVOKABLE void updateMarker(MarkerClass* marker, const  double &lat, const double &lon);
    Q_INVOKABLE void updateDrone(DroneClass* marker, const double &lat, const double &lon);
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
    std::unordered_map<QPair<double,double>,int,QPairHash> markerHits;

    void updateCenter(const QPair<double, double> &center);
    void roundCoords(MarkerClass* marker);

    //for demo
    QTimer* m_droneTimer;
    double m_angle;
    int state = 0;
    MarkerClass* markerArray[100] = {};
};

#endif // MAPCONTROLLER_H
