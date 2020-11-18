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

        signal placeClick(msg: string)

        function clearAllMarkers()
        {
            map.clearMapItems();
        }       
/*
        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
        }
  */

        signal mapClick(lat:double,lon:double);
        MouseArea
             {
                 id:mouse
                 anchors.fill: parent
                 hoverEnabled: true
                 onClicked:
                 {
                   //  console.log("map click clack");
                    mapClick(map.toCoordinate(Qt.point(mouse.x,mouse.y)).latitude,map.toCoordinate(Qt.point(mouse.x,mouse.y)).longitude);
                }
             }


        signal level(msg: string);
        onVisibleRegionChanged:
        {
            var lat = map.toCoordinate(Qt.point(mouse.x,mouse.y)).latitude
            var lon = map.toCoordinate(Qt.point(mouse.x,mouse.y)).longitude
            level(zoomLevel +";"+ lat +";"+ lon)
            //addMarker("Marker ", 64.5401,40.54330);
            //addMarker("Marker 2", 40.54330, 64.5401);

        }

        function addPlace(plUuid, plLat, plLon, plName, plANames)
        {
            var comp
            var marker
            comp = Qt.createComponent("qrc:/marker.qml")
            if (comp.status === Component.Ready)
            {
                marker = comp.createObject(map)
                marker.uuid=plUuid;
                marker.name=plName;
                marker.setCoordinates(plLat,plLon)
                marker.setRandomColor();
                map.addMapItem(marker);
                if (marker === null)
                {
                    console.log("Error creating placeMarker");
                }
            }
            else if (comp.status === Component.Error)
            {
                console.log("Error loading qml component marker.qml:", comp.errorString());
            }
        }

        function findPlace(searchUuid)
        {
            var ourPlace = undefined;
            for(var childItem in map.mapItems)
            {
                if(map.mapItems[childItem].uuid===searchUuid) return map.mapItems[childItem];
            }
            return ourPlace;
        }

        function changePlace(plUuid,plLat,plLon, plName, plANames)
        {
            console.log('searching for '+plUuid);
            var ourPlace=findPlace(plUuid);
            console.log("result .."+ourPlace);
            if(ourPlace!==undefined)
            {
                ourPlace.name=plName;
                ourPlace.anNames=plANames;
                ourPlace.setCoordinates(plLat,plLon);
                ourPlace.setRandomColor();
            }
            else
            {
                console.log('Place not found');
            }
        }
    }

