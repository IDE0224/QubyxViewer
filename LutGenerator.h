/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#ifndef LUTGENERATOR_H
#define LUTGENERATOR_H

#include <QQuickImageProvider>

class QubyxProfile;
class LutGenerator : public QQuickImageProvider
{
public:
    LutGenerator();
    ~LutGenerator();

    void setDisplayProfile(int displayId, QString filePath);
    void setWorkingProfile(QString filePath);

    virtual QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);

private:
    QubyxProfile* workingSpaceProfile_;
    std::map<int, QubyxProfile*> displayProfile_;
};

#endif // LUTGENERATOR_H
