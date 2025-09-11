import qbs 1.0
import "../QbxTool.qbs" as QbxTool

Project
{

    references: [
        "../../qubyxlibs/qubyxlibs.qbs",
    ]
    QbxTool
    {
        name: "AudiViewer"

        Depends { id: qtcore; name: "Qt.core" }
        Depends {
            name: "Qt";
            submodules:
                ["gui", "widgets", "xml", "sql", "network", "quick", "multimedia"]

        }

        files: [
            "*.cpp",
            "*.h",
            "*.qrc",
            "*.ui",
            "qml/*.qml",
            "qml/3dlut.frag",
            "qml/qmldir",
        ]

        Depends {name: "QubyxProfile"}
        Depends {name: "DisplayInfo"}
        Depends {name: "OSUtils"}
    }
}
