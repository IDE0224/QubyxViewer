/*
 * File:   main.cpp
 * Author: Roman
 *
 * Created on 28 листопада 2012, 9:31
 */

#include <QApplication>
#include "qubyxviewerdata.h"

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);

    QApplication app(argc, argv);
    QApplication::setOrganizationName("Qubyx");
    QApplication::setOrganizationDomain("qubyx.com");
    QApplication::setApplicationName("Qubyx Viewer");

    // create and show your widgets here
    QubyxViewerData viewerData;

    return app.exec();
}
