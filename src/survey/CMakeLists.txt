# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

add_library(internalLib STATIC entries.cpp)
set_target_properties(internalLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(internalLib Qt::Core)

add_executable(kde-inotify-survey main.cpp)
target_link_libraries(kde-inotify-survey internalLib)
install(TARGETS kde-inotify-survey ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
