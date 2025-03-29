#ifndef MARKERSMODEL_H
#define MARKERSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QList>
#include "markerclass.h"

class MarkersModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum MarkerRoles {
        TypeRole = Qt::UserRole + 1,
        LatitudeRole,
        LongitudeRole,
        LastUpdatedRole
        //need something to pull drone data, maybe pointer
    };

    explicit MarkersModel(QObject* parent = nullptr)
        : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_markers.count();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() < 0 || index.row() >= m_markers.count())
            return QVariant();

        QObject* obj = m_markers.at(index.row());
        if (role == TypeRole) {
            return qobject_cast<MarkerClass*>(obj)->getType();
        } else if (role == LatitudeRole) {
                return qobject_cast<MarkerClass*>(obj)->getLatitude();
        } else if (role == LongitudeRole) {
                return qobject_cast<MarkerClass*>(obj)->getLongitude();
        }
        return QVariant();
    }

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[TypeRole] = "type";
        roles[LatitudeRole] = "latitude";
        roles[LongitudeRole] = "longitude";
        roles[LastUpdatedRole] = "lastUpdated";
        return roles;
    }

    void addItem(QObject* item) {
        beginInsertRows(QModelIndex(), m_markers.count(), m_markers.count());
        m_markers.append(item);
        endInsertRows();
    }

    void updateItem(QObject* item, int index){
        if (index < 0 || index >= m_markers.size())
            return;
        m_markers[index] = item;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }

    MarkerClass* at(int index) const {
        if (index < 0 || index >= m_markers.size())
            return nullptr;
        return qobject_cast<MarkerClass*>(m_markers.at(index));
    }
    int size(){return m_markers.size();}

private:
    QList<QObject*> m_markers;
};

#endif // MARKERSMODEL_H
