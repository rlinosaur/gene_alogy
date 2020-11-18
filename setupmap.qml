import QtQuick 2.14
import QtLocation 5.14
import QtPositioning 5.14


    Map {
        id: map
        anchors.fill: parent
        activeMapType: map.supportedMapTypes[0]
        zoomLevel: 6
        visible: true      
        center: QtPositioning.coordinate(64.5401,40.54330); //Архангельск
        //copyrightsVisible: false
        plugin: Plugin {
            name: 'osm';
            PluginParameter {
                name: 'osm.mapping.offline.directory'                
                value: 'offline_tiles/'
            }
            PluginParameter {
                name: 'osm.mapping.cache.directory'                
                value: 'cache/'
            }
            PluginParameter {
                name: 'osm.mapping.providersrepository.disabled'
                value: 'true'
            }
        }

        signal clickSignal(lat:double,lon:double);
        MouseArea
             { anchors.fill: parent
              onClicked: {

                  poi.coordinate=QtPositioning.coordinate(map.toCoordinate(Qt.point(mouse.x,mouse.y)).latitude,map.toCoordinate(Qt.point(mouse.x,mouse.y)).longitude);
                  clickSignal(poi.coordinate.latitude,poi.coordinate.longitude);
              }
             }
        MapQuickItem {
                        id: poi
                        objectName: "markCoord"
                        sourceItem: Rectangle { width: 21; height: 21; color: "#00ff00"; border.width: 2; border.color: "white"; smooth: true; radius: 10 }
                        coordinate {
                            latitude: 54.4079
                            longitude: 36.2473
                        }
                        opacity:1.0
                        anchorPoint: Qt.point(sourceItem.width/2, sourceItem.height/2)
                    }
        function setCenter(latitude,longitude)
        {
            center=QtPositioning.coordinate(latitude, longitude);
        }
        function setMarkerCoords(latitude, longitude)
        {
            poi.coordinate=QtPositioning.coordinate(latitude, longitude);
        }
    }

