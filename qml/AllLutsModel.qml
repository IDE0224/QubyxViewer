pragma Singleton
import QtQuick 2.15

Item {
    property bool allLutsEnabled: {
        for (var i = 0; i < displayInfo.length; i++)
            if (!displayInfo[i].lutEnabled)
                return false;
        return true;
    }

    function enableAllLuts(enabled) {
        for (var i = 0; i < displayInfo.length; i++)
            displayInfo[i].lutEnabled = enabled;
    }
}
