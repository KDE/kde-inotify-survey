# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

cmake_minimum_required(VERSION 3.22)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "08")
set(RELEASE_SERVICE_VERSION_MICRO "0")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kde-inotify-survey VERSION ${RELEASE_SERVICE_VERSION})

set(KF_MIN_VERSION "5.240.0")
set(QT_REQUIRED_VERSION "6.6.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(KDEClangFormat)
include(KDEGitCommitHooks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable legacy stuff to get rid of some deprecation warnings. Notably duplicated QProcess::finished overloads.
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050e00)
add_definitions(-DKNOTIFICATIONS_DISABLE_DEPRECATED_BEFORE_AND_AT=0x054c00)

find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core DBus)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED
    COMPONENTS
        I18n
        CoreAddons
        Notifications
        DBusAddons
        Auth
)

add_subdirectory(src)
add_subdirectory(autotests)

ki18n_install(po)

install(FILES org.kde.inotify-survey.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
