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
{
    m_markersModel = new MarkersModel(this);
    // Populate with dummy drone objects for testing icon markers using setLattitude and setLongitude
    // DroneClass* drone1 = new DroneClass(this);
    // drone1->setName("Drone 1");
    // drone1->setLattitude(34.059174611493965);
    // drone1->setLongitude(-117.82051240067321);
    // addDrone(drone1);

    // DroneClass* drone2 = new DroneClass(this);
    // drone2->setName("Drone 2");
    // drone2->setLattitude(34.0600);
    // drone2->setLongitude(-117.8210);
    // addDrone(drone2);

    // DroneClass* drone3 = new DroneClass(this);
    // drone3->setName("Drone 3");
    // drone3->setLattitude(34.0615);
    // drone3->setLongitude(-117.8225);
    // addDrone(drone3);

    // DroneClass* drone4 = new DroneClass(this);
    // drone4->setName("Drone 4");
    // drone4->setLattitude(37.7749);
    // drone4->setLongitude(-122.4194);
    // addDrone(drone4);

    // DroneClass* drone5 = new DroneClass(this);
    // drone5->setName("Drone 5");
    // drone5->setLattitude(34.0119);
    // drone5->setLongitude(-118.4916);
    // addDrone(drone5);

    MarkerClass* drone1 = new MarkerClass("drone",34.059174611493965,-117.82051240067321,m_markersModel->size(),this);
    addMarker(drone1);
    MarkerClass* marker1 = new MarkerClass("fireMarker",34.05917,-117.82051,m_markersModel->size(),this);
    addMarker(marker1);
    MarkerClass* marker2 = new MarkerClass("fireMarker",34.05919,-117.82053,m_markersModel->size(),this);
    addMarker(marker2);
    MarkerClass* marker3 = new MarkerClass("fireMarker",34.05915,-117.82049,m_markersModel->size(),this);
    addMarker(marker3);
    MarkerClass* marker4 = new MarkerClass("fireMarker",34.05916,-117.82054,m_markersModel->size(),this);
    addMarker(marker4);
    MarkerClass* marker5 = new MarkerClass("fireMarker",34.05914,-117.82052,m_markersModel->size(),this);
    addMarker(marker5);

    //for demonstration
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

void MapController::setLocationMarking(const QVariant &lat, const QVariant &lon, const QString &type)
{
    addMarker(new MarkerClass(type,lat.toDouble(),lon.toDouble(),m_markersModel->size(),this));
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

void MapController::addMarker(MarkerClass* marker)
{
    if(marker){
        m_markersModel->addItem(marker);
        //emit locationMarked(marker);
    }
}
void MapController::updateMarker(MarkerClass* marker, int index){
    if(marker){
        m_markersModel->updateItem(marker,index);
        //emit markerUpdated(marker);
    }
}

void MapController::droneDemo(){
    if(m_markersModel->size() == 0) return;

    MarkerClass* drone = m_markersModel->at(0);

    // Define the center and radius for the circular path.
    const double centerLat = 34.05917;
    const double centerLon = -117.82051;
    const double radius = 0.001;  // This is an approximate degree offset
    // Increment the angle
    m_angle += 10; // increase by 10 degrees per update (adjust as needed)
    if (m_angle >= 360)
        m_angle -= 360;

    // Convert angle to radians
    double rad = qDegreesToRadians(m_angle);
    double newLat = centerLat + radius * qCos(rad);
    double newLon = centerLon + radius * qSin(rad);

    drone->setLatitude(newLat);
    drone->setLongitude(newLon);
    m_markersModel->updateItem(drone,0);

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
