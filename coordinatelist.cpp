#include "coordinatelist.h"

CoordinateList::CoordinateList(QObject* parent)
    : QAbstractListModel(parent) {}

// Required for QAbstractList to be implemented
// Must correspond to number of entries
int CoordinateList::rowCount(const QModelIndex &parent) const {
    return parent.isValid()? 0 : m_list.count();
}
// Required for QAbstractList to be implemented
// Must be able to take an index and role and return the expected data
// The role uses an ENUM for clarity but is otherwise not required
// QList makes this very easy because index.row() will just be the index
QVariant CoordinateList::data(const QModelIndex &index, int role) const {
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
// Required for QAbstractList to be implemented with user defined roles
// Must return a QHash, with an int for a key and its value as a string
// converted to a QByteArray, can be done implicitly.
// For our purposes the key/value should line up with the ENUM and
// the data function above. The string is used in Qt like a data member.
QHash<int, QByteArray> CoordinateList::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[LatitudeRole] = "latitude";
    roles[LongitudeRole] = "longitude";
    roles[LastUpdatedRole] = "lastUpdated";
    return roles;
}
// Combines update and insert
// Input a coordinate and either update its LastUpdated or
// add it to the map.
bool CoordinateList::insert(const QPair<double,double> &c){
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
    m_hash[c] = index;
    endInsertRows();
    return true;
}
// Removes a coordinate
// Uses Swap&Pop for performant removal
bool CoordinateList::remove(const QPair<double,double> &c){

    int index = m_hash.value(c, -1);
    if (index == -1) {
        return false;
    }
    int end = m_list.count() - 1;
    if (index != end) {
        const QPair<double, double> &key = m_list.last().coordinate;
        m_hash[key] = index;
        m_list.swapItemsAt(index, end);
        QModelIndex modelIndex = this->index(index);
        emit dataChanged(modelIndex, modelIndex);
    }
    beginRemoveRows(QModelIndex(), end, end);
    m_list.removeLast();
    m_hash.remove(c);
    endRemoveRows();
    return true;
}
// Gets lastUpdated from input coordinate. For future use
QDateTime CoordinateList::get(const QPair<double,double> &c) const{
    int index = m_hash.value(c, -1);
    if (index == -1) {
        return QDateTime();
    }
    return m_list.at(index).lastUpdated;
}
// Gets coordinates from index, primarily for iteration in mapController::mapFillScan()
QPair<double,double> CoordinateList::at(int i) const{
    if (i >= 0 && i < m_list.count()) {
        return m_list.at(i).coordinate;
    } else {
        qWarning() << "IndexedListModel::at() - Index" << i << "out of bounds (count:" << m_list.count() << ")";
        return QPair<double, double>(); // Default constructor gives (0.0, 0.0)
    }
}
bool CoordinateList::contains(const QPair<double,double> &c){
    return m_hash.contains(c);
}
int CoordinateList::size(){
    return m_list.count();
}
