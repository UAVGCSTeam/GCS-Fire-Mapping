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
        MapItemView{
            model: mapController.markersModel()
            delegate: markerDelegateChooser
            opacity: 0.25
            layer.enabled: true
            DelegateChooser{
                id: markerDelegateChooser
                role: "type"
                DelegateChoice{
                    id: fireMarkerDelegate
                    roleValue: "fireMarker"
                    MapCircle{
                        border.width: 0
                        visible: visibility
                        center: QtPositioning.coordinate(model.latitude, model.longitude)
                        radius: 2.53
                        color: 'red'
                    }
                    //gradient attempt, needs massive optimization
                    // MapQuickItem {
                    //     id: gradientItem
                    //     visible: visibility
                    //     coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
                    //     anchorPoint.x: canvas.width / 2
                    //     anchorPoint.y: canvas.height / 2
                    //     sourceItem: Canvas {
                    //         id: canvas
                    //         property real baseSize: 20
                    //         property real baseZoom: 18
                    //         property real scaleFactor: Math.pow(2, mapview.zoomLevel - baseZoom)
                    //         width: baseSize * scaleFactor
                    //         height: baseSize * scaleFactor
                    //         onPaint: {
                    //             var ctx = getContext("2d");
                    //             ctx.clearRect(0, 0, width, height);
                    //             var grad = ctx.createRadialGradient(width/2, height/2, 0, width/2, height/2, width/2);
                    //             grad.addColorStop(0, "red");
                    //             grad.addColorStop(1, "transparent");
                    //             ctx.fillStyle = grad;
                    //             ctx.fillRect(0, 0, width, height);
                    //         }
                    //         onWidthChanged: requestPaint()
                    //         onHeightChanged: requestPaint()
                    //         Connections {
                    //             target: mapview
                    //             function onZoomLevelChanged(){
                    //                 canvas.requestPaint()
                    //             }
                    //         }
                    //     }
                    // }
                }
                DelegateChoice{
                    id: smokeMarkerDelegate
                    roleValue: "smokeMarker"
                    MapCircle{
                        visible: visibility
                        border.width: 0
                        center: QtPositioning.coordinate(model.latitude, model.longitude)
                        radius: 2.53
                        color: 'black'
                    }
                    // MapRectangle{
                    //     visible: visibility
                    //     color: 'black'
                    //     topLeft: QtPositioning.coordinate(model.latitude+0.000018, model.longitude-0.000018)
                    //     bottomRight: QtPositioning.coordinate(model.latitude-0.000018, model.longitude+0.000018)
                    // }
                }
            }
        }
        MapItemView{
            //may want to have a seperate list for drones in the future
            model: mapController.markersModel()
            delegate: droneDelegateChooser
            DelegateChooser{
                id: droneDelegateChooser
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
