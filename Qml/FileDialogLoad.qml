import QtQuick 2.0
import QtQuick.Dialogs 1.0


FileDialog {

    title: "Please choose a file"
    folder: shortcuts.home
    onAccepted: {
        var filepath = filedialogload.fileUrl.toString().replace(/^(file:\/{3})/,"");
        console.log("opening" + filepath);
        qm.openModelFile(filepath);
    }
    onRejected: {
        console.log("Canceled")
    }
    //Component.onCompleted: visible = true
}