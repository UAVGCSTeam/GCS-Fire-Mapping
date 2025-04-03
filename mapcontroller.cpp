#include "mapcontroller.h"
#include <unordered_set>
#include <queue>
#include <QDebug>

/*
 * Used to emit signals to our QML functions.
 * Keeps logic in cpp.
 * https://doc.qt.io/qt-6/signalsandslots.html
*/

// Define constructor for MapController class
MapController::MapController(QObject *parent)
    // Defines all variables within our map
    : QObject(parent)
    , m_currentMapType(0)
    , m_supportedMapTypesCount(3)
    , m_droneTimer(new QTimer(this))
    , m_angle(0)
    , m_markersModel(new MarkersModel(this))
    , m_droneMarkersModel(new MarkersModel(this))
{
    //everything in this block is for demo purposes
    // Populate with dummy drone objects for demo
    //uses markers for simplicity but drones should normally use droneClass and addDrone
    DroneClass* drone1 = new DroneClass(this);
    addDrone(drone1);
    DroneClass* drone2 = new DroneClass(this);
    addDrone(drone2);
    DroneClass* drone3 = new DroneClass(this);
    addDrone(drone3);
    //populate with fire and smoke markers, unaligned
    double lat = 34.0591;
    double lon =  -117.82047;
    double inc = 0.000035;
    for(int i = 0; i < 32; i++){
        for(int j = 0; j < 32; j++){
            if(i < 15 || i > 20 || j < 15 || j > 20){
                MarkerClass* temp = new MarkerClass("fireMarker",lat-inc*j,lon+inc*i,this);
                addMarker(temp,true);
            }
        }
    }
    lat = 34.06;
    lon =  -117.821;
    for(int i = 0; i < 32; i++){
        for(int j = 0; j < 32; j++){
            MarkerClass* temp = new MarkerClass("smokeMarker",lat-inc*j,lon+inc*i,this);
            addMarker(temp,true);
        }
    }
    //for time to flow in demo
    connect(m_droneTimer, &QTimer::timeout, this, &MapController::droneDemo);
    m_droneTimer->start(100);
}

void MapController::addDrone(DroneClass* drone)
{
    if (drone) {
        m_drones.append(drone);
        m_droneMarkersModel->addItem(drone->getMarker());
    }
}

// QVariantList MapController::getAllDrones() const
// {
//     QVariantList droneList;
//     for (const DroneClass* drone : m_drones) {
//         QVariantMap droneData;
//         droneData["name"] = drone->getName();
//         droneData["latitude"] = drone->getLatitude();
//         droneData["longitude"] = drone->getLongitude();
//         droneList.append(droneData);
//     }
//     return droneList;
// }

void MapController::createDrone(const QString &input_name){
    DroneClass* temp = new DroneClass(this);
    temp->setName(input_name);
    //can run updateMarker with coordinates if desired
    // temp->setLattitude(34.06152);
    // temp->setLongitude(-117.82254);
    addDrone(temp);
}
//for use with markerHit table, could be defined elsewhere
void MapController::roundCoords(MarkerClass* marker){
    //0.000035 is distance markers should be from each other
    double s = 0.000035;
    marker->setLatitude(round(marker->getLatitude()/s) * s);
    marker->setLongitude(round(marker->getLongitude()/s) * s);
}

void MapController::setCenterPosition(const QVariant &lat, const QVariant &lon)
{
    QPair<double, double> newCenter(lat.toDouble(), lon.toDouble());
    // updateCenter below
    updateCenter(newCenter);
}

void MapController::setLocationMarking(const QVariant &lat, const QVariant &lon, const QString &type)
{
    MarkerClass* temp = new MarkerClass(type,lat.toDouble(),lon.toDouble(),this);
    addMarker(temp);
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

//adds marker to m_markersModel
//if hitDeconflict is true then it will round the coordinates and
//check if the coordinates have been received before in hash table
void MapController::addMarker(MarkerClass* marker, bool hitDeconflictEnabled)
{
    if(marker){
        int type = 0;
        if(marker->getType() == "drone")
            return;
        else if(marker->getType() == "fireMarker")
            type = 2;
        else if(marker->getType() == "smokeMarker")
            type = 1;
        marker->setVisibility(type & m_vis);
        if(hitDeconflictEnabled){
            // Round coords to be divisible by 0.000035
            roundCoords(marker);
            //ignore repeat hits
            QPair<double,double> temp(marker->getLatitude(),marker->getLongitude());
            if(m_markerHits.find(temp) == m_markerHits.end()){
                if(type == 2) m_markerHits[temp] = 1;
                else m_markerHits[temp] = 0;
                m_markersModel->addItem(marker);
            }else if(m_markerHits[temp] < 1 && marker->getType() == "fireMarker"){
                m_markerHits[temp] = 1;
                m_markersModel->addItem(marker);
            }
        }else{
            m_markersModel->addItem(marker);
        }
    }
}
void MapController::removeMarker(MarkerClass* marker, bool hitDeconflictRecover){
    if(marker){
        if(marker->getType() == "drone")
            return;
        if(hitDeconflictRecover){
            QPair<double,double> temp(marker->getLatitude(),marker->getLongitude());
            m_markerHits.erase(temp);
            m_markersModel->deleteItem(marker);
        }else{
            m_markersModel->deleteItem(marker);
        }

    }
}
// need removeDrone function!
void MapController::updateMarker(MarkerClass* marker, const double &lat, const double &lon){
    if(marker){
        if(marker->getType() == "drone")
            return;
        marker->setLatitude(lat);
        marker->setLongitude(lon);
        m_markersModel->updateItem(marker);
    }
}
void MapController::updateDrone(DroneClass* drone, const double &lat, const double &lon){
    if(drone){
        MarkerClass* marker = drone->getMarker();
        marker->setLatitude(lat);
        marker->setLongitude(lon);
        m_droneMarkersModel->updateItem(marker);
    }
}
void MapController::toggleTypeVisibility(const QString &type, bool vis){
    if(type == "fireMarker") m_vis = m_vis ^ 2;
    if(type == "smokemarker") m_vis = m_vis ^ 1;
    for(int i = 0; i < m_markersModel->size(); i++){
        if(m_markersModel->at(i)->getType() == type){
            m_markersModel->at(i)->setVisibility(vis);
            m_markersModel->refreshItem(m_markersModel->at(i));
        }
    }
}
// Fills map holes encircled with fire
void MapController::mapFillScan() {
    if (m_markerHits.empty()) return;

    const double delta = 0.000035;
    int min_ix = std::numeric_limits<int>::max();
    int max_ix = std::numeric_limits<int>::min();
    int min_iy = std::numeric_limits<int>::max();
    int max_iy = std::numeric_limits<int>::min();

    // Convert double coordinates to int
    std::unordered_set<QPair<int, int>, IntPairHash, IntPairEqual> fireGridPoints;
    for (const auto& p : m_markerHits) {
        int ix = static_cast<int>(std::round(p.first.first / delta));
        int iy = static_cast<int>(std::round(p.first.second / delta));
        fireGridPoints.insert({ix, iy});

        if (ix < min_ix) min_ix = ix;
        if (ix > max_ix) max_ix = ix;
        if (iy < min_iy) min_iy = iy;
        if (iy > max_iy) max_iy = iy;
    }
    // No fires
    if (min_ix > max_ix || min_iy > max_iy) return;

    // Check each direction next to fire markers and add missing points into potentials
    std::unordered_set<QPair<int, int>, IntPairHash, IntPairEqual> potentialPoints;
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};
    for (const auto& firePt : fireGridPoints) {
        for (int i = 0; i < 4; ++i) {
            QPair<int, int> neighbor = {firePt.first + dx[i], firePt.second + dy[i]};
            if (fireGridPoints.find(neighbor) == fireGridPoints.end()) {
                // Check boundaries for efficiency
                if (neighbor.first >= min_ix && neighbor.first <= max_ix &&
                    neighbor.second >= min_iy && neighbor.second <= max_iy)
                {
                    potentialPoints.insert(neighbor);
                }
            }
        }
    }
    // Nothing to fill
    if (potentialPoints.empty()) return;

    // Flood fill BFS
    std::queue<QPair<int, int>> outer_q;
    std::unordered_set<QPair<int, int>, IntPairHash, IntPairEqual> visited;

    int grid_min_ix = min_ix - 1;
    int grid_max_ix = max_ix + 1;
    int grid_min_iy = min_iy - 1;
    int grid_max_iy = max_iy + 1;

    auto tryAddToOuterQueue = [&](int ix, int iy) {
        QPair<int, int> point = {ix, iy};
        if (fireGridPoints.find(point) == fireGridPoints.end() &&
            visited.find(point) == visited.end()) {
            visited.insert(point);
            outer_q.push(point);
        }
    };

    for (int ix = grid_min_ix; ix <= grid_max_ix; ++ix) {
        tryAddToOuterQueue(ix, grid_max_iy);
        tryAddToOuterQueue(ix, grid_min_iy);
    }
    for (int iy = grid_min_iy + 1; iy < grid_max_iy; ++iy) {
        tryAddToOuterQueue(grid_min_ix, iy);
        tryAddToOuterQueue(grid_max_ix, iy);
    }

    // BFS for 'shoreline' empty coordinates
    while (!outer_q.empty()) {
        QPair<int, int> current = outer_q.front();
        outer_q.pop();

        for (int i = 0; i < 4; ++i) {
            QPair<int, int> neighbor = {current.first + dx[i], current.second + dy[i]};

            if (neighbor.first < grid_min_ix || neighbor.first > grid_max_ix ||
                neighbor.second < grid_min_iy || neighbor.second > grid_max_iy) {
                continue;
            }

            if (fireGridPoints.find(neighbor) == fireGridPoints.end() &&
                visited.find(neighbor) == visited.end())
            {
                visited.insert(neighbor);
                outer_q.push(neighbor);
            }
        }
    }

    // BFS for remaining coordinates
    std::unordered_set<QPair<int, int>, IntPairHash, IntPairEqual> filled;

    for (const auto& candidate : potentialPoints) {

        if (filled.find(candidate) == filled.end() &&
            filled.find(candidate) == filled.end())
        {
            // Begin Fill.
            std::queue<QPair<int, int>> fill_q;
            fill_q.push(candidate);

            while (!fill_q.empty()) {
                QPair<int, int> current = fill_q.front();
                fill_q.pop();

                for (int i = 0; i < 4; ++i) {
                    QPair<int, int> neighbor = {current.first + dx[i], current.second + dy[i]};

                    // Check if inbounds
                    // Check if in fire grid
                    // Check if reachable
                    // Check if filled
                    if (neighbor.first >= min_ix && neighbor.first <= max_ix &&
                        neighbor.second >= min_iy && neighbor.second <= max_iy &&
                        fireGridPoints.find(neighbor) == fireGridPoints.end() &&
                        visited.find(neighbor) == visited.end() &&
                        filled.find(neighbor) == filled.end())
                    {
                        filled.insert(neighbor);
                        double lat = static_cast<double>(neighbor.first) * delta;
                        double lon = static_cast<double>(neighbor.second) * delta;
                        addMarker(new MarkerClass("fireMarker", lat, lon, parent()));
                        fill_q.push(neighbor);
                    }
                }
            }
        }
    }
}

void MapController::droneDemo(){
    if(m_droneMarkersModel->size() == 0 || m_drones.size() == 0) return;

    // Define the center and radius for the circular path.
    const double centerLat = 34.05917;
    const double centerLon = -117.82051;
    const double centerLat2 = 34.06;
    const double centerLon2 = -117.821;
    const double centerLat3 = 34.0615;
    const double centerLon3 = -117.8225;
    const double radius = 0.0003;  // This is an approximate degree offset

    // Increment the angle
    m_angle += 10; // increase by 10 degrees per update (adjust as needed)
    if (m_angle >= 360)
        m_angle -= 360;
    // Convert angle to radians
    double rad = qDegreesToRadians(m_angle);
    double newLat = centerLat + radius * qCos(rad);
    double newLon = centerLon + radius * qSin(rad);
    double newLat2 = centerLat2 + radius * qCos(-rad);
    double newLon2 = centerLon2 + radius * qSin(-rad);
    double newLat3 = centerLat3 + radius * qCos(rad);
    double newLon3 = centerLon3 + radius * qSin(rad);


    updateDrone(m_drones[0],newLat,newLon);
    updateDrone(m_drones[1],newLat2,newLon2);
    updateDrone(m_drones[2],newLat3,newLon3);
    state++;
    if(state == 50){
        mapFillScan();
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
