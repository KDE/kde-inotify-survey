// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

#include "entries.h"

QJsonObject entryToJsonObject(const Entry &entry)
{
    return QJsonObject({
        {QStringLiteral("uid"), QJsonValue::fromVariant(entry.uid)},
        {QStringLiteral("pid"), QJsonValue::fromVariant(entry.pid)},
        {QStringLiteral("instances"), QJsonValue::fromVariant(entry.instances)},
        {QStringLiteral("watches"), QJsonValue::fromVariant(entry.watches)},
        {QStringLiteral("cmdline"), QJsonValue::fromVariant(entry.cmdline)},
    });
}

QJsonArray entryVectorToJsonArray(const std::vector<Entry> &entries)
{
    QJsonArray ary;
    for (const auto &entry : entries) {
        ary.append(entryToJsonObject(entry));
    }
    return ary;
}

int main(int argc, char **argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    const auto entries = collectEntries();

    qulonglong allInstances = 0;
    qulonglong allWatches = 0;
    for (const auto &entry : entries) {
        allInstances += entry.instances;
        allWatches += entry.watches;
    }

    const auto capacity = inotifyCapacity();
    constexpr auto percent = 100;
    const auto instancePercent = percent * allInstances / capacity.max_user_instances;
    const auto watchPercent = percent * allWatches / capacity.max_user_watches;

    const QJsonObject totalsObject({
        {QStringLiteral("instances"), QJsonValue::fromVariant(allInstances)},
        {QStringLiteral("maxInstances"), QJsonValue::fromVariant(capacity.max_user_instances)},
        {QStringLiteral("instancePercent"), QJsonValue::fromVariant(instancePercent)},
        {QStringLiteral("watches"), QJsonValue::fromVariant(allWatches)},
        {QStringLiteral("maxWatches"), QJsonValue::fromVariant(capacity.max_user_watches)},
        {QStringLiteral("watchPercent"), QJsonValue::fromVariant(watchPercent)},
    });

    QTextStream(stdout) << QJsonDocument(QJsonObject({
                                             {QStringLiteral("processes"), entryVectorToJsonArray(entries)},
                                             {QStringLiteral("totals"), totalsObject},
                                         }))
                               .toJson(QJsonDocument::JsonFormat::Indented);

    return 0;
}
