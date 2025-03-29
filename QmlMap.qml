import QtQuick 2.15
import QtQuick.Controls
import Qt.labs.qmlmodels
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
        MapItemView
                {
                    model: mapController.markersModel()
                    delegate: markerDelegateChooser
                    DelegateChooser{
                        id: markerDelegateChooser
                        role: "type"
                        DelegateChoice{
                            id: droneDelegate
                            roleValue: "drone"
                            MapQuickItem{
                                coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
                                anchorPoint.x: markerImage.width / 2
                                anchorPoint.y: markerImage.height
                                sourceItem: Image {
                                    id: markerImage
                                    source: "qrc:/resources/droneMapIconSVG.svg"  // Make sure this path is correct, currently in the CMake as this path
                                    width: 50
                                    height: 50
                                }
                            }
                        }
                        DelegateChoice{
                            id: fireMarkerDelegate
                            roleValue: "fireMarker"
                            MapCircle{
                                center: QtPositioning.coordinate(model.latitude, model.longitude)
                                radius: 2.5
                                color: 'red'
                                opacity: 0.3
                            }
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
        // function onLocationMarked(marker) {
        //     markersModel.append({
        //         type: marker.type,
        //         lastUpdated: marker.lastUpdated,
        //         latitude: marker.latitude,
        //         longitude: marker.longitude,
        //     })
        // }
        //function onMarkerUpdated(marker){
        //}
        function onMapTypeChanged(index) {
            if (index < mapview.supportedMapTypes.length) {
                // Sets current maptype
                mapview.activeMapType = mapview.supportedMapTypes[index]
            }
        }
    }
    Component.onCompleted: {
        }
}
