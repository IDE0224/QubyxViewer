/*
 * File:   main.cpp
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 28 листопада 2012, 9:31
 */

#include <QApplication>
#include "qubyxviewerdata.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setOrganizationName("Qubyx");
    QApplication::setOrganizationDomain("qubyx.com");
    QApplication::setApplicationName("Qubyx Viewer");

    // create and show your widgets here
    QubyxViewerData viewerData;

    return app.exec();
}
