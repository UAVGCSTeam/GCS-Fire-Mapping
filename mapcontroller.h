#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QObject>
#include <QVariant>
#include <QPair>
#include <QVector>
#include "droneclass.h"
#include "coordinatelist.h"
#include <QTimer>

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
struct IntPairHash {
    std::size_t operator()(const QPair<int, int>& p) const {
        auto h1 = std::hash<int>{}(p.first);
        auto h2 = std::hash<int>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};
struct IntPairEqual {
    bool operator()(const QPair<int, int>& lhs, const QPair<int, int>& rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

class MapController : public QObject
{
    Q_OBJECT

public:
    explicit MapController(QObject *parent = nullptr);
    // Q_INVOKABLE void debugPrintDrones() const;
    Q_INVOKABLE void createDrone(const QString &input_name);

public slots:
    void setCenterPosition(const QVariant &lat, const QVariant &lon);
    void setLocationMarking(const QVariant &lat, const QVariant &lon);
    void changeMapType(int typeIndex);

    Q_INVOKABLE void addDrone(DroneClass* drone);
    Q_INVOKABLE QVariantList getAllDrones() const;

    //Fire overlay update functions
    Q_INVOKABLE QAbstractListModel* fireMap() {return m_fireMap;};
    Q_INVOKABLE QAbstractListModel* smokeMap() {return m_smokeMap;};
    Q_INVOKABLE void mapFillScan();
    void droneDemo();

signals:
    void centerPositionChanged(const QVariant &lat, const QVariant &lon);
    void locationMarked(const QVariant &lat, const QVariant &lon);
    void mapTypeChanged(int typeIndex);

private:
    constexpr static double delta = 0.000035;
    QPair<double, double> m_center;
    QVector<QPair<double, double>> m_markers;
    int m_currentMapType;
    int m_supportedMapTypesCount;

    QVector<DroneClass*> m_drones;

    void updateCenter(const QPair<double, double> &center);
    void addMarker(const QPair<double, double> &position);

    //Fire overlay update functions
    void addMarker(const QPair<double, double> &c, int type);
    void removeMarker(const QPair<double, double> &c, int type);

    QPair<double,double> roundCoordinates(const QPair<double,double> &c);

    CoordinateList* m_fireMap;
    CoordinateList* m_smokeMap;
    //for demo
    QTimer* m_droneTimer;
    int state = 0;
};

#endif // MAPCONTROLLER_H
