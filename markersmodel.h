#ifndef MARKERSMODEL_H
#define MARKERSMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QStack>
#include "markerclass.h"

class MarkersModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum MarkerRoles {
        TypeRole = Qt::UserRole + 1,
        LatitudeRole,
        LongitudeRole,
        LastUpdatedRole,
        VisibilityRole
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
        }else if (role == VisibilityRole) {
            return qobject_cast<MarkerClass*>(obj)->getVisibility();
        }
        return QVariant();
    }
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[TypeRole] = "type";
        roles[LatitudeRole] = "latitude";
        roles[LongitudeRole] = "longitude";
        roles[LastUpdatedRole] = "lastUpdated";
        roles[VisibilityRole] = "visibility";
        return roles;
    }
    void addItem(QObject* item) {
        if(!openIndex.empty()){
            qobject_cast<MarkerClass*>(item)->setIndex(openIndex.top());
            replaceItem(item,openIndex.top());
            openIndex.pop();
        }else{
            qobject_cast<MarkerClass*>(item)->setIndex(m_markers.size());
            beginInsertRows(QModelIndex(), m_markers.count(), m_markers.count());
            m_markers.append(item);
            endInsertRows();
        }
    }
    void deleteItem(QObject* item){
        //does not remove item, save index for overwrite and hide/set
        int index = qobject_cast<MarkerClass*>(item)->getIndex();
        if (index == -1)
            return;
        qobject_cast<MarkerClass*>(item)->setType("deleted");
        qobject_cast<MarkerClass*>(item)->setVisibility(false);
        openIndex.push(index);
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
    void updateItem(QObject* item){
        int index = qobject_cast<MarkerClass*>(item)->getIndex();
        if (index == -1)
            return;
        qobject_cast<MarkerClass*>(item)->resetLastUpdated();
        m_markers[index] = item;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
    void refreshItem(QObject* item){
        int index = qobject_cast<MarkerClass*>(item)->getIndex();
        if (index == -1)
            return;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
    void replaceItem(QObject* item, int index){
        delete m_markers[index];
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
    QStack<int> openIndex;
};



#endif // MARKERSMODEL_H
