import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import JQImageItem 1.0

Window {
    id: window
    width: 720
    height: 720
    visible: true
    color: "#eef3fa"

    Component.onDestruction: {
        Helper.stop();
    }

    Popup{
        id: popup
        y: 100
        visible: true
        width: parent.width
        height: parent.height - 100
        JQImageItem {
            id: imageItem
            anchors.fill: parent
            visible: true
        }

        JQImageItem2 {
            id: imageItem2
            anchors.fill: parent
            visible: false
        }
        Component.onCompleted: {
            Helper.setImageItem(imageItem)
        }
    }

    MouseArea {
        anchors.fill: parent

        Row{
        Button{
            text: "close"
            width: 100
            height: 100
            onClicked: {
                popup.close()
            }
        }
        Button{
            text: "open"
            width: 100
            height: 100
            onClicked: {
                Helper.setImageItem(imageItem)
                popup.open()
            }
        }
        }


        function update() {
            imageItem.visible  = !imageItem.visible;
            imageItem2.visible = !imageItem.visible;

            // 根据visible属性，激活对应的控件
            if ( imageItem.visible )
            {
                Helper.setImageItem( imageItem );
                Helper.setImageItem2( null );
            }
            else
            {
                Helper.setImageItem( null );
                Helper.setImageItem2( imageItem2 );
            }
        }

        onClicked: {
            update();
        }

        Component.onCompleted: {
            update();
        }
    }

    Text {
        x: 15
        y: 15
        font.pixelSize: 15
        color: "#ff00ff"
        text: {
            var result = "";

            result += window.width.toString();
            result += " x ";
            result += window.height.toString();

            if ( imageItem.visible )
            {
                result += "\nJQImageItem";
            }
            else if ( imageItem2.visible )
            {
                result += "\nJQImageItem2";
            }

            return result;
        }
    }
}
