// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

#pragma once

#include <filesystem>

#include <QString>

struct Entry {
    uid_t uid = -1;
    pid_t pid = -1;
    qulonglong instances = 0;
    qulonglong watches = 0;
    QString cmdline;
};

struct INotifyCapacity {
    const qulonglong max_user_instances;
    const qulonglong max_user_watches;
};

[[nodiscard]] qulonglong procULongLong(const QString &path);
[[nodiscard]] std::vector<Entry> collectEntries(const std::string_view &procPath = {"/proc/"});
[[nodiscard]] INotifyCapacity inotifyCapacity(const std::string_view &procPath = {"/proc/"});
[[nodiscard]] INotifyCapacity maximumINotifyCapacity();
[[nodiscard]] qulonglong instanceStepSize();
[[nodiscard]] qulonglong watchStepSize();

[[nodiscard]] std::filesystem::path max_user_instances_path(const std::string_view &procPath = {"/proc/"});
[[nodiscard]] std::filesystem::path max_user_watches_path(const std::string_view &procPath = {"/proc/"});
