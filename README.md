<!--
SPDX-License-Identifier: CC0-1.0
SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>
-->

# CLI

`kde-inotify-survey` prints a json presentation of the current inotify state of the user.

# KDED

The inotify KDED module additionally polls the state every couple minutes and warns if the user capacities have been exhausted.
The notification also includes the ability to increase the capacity by a fixed amount up to a fixed limit (8192 for instances; *128 for watches).

![Screenie](https://invent.kde.org/sitter/kde-inotify-survey/-/raw/master/screenshot.png)
