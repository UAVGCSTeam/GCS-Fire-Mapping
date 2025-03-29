#ifndef MARKERLIST_H
#define MARKERLIST_H
#include <QAbstractListModel>
#include <QHash>
#include <QVariant>

class MarkerListModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit MarkerListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}
private:
    struct DataItem {
        QString type;
        time_t lastUpdated;
        double latitude;
        double longitude;
    };
    QList<DataItem> dataList;
};

#endif // MARKERLIST_H
