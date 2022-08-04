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

find_package(Qt${QT_MAJOR_VERSION} 5.15 CONFIG REQUIRED Core DBus Qml Quick)
find_package(KF5 5.93.0 REQUIRED
    COMPONENTS
        KIO
        I18n
        CoreAddons
        Notifications
        DBusAddons
        Auth
)

add_subdirectory(src)
add_subdirectory(autotests)

install(FILES org.kde.inotify-survey.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
