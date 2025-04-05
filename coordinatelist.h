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

    explicit CoordinateList(QObject* parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool insert(const QPair<double,double> &c);
    bool remove(const QPair<double,double> &c);
    bool refresh(QPair<double,double> &c);
    bool contains(const QPair<double,double> &c);
    QPair<double,double> at(int i) const;
    int size();

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
