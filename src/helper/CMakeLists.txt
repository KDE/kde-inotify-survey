# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

add_executable(kded-inotify-helper helper.cpp)
target_link_libraries(kded-inotify-helper internalLib KF5::Auth)

kauth_install_actions(org.kde.kded.inotify org.kde.kded.inotify.actions)
kauth_install_helper_files(kded-inotify-helper org.kde.kded.inotify root)
install(TARGETS kded-inotify-helper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
