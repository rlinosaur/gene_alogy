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

        signal markerClick(msg: string)        

        function clearAllMarkers()
        {
            map.clearMapItems();
        }       

        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
        }

        signal level(msg: string);
        onVisibleRegionChanged:
        {
            var lat = map.toCoordinate(Qt.point(mouse.x,mouse.y)).latitude
            var lon = map.toCoordinate(Qt.point(mouse.x,mouse.y)).longitude
            level(zoomLevel +";"+ lat +";"+ lon)
            addMarker("Marker ", 64.5401,40.54330);
            addMarker("Marker 2", 40.54330, 64.5401);

        }


        function addMarker(mName, mLon, mLat)
                {

                    console.log('Добавляем объект на карту ');

                    var comp
                    var marker

                    comp = Qt.createComponent("qrc:/marker.qml")
                    if (comp.status === Component.Ready)
                    {
                        marker = comp.createObject(map)
                        marker.name=mName;
                        //marker.status=devStatus;
                        marker.setCoordinates(mLat,mLon)
                        marker.setRandomColor();
                        marker.setRandomCoordinates();
                        map.addMapItem(marker);
                        if (marker === null)
                        {
                            console.log("Error creating marker mobject");
                        }
                        console.log("Я так понимаю, что объект добавлен..где вид его? почему не видим его?");
                    }
                    else if (comp.status === Component.Error)
                    {
                        console.log("Error loading qml component marker.qml:", comp.errorString());
                    }
                }

    }

