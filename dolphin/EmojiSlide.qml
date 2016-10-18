import QtQuick 2.2
import com.jolla.keyboard 1.0
import Sailfish.Silica 1.0
import harbour.dolphin.keyboard 1.0
import "Emoji.js" as Emoji

SilicaGridView {

    property var content: []

    id: listView
    height: view.itemHeight
    width: view.itemWidth

    flickableDirection: Flickable.VerticalFlick
    model: content
    cellWidth: width / 7
    cellHeight: height / 4
    clip: true

    VerticalScrollDecorator {  }

    delegate:Component {
        EmojiKey {
            id: emojiKey
            property bool hasSkin: skin.indexOf(caption) > -1 && index !== 0

            caption: modelData
            width: listView.cellWidth
            height: listView.cellHeight
            src: Emoji.emojifyNew(modelData)

            onClicked: {
                //Push frequent used emoji to array for saving
                if ( frequent.indexOf(caption) === -1 ) frequent.unshift(caption)
                commit(caption)
            }

            Rectangle {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: -6
                anchors.bottomMargin: -6
                height: 12
                width: 12
                color: Qt.darker(Theme.highlightBackgroundColor, 1.2)
                visible: hasSkin
                transform: Rotation { origin.x: 6; origin.y: 6; angle: 45 }
            }

            onPressAndHold: {

                console.warn("hasSkin?")

                if ( !hasSkin ) {
                    return
                }
                console.warn("modelData", modelData)
                skinPopup.hide()
                skinPopup.textData = modelData
                var pos = listView.mapToItem(emojiKey.x, emojiKey.y)
                skinPopup.show()

            }
        }
    }
}
