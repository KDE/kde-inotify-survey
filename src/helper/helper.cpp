// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

#include "helper.h"

#include <QDebug>
#include <QFile>

#include <entries.h>

KAuth::ActionReply INotifySurveyHelper::increaseinstancelimit(const QVariantMap &args)
{
    Q_UNUSED(args);
    const auto capacity = inotifyCapacity();
    const auto maxCapacity = maximumINotifyCapacity();
    const auto newLimit = qMin(capacity.max_user_instances + instanceStepSize(), maxCapacity.max_user_instances);
    if (QFile file(QString::fromStdU16String(max_user_instances_path().u16string())); file.open(QFile::WriteOnly | QFile::Truncate)) {
        file.write(QString::number(newLimit).toUtf8());
    } else {
        qWarning() << "Failed to open file" << file.errorString();
        return KAuth::ActionReply::HelperErrorReply();
    }
    return KAuth::ActionReply::SuccessReply();
}

KAuth::ActionReply INotifySurveyHelper::increasewatchlimit(const QVariantMap &args)
{
    Q_UNUSED(args);
    const auto capacity = inotifyCapacity();
    const auto maxCapacity = maximumINotifyCapacity();
    const auto newLimit = qMin(capacity.max_user_watches + watchStepSize(), maxCapacity.max_user_watches);
    if (QFile file(QString::fromStdU16String(max_user_watches_path().u16string())); file.open(QFile::WriteOnly | QFile::Truncate)) {
        file.write(QString::number(newLimit).toUtf8());
    } else {
        qWarning() << "Failed to open file" << file.errorString();
        return KAuth::ActionReply::HelperErrorReply();
    }
    return KAuth::ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("org.kde.kded.inotify", INotifySurveyHelper)
