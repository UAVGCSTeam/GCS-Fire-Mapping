#ifndef MARKERSMODEL_H
#define MARKERSMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QStack>
#include "markerclass.h"


//QML requires an QAbstractListModel if we are to expose a c++ data structure to our QmlMap.qml
//We can format this list as needed so it can be used in ListView while maintaining access properties of an array
//Current implementation only works with MarkersClass*, but QObject* is still used for conveinence
class MarkersModel : public QAbstractListModel
{
    Q_OBJECT
public:
    //simply for readability in data
    enum MarkerRoles {
        TypeRole = Qt::UserRole + 1,
        LatitudeRole,
        LongitudeRole,
        LastUpdatedRole,
        VisibilityRole
    };

    explicit MarkersModel(QObject* parent = nullptr)
        : QAbstractListModel(parent) {}

    //Required for QML to parse the list
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_markers.count();
    }
    //Required for QML to parse the list, returns data member based on role/member variable
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
    //hashes string into bytearray for faster look up speed
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[TypeRole] = "type";
        roles[LatitudeRole] = "latitude";
        roles[LongitudeRole] = "longitude";
        roles[LastUpdatedRole] = "lastUpdated";
        roles[VisibilityRole] = "visibility";
        return roles;
    }
    //check for open index to replace otherwise add
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
    //marks for replacement, placing its index on the stack and then hides
    void deleteItem(QObject* item){
        int index = qobject_cast<MarkerClass*>(item)->getIndex();
        if (index == -1)
            return;
        qobject_cast<MarkerClass*>(item)->setType("deleted");
        qobject_cast<MarkerClass*>(item)->setVisibility(false);
        openIndex.push(index);
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
    //sets lastUpdated to current time, otherwise signals data change at a specific index
    void updateItem(QObject* item){
        int index = qobject_cast<MarkerClass*>(item)->getIndex();
        if (index == -1)
            return;
        qobject_cast<MarkerClass*>(item)->resetLastUpdated();
        m_markers[index] = item;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
    //signals data change without updating current time
    void refreshItem(QObject* item){
        int index = qobject_cast<MarkerClass*>(item)->getIndex();
        if (index == -1)
            return;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
    //deletes overwritten before assigning pointer to index
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
    //holds the markers!
    QList<QObject*> m_markers;
    //used for tracking deleted members
    QStack<int> openIndex;
};



#endif // MARKERSMODEL_H
