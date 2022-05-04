# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

project(kde-inotify-survey)

cmake_minimum_required(VERSION 3.15)
find_package(ECM 5.78 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable legacy stuff to get rid of some deprecation warnings. Notably duplicated QProcess::finished overloads.
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050e00)
add_definitions(-DKNOTIFICATIONS_DISABLE_DEPRECATED_BEFORE_AND_AT=0x054c00)

find_package(Qt5 5.15 CONFIG REQUIRED Core DBus Qml Quick)
find_package(KF5KIO 5.78 REQUIRED)
find_package(KF5CoreAddons 5.78 REQUIRED)
find_package(KF5DBusAddons 5.78 REQUIRED)
find_package(KF5Notifications 5.78 REQUIRED)
find_package(KF5Auth 5.78 REQUIRED)
find_package(KF5I18n 5.78 REQUIRED)

add_subdirectory(src)
add_subdirectory(autotests)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
