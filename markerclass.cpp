#include "markerclass.h"


MarkerClass::MarkerClass(QObject *parent) :
    QObject(parent)
    , m_lastUpdated(time(0))
    , m_type("")
    , m_latitude(-1)
    , m_longitude(-1)
    , m_index(-1)
{

}

MarkerClass::MarkerClass(const QString &input_type,
                         const double &input_lat,
                         const double &input_lon,
                         int index,
                         QObject *parent) :
    QObject(parent)
    , m_lastUpdated(time(0))
    , m_type(input_type)
    , m_latitude(input_lat)
    , m_longitude(input_lon)
    , m_index(index)
{

}


void MarkerClass::setUpdated(const time_t &inputTime){
    if (m_lastUpdated != inputTime){
        m_lastUpdated = inputTime;
        emit updatedChanged();
    }
}
void MarkerClass::setType(const QString &inputRole){
    if (m_type != inputRole){
        m_type = inputRole;
        emit typeChanged();
    }
}
void MarkerClass::setPosition(const QVector2D &pos){
    if (pos[0] != m_latitude || pos[1] != m_longitude ) {
        m_latitude = pos[0];
        m_longitude = pos[1];
        emit positionChanged();
    }
}
void MarkerClass::setPosition(const double lat, const double lon){
    setPosition(QVector2D(lat, lon));
}
void MarkerClass::setLatitude(const double lat) {
    if (m_latitude != lat) {
        m_latitude = lat;
        emit latitudeChanged();
    }
}
void MarkerClass::setLongitude(const double lon) {
    if (m_longitude != lon) {
        m_longitude = lon;
        emit longitudeChanged();
    }
}
