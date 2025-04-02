#include "mapcontroller.h"
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
    // lat = 34.0585;
    // lon =  -117.821;
    // for(int i = 0; i < 10; i++){
    //     for(int j = 0; j < 10; j++){
    //         markerArray[i+j*10] = new MarkerClass("fireMarker",lat-inc*j,lon+inc*i,this);
    //     }
    // }
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
            //round coords to be divisible by 0.000035
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
//need removeDrone function
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

//Fills map holes encircled with fire
void MapController::mapFillScan(){

    if (m_markerHits.empty()) return;

    const double delta = 0.000035;

    // --- Step 1: Convert to Integer Grid Coordinates & Find Bounds ---
    std::unordered_set<QPair<double,double>,QPairHash,QPairEqual> presentGridPoints;
    int min_ix = std::numeric_limits<int>::max();
    int max_ix = std::numeric_limits<int>::min();
    int min_iy = std::numeric_limits<int>::max();
    int max_iy = std::numeric_limits<int>::min();

    //input fireHits into set
    for (const auto& p : m_markerHits) {
        //check for fire
        if(p.second){
            // converting to ints to make comparisons easier
            int ix = static_cast<int>(std::round(p.first.first / delta));
            int iy = static_cast<int>(std::round(p.first.second / delta));
            presentGridPoints.insert({ix, iy});

            if (ix < min_ix) min_ix = ix;
            if (ix > max_ix) max_ix = ix;
            if (iy < min_iy) min_iy = iy;
            if (iy > max_iy) max_iy = iy;
        }
    }

    // Define grid boundaries with a margin of 1 cell
    int grid_min_ix = min_ix - 1;
    int grid_max_ix = max_ix + 1;
    int grid_min_iy = min_iy - 1;
    int grid_max_iy = max_iy + 1;

    // --- Step 2: Flood Fill from the Boundary (BFS) ---
    std::queue<QPair<int,int>> q;
    std::unordered_set<QPair<double,double>,QPairHash,QPairEqual> visitedMissingPoints; // Stores missing points reachable from outside

    // Add all boundary points to the queue if they are MISSING
    for (int ix = grid_min_ix; ix <= grid_max_ix; ++ix) {
        QPair<int,int> top = {ix, grid_max_iy};
        QPair<int,int> bottom = {ix, grid_min_iy};
        if (presentGridPoints.find(top) == presentGridPoints.end()) { // If missing
            if (visitedMissingPoints.find(top) == visitedMissingPoints.end()) {
                q.push(top);
                visitedMissingPoints.insert(top);
            }
        }
        if (presentGridPoints.find(bottom) == presentGridPoints.end()) { // If missing
            if (visitedMissingPoints.find(bottom) == visitedMissingPoints.end()) {
                q.push(bottom);
                visitedMissingPoints.insert(bottom);
            }
        }
    }
    for (int iy = grid_min_iy + 1; iy < grid_max_iy; ++iy) { // Avoid corners twice
        QPair<int,int> left = {grid_min_ix, iy};
        QPair<int,int> right = {grid_max_ix, iy};
        if (presentGridPoints.find(left) == presentGridPoints.end()) { // If missing
            if (visitedMissingPoints.find(left) == visitedMissingPoints.end()) {
                q.push(left);
                visitedMissingPoints.insert(left);
            }
        }
        if (presentGridPoints.find(right) == presentGridPoints.end()) { // If missing
            if (visitedMissingPoints.find(right) == visitedMissingPoints.end()) {
                q.push(right);
                visitedMissingPoints.insert(right);
            }
        }
    }


    // BFS execution
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (!q.empty()) {
        QPair<int,int> current = q.front();
        q.pop();

        for (int i = 0; i < 4; ++i) {
            QPair<int,int> neighbor = {current.first + dx[i], current.second + dy[i]};

            // Stay within grid bounds
            if (neighbor.first < grid_min_ix || neighbor.first > grid_max_ix ||
                neighbor.second < grid_min_iy || neighbor.second > grid_max_iy) {
                continue;
            }

            // If the neighbor is MISSING and NOT VISITED yet
            if (presentGridPoints.find(neighbor) == presentGridPoints.end() &&
                visitedMissingPoints.find(neighbor) == visitedMissingPoints.end())
            {
                visitedMissingPoints.insert(neighbor);
                q.push(neighbor);
            }
        }
    }

    // --- Step 3: Identify Hole Points ---
    // Iterate through all possible grid points *inside* the boundary margin
    for (int ix = min_ix; ix <= max_ix; ++ix) {
        for (int iy = min_iy; iy <= max_iy; ++iy) {
            QPair<int,int> currentInt = {ix, iy};

            // If a point is MISSING from input AND was NOT visited by the boundary BFS
            if (presentGridPoints.find(currentInt) == presentGridPoints.end() &&
                visitedMissingPoints.find(currentInt) == visitedMissingPoints.end())
            {
                // This is a hole point. Convert back to double coordinates.
                double lat = static_cast<double>(ix) * delta;
                double lon = static_cast<double>(iy) * delta;
                MarkerClass* temp= new MarkerClass("fireMarker",lat,lon,parent());
                addMarker(temp);
            }
        }
    }
}

void MapController::droneDemo(){
    if(m_droneMarkersModel->size() == 0 || m_drones.size() == 0) return;

    // if(state == 0){
    //     for(int i = 0; i < 100; i++){
    //         addMarker(markerArray[i]);
    //     }
    //     state = 1;
    // }else if(state == 11){
    //     for(int i = 0; i < 100; i++){
    //         removeMarker(markerArray[i]);
    //     }
    //     double lat = 34.0585;
    //     double lon =  -117.821;
    //     double inc = 0.000035;
    //     for(int i = 0; i < 10; i++){
    //         for(int j = 0; j < 10; j++){
    //             markerArray[i+j*10] = new MarkerClass("fireMarker",lat-inc*j,lon+inc*i,this);
    //         }
    //     }
    //     state = -5;
    // }else{
    //     state++;
    // }

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
    if(state == 100){
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
