import QtQuick 2.0
import QtQuick.Dialogs 1.0


FileDialog {
    title: "Please choose a file"
    folder: shortcuts.home
    nameFilters: ["3D files(*.stl *.obj)"]
	property var isModelBuilder: false
    onAccepted: {
		if(isModelBuilder)
		{
			var filepath = fileDialogMB.fileUrl.toString().replace(/^(file:\/{3})/,"");
			qm.openAndBuildModel(filepath);
		}
		else
		{
		    var filepath = filedialogload.fileUrl.toString().replace(/^(file:\/{3})/,"");
			qm.openModelFile(filepath);
		}
    }
    onRejected: {
        console.log("Canceled")
    }
}


