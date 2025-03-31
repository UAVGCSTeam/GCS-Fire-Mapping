#ifndef MARKERCLASS_H
#define MARKERCLASS_H


#include <QObject>
#include <QVector2D>
#include <QString>

class MarkerClass : public QObject{
    Q_OBJECT
private:
    time_t m_lastUpdated;
    QString m_type;
    double m_latitude;
    double m_longitude;
    int m_index;
    bool m_visibility;
public:
    explicit MarkerClass(QObject *parent = nullptr):
        QObject(parent)
        , m_lastUpdated(time(0))
        , m_type("")
        , m_latitude(-1)
        , m_longitude(-1)
        , m_index(-1)
        , m_visibility(true)
    {};
    MarkerClass(const QString &input_type,
                const double &input_lat,
                const double &input_lon,
                QObject *parent = nullptr):
        QObject(parent)
        , m_lastUpdated(time(0))
        , m_type(input_type)
        , m_latitude(input_lat)
        , m_longitude(input_lon)
        , m_index(-1)
        , m_visibility(true)
    {};
    time_t getLastUpdated() const { return m_lastUpdated; };
    void setLastUpdated(const time_t &inputLastUpdated) {m_lastUpdated = inputLastUpdated;};
    void resetLastUpdated() {m_lastUpdated = time(0);};
    QString getType() const {return m_type;};
    void setType(const QString &inputType) {m_type = inputType;};
    double getLatitude() const {return m_latitude;};
    void setLatitude(const double &inputLatitude){m_latitude = inputLatitude;};
    double getLongitude() const {return m_longitude;};
    void setLongitude(const double &inputLongitude){m_longitude = inputLongitude;};;
    QVector2D getPosition() const {return QVector2D(m_latitude,m_longitude);};
    void setPosition(const double &inputLat,const double inputLon){
        setPosition(QVector2D(inputLat, inputLon));
    };
    void setPosition(const QVector2D &pos){
        m_latitude = pos[0];
        m_longitude = pos[1];
    };
    int getIndex() const {return m_index;};
    void setIndex(int inputIndex){m_index = inputIndex;};
    bool getVisibility() const {return m_visibility;};
    void setVisibility(bool inputVisibility){m_visibility = inputVisibility;};
};

#endif // MARKERCLASS_H
