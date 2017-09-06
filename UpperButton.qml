import QtQuick 2.0

Rectangle{
    property string iconSource
    property string iconText
    id: temp
    width: buttonWidth
    height: buttonHeight

    anchors.top : parent.top
    anchors.bottom : tabgroupname.top

    color : "transparent"

    Image{anchors.margins:2; anchors.fill:parent; source:iconSource}
    Text{
        anchors.top:parent.top;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.topMargin: 16;
        text: iconText
        font.pixelSize: 16
        //font.bold: true
        font.family: "Arial"
        color: "#494949"
    }


    function collectConfigurations(){
        var configurations = {};

        // do collecting things
        // configurations[key] = value;
        configurations["inputfilepath"] = "C:/Users/diridiri/Desktop/DLP/lowerjaw_13.STL";//"C:/Users/diridiri/Desktop/DLP/lowerjaw.STL";
        configurations["outputfilepath"] = "C:/Users/diridiri/Desktop/DLP/output";

        return configurations;
    }

    MouseArea{
        anchors.fill: parent
        onClicked:{
            console.log(parent.id);
            console.log(parent.parent.id);



            /*(function upperButtonFunction(id){
                switch (id){
                case first_tab_button_export:{
                    console.log("exporting");
                    // collect configurations
                    var cfg = collectConfigurations();
                    se.slice(cfg);
                    break;
                }
                }
            })();*/

            console.log("exporting");
            // collect configurations
            var cfg = collectConfigurations();
            se.slice(cfg);

        }
    }
}

