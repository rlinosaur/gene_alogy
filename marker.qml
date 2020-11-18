import QtQuick 2.14
import QtLocation 5.14
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14
import QtPositioning 5.14
//import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3

MapQuickItem
{
    id: marker
    objectName: "placeMark"
    property string uuid
    property string name
    property string anNames
    property bool visibility:true
    visible: visibility
    autoFadeIn:false;    
    sourceItem:
        Image {
                anchors.centerIn: parent
                id: image
                property string color:"lightgray"//цвет по умолчанию, ага.
                source: "qrc:/marker.svg"                
                transform: Scale{xScale:1.27;yScale:1.27;}
                //antialiasing: true
                visible: true
                //как достать цвет оверлея? Если не можешь достать цвет оверлея - задай его!
                ColorOverlay{
                    id: cOverlay
                    anchors.fill: image
                    source:image
                    color:image.color
                    //transform:rotation
                    //antialiasing: true
                }
        }

    opacity:1.0
    anchorPoint.x: image.width / 2
    anchorPoint.y: image.height / 2

    MouseArea {
        id: marea
        anchors.fill: parent    
        onClicked: map.placeClick(uuid)//Осталось только принять! Кстати...вот и усё!
        hoverEnabled: true
    }



    ToolTip {
                visible: marea.containsMouse
                delay: 0
                contentItem: Text{
                            font.family: "tahoma"
                            font.pixelSize: 24
                            text: name+" "+uuid
                                }
                       background: Rectangle {
                           border.color:image.color;
                           //color: "lightgray"
                       }
    }


    function setColor(color)
    {
        sourceItem.color=color;     
    }  
    function setRandomColor(color)
    {
        setColor(Qt.rgba(Math.random(),Math.random(),Math.random(),1));
    }
    function setCoordinates(latitude,longitude)
    {
        coordinate=QtPositioning.coordinate(latitude,longitude);
    }

    function setRandomCoordinates()
    {
        setCoordinates(Math.random()*180-90,Math.random()*360-180);
    }

}
