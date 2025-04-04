#ifndef COORDINATELIST_H
#define COORDINATELIST_H
#include <QAbstractListModel>
#include <QHash>
#include <QDateTime>


class CoordinateList : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CoordinateRoles {
        LatitudeRole = Qt::UserRole + 1,
        LongitudeRole,
        LastUpdatedRole
    };
    Q_ENUM(CoordinateRoles)

    explicit CoordinateList(QObject* parent = nullptr)
        : QAbstractListModel(parent) {}
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return parent.isValid()? 0 : m_list.count();
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_list.count() || index.row() < 0)
            return QVariant();
        const CoordinateData& item = m_list.at(index.row());
        switch (role) {
        case LatitudeRole:
            return item.coordinate.first;
        case LongitudeRole:
            return item.coordinate.second;
        case LastUpdatedRole:
            return item.lastUpdated;
        default:
            break;
        }
        return QVariant();
    }
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[LatitudeRole] = "latitude";
        roles[LongitudeRole] = "longitude";
        roles[LastUpdatedRole] = "lastUpdated";
        return roles;
    }
    bool insert(const QPair<double,double> &c){
        int index = m_hash.value(c, -1);
        if (index != -1) {
            m_list[index].lastUpdated = QDateTime::currentDateTime();
            QModelIndex modelIndex = this->index(index);
            emit dataChanged(modelIndex, modelIndex, {LastUpdatedRole});
            return false;
        }
        index = m_list.count();
        beginInsertRows(QModelIndex(), index, index);
        m_list.append(CoordinateData(c, QDateTime::currentDateTime()));
        m_hash.insert(c, index);
        endInsertRows();
        return true;
    }
    bool remove(const QPair<double,double> &c){
        int index = m_hash.value(c, -1);
        if (index == -1) {
            return false;
        }
        int end = m_list.count() - 1;
        beginRemoveRows(QModelIndex(), index, index);
        if (index == end) {
            m_list.removeLast();
            m_hash.remove(c);
        } else {
            const QPair<double, double>& key = m_list.last().coordinate;
            m_list.swapItemsAt(index, end);
            m_list.removeLast();
            m_hash.remove(c);
            m_hash[key] = index;
        }
        endRemoveRows();
        return true;
    }
    bool refresh(QPair<double,double> &c){
        int index = m_hash.value(c, -1);
        if (index == -1) {
            return false;
        }
        QModelIndex modelIndex = this->index(index);
        emit dataChanged(modelIndex, modelIndex);
        return true;
    }
    bool contains(const QPair<double,double> &c){
        return m_hash.contains(c);
    }
    QPair<double,double> at(int i) const{
        if (i >= 0 && i < m_list.count()) {
            return m_list.at(i).coordinate;
        } else {
            qWarning() << "IndexedListModel::at() - Index" << i << "out of bounds (count:" << m_list.count() << ")";
            return QPair<double, double>(); // Default constructor gives (0.0, 0.0)
        }
    }
    int size(){return m_list.count();}

private:
    struct CoordinateData {
        QPair<double, double> coordinate;
        QDateTime lastUpdated;

        CoordinateData() = default;
        CoordinateData(const QPair<double, double>& coord, const QDateTime& dt)
            : coordinate(coord), lastUpdated(dt) {}
    };
    QList<CoordinateData> m_list;
    QHash<QPair<double, double>, int> m_hash;
};

#endif // COORDINATELIST_H
