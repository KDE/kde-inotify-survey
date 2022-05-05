<!--
SPDX-License-Identifier: CC0-1.0
SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>
-->

Have you ever wondered why dolphin or any other application stopped noticing file changes? Chances are you ran out of inotify resources. kde-inotify-survey to the rescue! Sporting a kded module to tell you when things are getting dicey and a CLI tool to inspect the state of affairs.

# CLI

`kde-inotify-survey` prints a json presentation of the current inotify state of the user.

# KDED

The inotify KDED module additionally polls the state every couple minutes and warns if the user capacities have been exhausted.
The notification also includes the ability to increase the capacity by a fixed amount up to a fixed limit (8192 for instances; *128 for watches).

![Screenie](https://invent.kde.org/sitter/kde-inotify-survey/-/raw/master/screenshot.png)
