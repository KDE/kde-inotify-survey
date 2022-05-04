// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

#pragma once

#include <KAuth>

class INotifySurveyHelper : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
public Q_SLOTS:
    KAuth::ActionReply increaseinstancelimit(const QVariantMap &args);
    KAuth::ActionReply increasewatchlimit(const QVariantMap &args);
};
