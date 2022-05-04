// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

#include <QDebug>
#include <QTest>

#include <entries.h>

class EntriesTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testProcULongLong()
    {
        QCOMPARE(procULongLong(QFINDTESTDATA("fixtures/proc/sys/fs/inotify/max_user_instances")), 321987);
    }

    void testCollect()
    {
        const auto entries = collectEntries(QFINDTESTDATA("fixtures/proc").toStdString());
        QCOMPARE(entries.size(), 1);
        QCOMPARE(entries[0].pid, 2);
        QCOMPARE(entries[0].instances, 1);
        QCOMPARE(entries[0].watches, 40);
        QVERIFY(entries[0].uid > 0);
    }

    void testCapacity()
    {
        const auto capacity = inotifyCapacity(QFINDTESTDATA("fixtures/proc").toStdString());
        QCOMPARE(capacity.max_user_instances, 321987);
        QCOMPARE(capacity.max_user_watches, 321654);
    }
};

QTEST_MAIN(EntriesTest)

#include "entriestest.moc"
