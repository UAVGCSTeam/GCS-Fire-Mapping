#ifndef MARKERCLASS_H
#define MARKERCLASS_H


#include <QObject>
#include <QPair>
#include <QVector2D>
#include <QString>

class MarkerClass : public QObject{
    Q_OBJECT
    Q_PROPERTY(time_t updated READ getUpdated WRITE setUpdated NOTIFY updatedChanged FINAL)
    Q_PROPERTY(QString type READ getType WRITE setType NOTIFY typeChanged FINAL)
    Q_PROPERTY(double latitude READ getLatitude WRITE setLatitude NOTIFY latitudeChanged FINAL)
    Q_PROPERTY(double longitude READ getLongitude WRITE setLongitude NOTIFY longitudeChanged FINAL)
private:
    time_t m_lastUpdated;
    QString m_type;
    double m_latitude;
    double m_longitude;
    int m_index;
public:
    explicit MarkerClass(QObject *parent = nullptr);
    MarkerClass(const QString &input_type,
                const double &input_lat,
                const double &input_long,
                int input_index,
                QObject *parent = nullptr);
    time_t getUpdated() const { return m_lastUpdated; };
    void setUpdated(const time_t &inputTime);
    QString getType() const {return m_type; };
    void setType(const QString &inputType);
    double getLatitude() const {return m_latitude; };
    void setLatitude(const double latitude);
    double getLongitude() const {return m_longitude; };
    void setLongitude(const double longitude);
    QVector2D getPosition() const {return QVector2D(m_latitude,m_longitude); };
    void setPosition(const QVector2D &pos);

    Q_INVOKABLE void setPosition(const double lat,const double lon);
signals:
    void updatedChanged();
    void typeChanged();
    void positionChanged();
    void latitudeChanged();
    void longitudeChanged();
};

#endif // MARKERCLASS_H
