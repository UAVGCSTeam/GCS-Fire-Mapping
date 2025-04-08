#include "mapcontroller.h"
#include <QSet>
#include <queue>
#include <QDebug>

/*
 * Used to emit signals to our QML functions.
 * Keeps logic in cpp.
 * https://doc.qt.io/qt-6/signalsandslots.html
*/

// Define constructor for MapController class
MapController::MapController(DBManager &db, QObject *parent)
    // Defines all variables within our map
    : QObject(parent)
    , m_currentMapType(0)
    , m_supportedMapTypesCount(3)
    , dbManager(db)
    , m_fireMap(new CoordinateList(this))
    , m_smokeMap(new CoordinateList(this))
    , m_droneTimer(new QTimer(this)) // for demo
{
    dbManager.loadOverlayMap(m_fireMap,"fire");
    dbManager.loadOverlayMap(m_smokeMap,"smoke");

    // Populate with dummy drone objects for testing icon markers using setLattitude and setLongitude
    DroneClass* drone1 = new DroneClass(this);
    drone1->setName("Drone 1");
    drone1->setLattitude(34.059174611493965);
    drone1->setLongitude(-117.82051240067321);
    addDrone(drone1);

    DroneClass* drone2 = new DroneClass(this);
    drone2->setName("Drone 2");
    drone2->setLattitude(34.0600);
    drone2->setLongitude(-117.8210);
    addDrone(drone2);

    DroneClass* drone3 = new DroneClass(this);
    drone3->setName("Drone 3");
    drone3->setLattitude(34.0615);
    drone3->setLongitude(-117.8225);
    addDrone(drone3);

    DroneClass* drone4 = new DroneClass(this);
    drone4->setName("Drone 4");
    drone4->setLattitude(37.7749);
    drone4->setLongitude(-122.4194);
    addDrone(drone4);

    DroneClass* drone5 = new DroneClass(this);
    drone5->setName("Drone 5");
    drone5->setLattitude(34.0119);
    drone5->setLongitude(-118.4916);
    addDrone(drone5);

    //for time to flow in demo
    connect(m_droneTimer, &QTimer::timeout, this, &MapController::droneDemo);
    m_droneTimer->start(100);
}

void MapController::addDrone(DroneClass* drone)
{
    if (drone) {
        m_drones.append(drone);
    }
}

QVariantList MapController::getAllDrones() const
{
    QVariantList droneList;
    for (const DroneClass* drone : m_drones) {
        QVariantMap droneData;
        droneData["name"] = drone->getName();
        droneData["latitude"] = drone->getLattitude();
        droneData["longitude"] = drone->getLongitude();
        droneList.append(droneData);
    }
    return droneList;
}

void MapController::createDrone(const QString &input_name){
    DroneClass* temp = new DroneClass(this);
    temp->setName(input_name);
    temp->setLattitude(34.06152);
    temp->setLongitude(-117.82254);
    addDrone(temp);
}

void MapController::setCenterPosition(const QVariant &lat, const QVariant &lon)
{
    QPair<double, double> newCenter(lat.toDouble(), lon.toDouble());
    // updateCenter below
    updateCenter(newCenter);
}

void MapController::setLocationMarking(const QVariant &lat, const QVariant &lon)
{
    QPair<double, double> position(lat.toDouble(), lon.toDouble());
    // addMarker below
    addMarker(position);
}

// emit sends the data that our cpp logic did to our QML files
void MapController::changeMapType(int index)
{
    if (index < m_supportedMapTypesCount) {
        m_currentMapType = index;
        emit mapTypeChanged(index);
        qDebug() << "Changed to map type:" << index;
    } else {
        qDebug() << "Unsupported map type index:" << index;
    }
}

void MapController::updateCenter(const QPair<double, double> &center)
{
    if (m_center != center) {
        m_center = center;
        emit centerPositionChanged(QVariant(center.first), QVariant(center.second));
    }
}

void MapController::addMarker(const QPair<double, double> &position)
{
    // Stores markers on cpp side
    m_markers.append(position);
    emit locationMarked(QVariant(position.first), QVariant(position.second));
}

QPair<double,double> MapController::roundCoordinates(const QPair<double,double> &c){
    return QPair<double,double>(round(c.first/delta) * delta,round(c.second/delta) * delta);
}

void MapController::addOverlayMarker(const QPair<double, double> &c, int type){
    QPair<double,double> p = roundCoordinates(c);
    if(type == 1){
        m_smokeMap->remove(p);
        m_fireMap->insert(p);
    }else if(!(m_fireMap->contains(p))){
        m_smokeMap->insert(p);
    }
}

void MapController::removeOverlayMarker(const QPair<double, double> &c){
    QPair<double,double> p = roundCoordinates(c);
    m_smokeMap->remove(p);
    m_fireMap->remove(p);
}

void MapController::saveOverlay(){
    dbManager.saveOverlayMap(m_fireMap,"fire");
    dbManager.saveOverlayMap(m_smokeMap, "smoke");
}
void MapController::mapFillScan() {

    if (m_fireMap->size() == 0) {
        return;
    }

    int min_ix = std::numeric_limits<int>::max();
    int max_ix = std::numeric_limits<int>::min();
    int min_iy = std::numeric_limits<int>::max();
    int max_iy = std::numeric_limits<int>::min();

    // Convert double coordinates to int grid points and find boundary
    QSet<QPair<int, int>> fireGridPoints;
    for (int i = 0; i < m_fireMap->size(); i++) {
        int ix = static_cast<int>(std::round(m_fireMap->at(i).first / delta));
        int iy = static_cast<int>(std::round(m_fireMap->at(i).second / delta));
        fireGridPoints.insert({ix, iy});
        if (ix < min_ix) min_ix = ix;
        if (ix > max_ix) max_ix = ix;
        if (iy < min_iy) min_iy = iy;
        if (iy > max_iy) max_iy = iy;
    }

    // No fires
    if (min_ix > max_ix || min_iy > max_iy) {
        return;
    }

    // Define a boundary for the BFS
    int grid_min_ix = min_ix - 1;
    int grid_max_ix = max_ix + 1;
    int grid_min_iy = min_iy - 1;
    int grid_max_iy = max_iy + 1;

    QSet<QPair<int, int>> reachable_from_outside;
    std::queue<QPair<int, int>> outer_q;

    // Helper function to add valid points to start BFS
    auto trySeedOuterQueue = [&](int ix, int iy) {
        QPair<int, int> point = {ix, iy};
        if (fireGridPoints.find(point) == fireGridPoints.end()) {
            if (reachable_from_outside.find(point) == reachable_from_outside.end()) {
                reachable_from_outside.insert(point);
                outer_q.push(point);
            }
        }
    };

    // Seed the BFS queue from the perimeter
    for (int ix = grid_min_ix; ix <= grid_max_ix; ++ix) {
        trySeedOuterQueue(ix, grid_min_iy);
        trySeedOuterQueue(ix, grid_max_iy);
    }
    for (int iy = grid_min_iy + 1; iy < grid_max_iy; ++iy) {
        trySeedOuterQueue(grid_min_ix, iy);
        trySeedOuterQueue(grid_max_ix, iy);
    }

    // Define neighbor for each direction
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    // Run the BFS to find all points reachable from the outer queue
    while (!outer_q.empty()) {
        QPair<int, int> current = outer_q.front();
        outer_q.pop();

        for (int i = 0; i < 4; ++i) {
            QPair<int, int> neighbor = {current.first + dx[i], current.second + dy[i]};
            // Boundary check
            if (neighbor.first < grid_min_ix || neighbor.first > grid_max_ix ||
                neighbor.second < grid_min_iy || neighbor.second > grid_max_iy) {
                continue;
            }
            // Explore neighbor only if it's not fire and not already visited
            if (fireGridPoints.find(neighbor) == fireGridPoints.end() &&
                reachable_from_outside.find(neighbor) == reachable_from_outside.end())
            {
                reachable_from_outside.insert(neighbor);
                outer_q.push(neighbor);
            }
        }
    }

    // Final check and insertion
    for (int ix = min_ix; ix <= max_ix; ++ix) {
        for (int iy = min_iy; iy <= max_iy; ++iy) {
            QPair<int, int> current_point = {ix, iy};

            // Check if inbounds
            // Check if in fire grid
            // Check if reachable
            if (fireGridPoints.find(current_point) == fireGridPoints.end() &&
                reachable_from_outside.find(current_point) == reachable_from_outside.end())
            {
                double real_x = static_cast<double>(ix) * delta;
                double real_y = static_cast<double>(iy) * delta;
                m_fireMap->insert({real_x, real_y});
            }
        }
    }
}

void MapController::droneDemo(){
    state++;
    if(state == 25){
        qDebug() << "beginning scan";
        mapFillScan();
    }
    if(state == 50){
        qDebug() << "beginning removal";
        double lat = 34.0591;
        double lon =  -117.82047;
        for(int i = 15; i < 20; i++){
            for(int j = 15; j < 20; j++){
                removeOverlayMarker({lat-delta*i,lon+delta*j});
            }
        }
        state = 0;
    }
}

//Prints all Dummy Drone Objects
// void MapController::debugPrintDrones() const {
//     qDebug() << "------ Drone Objects ------";
//     for (const DroneClass* drone : m_drones) {
//         qDebug() << "Drone Name:" << drone->getName()
//         << ", Latitude:" << drone->getLattitude()
//         << ", Longitude:" << drone->getLongitude();
//     }
//     qDebug() << "---------------------------";
// }
