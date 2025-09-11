/*
 * File:   main.cpp
 * Author: Roman
 *
 * Created on 28 листопада 2012, 9:31
 */

#include <QApplication>
#include "audiviewerdata.h"

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);

    QApplication app(argc, argv);
    QApplication::setOrganizationName("Qubyx");
    QApplication::setOrganizationDomain("qubyx.com");
    QApplication::setApplicationName("Audi Viewer");

    // create and show your widgets here
    AudiViewerData viewerData;

    return app.exec();
}
