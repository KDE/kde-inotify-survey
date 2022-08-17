# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

add_library(kded-inotify MODULE kded.cpp)
set_target_properties(kded-inotify PROPERTIES OUTPUT_NAME inotify)
target_link_libraries(kded-inotify internalLib KF5::CoreAddons KF5::DBusAddons KF5::Notifications KF5::AuthCore KF5::I18n)
install(TARGETS kded-inotify DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/kded)
install(FILES org.kde.kded.inotify.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
