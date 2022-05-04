// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

#include "entries.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cctype>
#include <filesystem>

#include <QDebug>
#include <QFile>

static constexpr auto kernelMaxWatches = 1048576UL;
static constexpr auto kernelDefaultWatches = 8192UL;
static constexpr auto kernelDefaultInstances = 128UL;

qulonglong procULongLong(const QString &path)
{
    if (QFile file(path); file.open(QFile::ReadOnly)) {
        bool ok = false;
        const qulonglong value = file.readAll().simplified().toULongLong(&ok);
        if (ok) {
            return value;
        }
    }
    return 0;
}

std::vector<Entry> collectEntries(const std::string_view &procPath)
{
    std::vector<Entry> ret;
    std::filesystem::path proc(procPath);
    for (const auto &procEntry : std::filesystem::directory_iterator(proc)) {
        if (!procEntry.is_directory()) {
            continue;
        }

        bool isInt = false;
        Entry entry;
        entry.pid = QString::fromStdString(procEntry.path().filename()).toInt(&isInt);
        if (!isInt) {
            continue;
        }

        struct stat statbuf{};
        if (lstat(procEntry.path().c_str(), &statbuf) != 0) {
            qWarning() << "Failed to stat" << procEntry.path().c_str() << strerror(errno);
            continue;
        }
        entry.uid = statbuf.st_uid;

        std::error_code error;
        for (const auto &fdEntry : std::filesystem::directory_iterator(procEntry.path() / u"fd", error)) {
            if (!fdEntry.is_symlink()) {
                continue;
            }
            const auto symlinkTarget = std::filesystem::read_symlink(fdEntry, error);
            if (error) {
                continue;
            }
            if (symlinkTarget != u"anon_inode:inotify") {
                continue;
            }
            ++entry.instances;

            const QString fdinfoPath = QString::fromStdString(procEntry.path() / u"fdinfo" / fdEntry.path().filename());
            if (QFile fdinfo(fdinfoPath); fdinfo.open(QFile::ReadOnly)) {
                // This being a pseudo filesystem we want to read this all at once as canReadLine and friends will
                // not necessarily work correctly.
                const QList<QByteArray> lines = fdinfo.readAll().split('\n');
                for (const auto &line : lines) {
                    if (line.startsWith(QByteArrayLiteral("inotify "))) {
                        ++entry.watches;
                    }
                }
            } else {
                qWarning() << "Failed to open fdinfo" << fdinfoPath;
                continue;
            }
        }
        if (error) {
            continue;
        }
        if (entry.instances <= 0) {
            continue;
        }

        const QString cmdlinePath = QString::fromStdString(procEntry.path() / u"cmdline");
        if (QFile cmdline(cmdlinePath); cmdline.open(QFile::ReadOnly)) {
            entry.cmdline = cmdline.readAll().simplified();
            static constexpr auto limit = 70;
            static constexpr auto limitWithEllipsis = 70 + 3;
            if (entry.cmdline.size() > limitWithEllipsis) {
                entry.cmdline = entry.cmdline.left(limit) + QLatin1String("...");
            }
        }

        ret.push_back(entry);
    }
    return ret;
}

INotifyCapacity inotifyCapacity(const std::string_view &procPath)
{
    return {
        .max_user_instances = procULongLong(QString::fromStdU16String(max_user_instances_path(procPath).u16string())),
        .max_user_watches = procULongLong(QString::fromStdU16String(max_user_watches_path(procPath).u16string())),
    };
}

INotifyCapacity maximumINotifyCapacity()
{
    return {
        .max_user_instances = kernelMaxWatches / kernelDefaultInstances,
        .max_user_watches = kernelMaxWatches,
    };
}

qulonglong instanceStepSize()
{
    return kernelDefaultInstances;
}

qulonglong watchStepSize()
{
    return kernelDefaultWatches;
}

std::filesystem::path max_user_instances_path(const std::string_view &procPath)
{
    return std::filesystem::path(procPath) / std::filesystem::path("sys/fs/inotify/max_user_instances");
}

std::filesystem::path max_user_watches_path(const std::string_view &procPath)
{
    return std::filesystem::path(procPath) / std::filesystem::path("sys/fs/inotify/max_user_watches");
}
