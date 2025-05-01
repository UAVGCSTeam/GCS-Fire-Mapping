import QtQuick 2.15
import QtLocation
import QtPositioning

Item
{
    id:mapwindow

    property double latitude: 34.059174611493965
    property double longitude: -117.82051240067321
    // Current map "types", dont have real control over map type in osm. Will keep for when we change map provider, just plug in.
    property var supportedMapTypes: [
        { name: "Street", type: Map.StreetMap },
        { name: "Satellite", type: Map.SatelliteMapDay },
        { name: "Terrain", type: Map.TerrainMap },
    ]
    property int currentMapTypeIndex: 0

    Plugin {
        id: mapPlugin
        name: "osm"
        // This refers to map type 'here' provides different map views as opposed to google maps and especially osm
        // We will want to change to here in the future, but it requires a paid API key and Token
        // name: "here"
        // PluginParameter { name: "here.app_id"; value: "GCS" }
        // PluginParameter { name: "here.token"; value: "sgISOwxxqF1JeBFaXKRbkHSsfHxsxWITKXCqeCkLP0A" }
    }

    Map
    {
        // Create actual Map Component
        // Reference id, not file name
        id:mapview
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(latitude,longitude)
        zoomLevel: 18
        // Handles clicking and dragging and zoom
        PinchHandler
        {
            target: null
            grabPermissions: PointerHandler.TakeOverForbidden
            property geoCoordinate startCenteroid
            onActiveChanged:
            {
                if (active)
                    startCenteroid = mapview.toCoordinate(centroid.position, false)
            }
            onScaleChanged: (delta) =>
                            {
                mapview.zoomLevel += Math.log(delta)
                mapview.alignCoordinateToPoint(startCenteroid, centroid.position)
            }
        }

        WheelHandler
        {
            onWheel: function(event)
            {
                const loc = mapview.toCoordinate(point.position)
                mapview.zoomLevel += event.angleDelta.y / 120;
                mapview.alignCoordinateToPoint(loc, point.position)
            }
        }

        DragHandler {
            target: null
            grabPermissions: PointerHandler.TakeOverForbidden
            onTranslationChanged: (delta) => { mapview.pan(-delta.x, -delta.y); }
        }

        MapItemView{
            id: smokeVisualLayer
            model: mapController ? mapController.smokeMap() : null
            opacity: 0.30
            layer.enabled: true
            visible: smokeMarkersItem.checked // Link visibility
            delegate: MapCircle{
                border.width: 0
                center: QtPositioning.coordinate(model.latitude, model.longitude)
                radius: 2.53
                color: 'black'
            }
        }
        MapItemView {
            id: smokeInteractionLayer
            model: mapController ? mapController.smokeMap() : null
            opacity: 0.8
            layer.enabled: true
            visible: smokeMarkersItem.checked
            delegate: MapQuickItem {
            coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
            anchorPoint.x: smokeInteractionItem.width / 2
            anchorPoint.y: smokeInteractionItem.height / 2
            sourceItem: Item {
                id: smokeInteractionItem
                width: 20
                height: 20
                Text {
                    id: smokeTooltip
                    visible: smokeMouseArea.containsMouse
                    x: parent.width / 2 - width / 2
                    y: -height - 5
                    text: model.lastUpdated ? new Date(model.lastUpdated).toLocaleString(Qt.locale(), Locale.ShortFormat) : "N/A"
                    font.pixelSize: 20; color: "black"; padding: 4
                }
                MouseArea {
                    id: smokeMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    }
                }
            }
        }

        MapItemView{
                     id: fireVisualLayer
                     model: mapController ? mapController.fireMap() : null
                     opacity: 0.3 // Opacity applied here
                     layer.enabled: true
                     visible: fireMarkersItem.checked // Link visibility
                     delegate: MapCircle{
                         border.width: 0
                         center: QtPositioning.coordinate(model.latitude, model.longitude)
                         radius: 2.53
                         color: 'red'
                     }
                }

        MapItemView {
            id: fireInteractionLayer
            model: mapController ? mapController.fireMap() : null
            opacity: 0.8
            layer.enabled: true
            visible: fireMarkersItem.checked
            delegate: MapQuickItem {
            coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
            anchorPoint.x: fireInteractionItem.width / 2
            anchorPoint.y: fireInteractionItem.height / 2
            sourceItem: Item {
                id: fireInteractionItem
                width: 20
                height: 20
                Text {
                    id: fireTooltip
                    visible: fireMouseArea.containsMouse
                    x: parent.width / 2 - width / 2
                    y: -height - 5
                    text: model.lastUpdated ? new Date(model.lastUpdated).toLocaleString(Qt.locale(), Locale.ShortFormat) : "N/A"
                    font.pixelSize: 20; color: "black"; padding: 4
                }
                MouseArea {
                    id: fireMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    }
                }
            }
        }

        MapItemView
        {
            // Create list for all pins (Will be used to track drones later with some optimization)
            model: ListModel { id: markersModel }
            delegate: MapQuickItem
            {
                coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
                anchorPoint.x: markerImage.width / 2
                anchorPoint.y: markerImage.height
                sourceItem: Image {
                    id: markerImage
                    source: "qrc:/resources/droneMapIconSVG.svg"  // Make sure this path is correct, currently in the CMake as this path
                    width: 100
                    height: 100
                }
            }
        }
    }

    /*
      These are our QML declarations of these functions, they occur once the signal is emitted from our cpp files.
      They listen for emit and then take the data that is emitted and uses it
    */
    Connections {
        target: mapController
        function onCenterPositionChanged(lat, lon) {
            mapview.center = QtPositioning.coordinate(lat, lon)
        }
        function onLocationMarked(lat, lon) {
            markersModel.append({"latitude": lat, "longitude": lon})
        }
        function onMapTypeChanged(index) {
            if (index < mapview.supportedMapTypes.length) {
                // Sets current maptype
                mapview.activeMapType = mapview.supportedMapTypes[index]
            }
        }
    }
    Component.onCompleted: {
            let drones = mapController.getAllDrones();
            markersModel.clear();
            for (let i = 0; i < drones.length; i++) {
                markersModel.append({
                    "name": drones[i].name,
                    "latitude": drones[i].latitude,
                    "longitude": drones[i].longitude
                });
            }
        }
}
