import QtQuick 2.0
import QtQuick.Dialogs 1.0


FileDialog {

    title: "Please choose a file"
    folder: shortcuts.home
    onAccepted: {
        console.log("You chose: " + filedialogload.fileUrls);
        sceneRoot.loadOBJFile(filedialogload.fileUrls);

    }
    onRejected: {
        console.log("Canceled")
    }
    //Component.onCompleted: visible = true
}
