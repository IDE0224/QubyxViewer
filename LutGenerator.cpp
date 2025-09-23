/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "LutGenerator.h"
#include <QDebug>
#include <cmath>
#include <vector>

#include "libs/QubyxProfile.h"
#include "libs/qubyxprofilechain.h"

LutGenerator::LutGenerator() : QQuickImageProvider(QQuickImageProvider::Image)
{
    workingSpaceProfile_ = nullptr;
}

LutGenerator::~LutGenerator()
{
    delete workingSpaceProfile_;

    for (auto profile : displayProfile_)
        delete profile.second;
}

void LutGenerator::setDisplayProfile(int displayId, QString filePath)
{
    if (displayProfile_.count(displayId))
        delete displayProfile_[displayId];

    QubyxProfile* profile = new QubyxProfile(filePath);
    profile->LoadFromFile();

    displayProfile_[displayId] = profile;
}

void LutGenerator::setWorkingProfile(QString filePath)
{
    if (workingSpaceProfile_)
        delete workingSpaceProfile_;

    workingSpaceProfile_ = new QubyxProfile(filePath);
    workingSpaceProfile_->LoadFromFile();
}

QImage LutGenerator::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    qDebug() << "LutGenerator::requestImage" << id;
    const int lutSize = 17 * 2 + 1;
    const int displayId = id.split("&")[0].toInt();
    bool hasProfiles = workingSpaceProfile_ && displayProfile_.count(displayId);

    QubyxProfileChain chain(QubyxProfileChain::SpaceType::DeviceSpecific,
        QubyxProfileChain::SpaceType::DeviceSpecific,
        QubyxProfileChain::RI::RealisticColorimetricWithLuminance);

    if (hasProfiles)
    {
        qDebug() << "create chain";
        chain.addProfile(workingSpaceProfile_);
        chain.addProfile(displayProfile_[displayId]);

        qDebug() << workingSpaceProfile_->profilePath() << displayProfile_[displayId]->profilePath();
    }

    if (size)
        *size = QSize(lutSize * 2, lutSize * lutSize);


    QImage lutImg(lutSize * 2, lutSize * lutSize, QImage::Format_RGB32);

    for (int R = 0; R < lutSize; R++)
        for (int G = 0; G < lutSize; G++)
            for (int B = 0; B < lutSize; B++)
            {
                std::vector<double> in(3), out;

                in[0] = R / (lutSize - 1.0);
                in[1] = G / (lutSize - 1.0);
                in[2] = B / (lutSize - 1.0);

                if (hasProfiles)
                    chain.transform(in, out);

                else
                    out = in;

                int maxValue = 256 * 256 - 1;
                unsigned int r = round(out[0] * maxValue);
                unsigned int g = round(out[1] * maxValue);
                unsigned int b = round(out[2] * maxValue);

                lutImg.setPixel(B, R * lutSize + G, qRgb(r % 256, g % 256, b % 256));
                lutImg.setPixel(B + lutSize, R * lutSize + G, qRgb(r / 256, g / 256, b / 256));
            }

    return lutImg;
}
