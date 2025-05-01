#ifndef COORDINATELIST_H
#define COORDINATELIST_H
#include <QAbstractListModel>
#include <QHash>
#include <QDateTime>

// QAbstractList allows us to create a list for use in Qt while defining functions for ourselves in C++.
// CoordinateList is designed for fire and smoke marker use by combining QHash for deconflicting repeated
// coordinates with a Qlist which has functions required for QAbstractList to behave as expected on Qt side.
// Additionally the use a QHash allows for O(1) read/write in most cases
// Moved to backend so we can create and save using the data model directly
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

    explicit CoordinateList(QObject* parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool insert(const QPair<double,double> &c);
    bool insert(const QPair<double,double> &c, const QDateTime &t);
    bool remove(const QPair<double,double> &c);
    QDateTime get(const QPair<double,double> &c) const;
    QDateTime getAt(int i) const;
    QPair<double,double> at(int i) const;
    bool contains(const QPair<double,double> &c);
    int size() const;

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
