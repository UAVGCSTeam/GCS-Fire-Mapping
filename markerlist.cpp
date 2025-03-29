#include "markerlist.h"



class MarkerListModel : public QAbstractListModel {
    Q_OBJECT


public:
    enum MarkerRoles {
        TypeRole = Qt::UserRole + 1,
        LastUpdatedRole,
        LatitudeRole,
        LongitudeRole
    };


    int rowCount(const QModelIndex &parent) const override {
        // For simple list, parent is not used
        Q_UNUSED(parent);
        return dataList.size();
    }


    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= dataList.size())
            return QVariant();


        const auto &item = dataList.at(index.row());
        switch (role) {
        case TypeRole:
            return item.type;
        case LastUpdatedRole:
            return item.lastUpdated;
        case LatitudeRole:
            return item.latitude;
        case LongitudeRole:
            return item.longitude;
        default:
            return QVariant();
        }
    }


    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[TypeRole] = "type";
        roles[LastUpdatedRole] = "lastUpdated";
        roles[LatitudeRole] = "latitude";
        roles[LongitudeRole] = "longitude";
        return roles;
    }


    void addData(const QString &type, time_t &lastUpdated, double &latitude, double &longitude) {
        beginInsertRows(QModelIndex(), dataList.size(), dataList.size());
        dataList.append({type, lastUpdated, latitude, longitude});
        endInsertRows();
    }

private:
};
