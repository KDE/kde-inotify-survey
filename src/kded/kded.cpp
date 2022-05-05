// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2022 Harald Sitter <sitter@kde.org>

#include <chrono>

#include <QTimer>

#include <KAuth/KAuthAction>
#include <KAuth/KAuthExecuteJob>
#include <KDEDModule>
#include <KLocalizedString>
#include <KNotification>
#include <KPluginFactory>

#include <entries.h>

using namespace std::chrono_literals;

static constexpr auto warningPercent = 90;

class Notifier : public QObject
{
    Q_OBJECT
public:
    struct Context {
        const qulonglong percent;
        const QString eventId;
        const QString title;
        const QString text;
        bool actionable;
        const QString actionLabel;
        const QString authAction;
    };

    using QObject::QObject;

    void process(const Context &context)
    {
        if (context.percent < warningPercent) {
            m_notified = false;
            m_notification = nullptr;
        } else if (!m_notified) {
            m_notified = true;
            m_notification = new KNotification(context.eventId);
            m_notification->setComponentName(QStringLiteral("org.kde.kded.inotify"));
            m_notification->setTitle(context.title);
            m_notification->setText(context.text);

            if (context.actionable) {
                m_notification->setActions({context.actionLabel});
                m_notification->setFlags(KNotification::Persistent);
            }
            m_notification->sendEvent();

            const QString authAction = context.authAction; // so we don't need to keep the entire context
            connect(m_notification.data(), &KNotification::activated, this, [this, authAction]() {
                m_notification->disconnect(this);
                m_notification->deleteLater();

                KAuth::Action action(authAction);
                action.setHelperId(QStringLiteral("org.kde.kded.inotify"));

                KAuth::ExecuteJob *job = action.execute();
                connect(job, &KJob::result, this, [this, job] {
                    job->deleteLater();
                    Q_EMIT actioned();
                });
                job->start();
            });
        }
    }

Q_SIGNALS:
    void actioned();

private:
    bool m_notified = false;
    QPointer<KNotification> m_notification; // Notifications auto-delete use a QPointer to track them properly.
};

class InotifyModule : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.inotify")
public:
    explicit InotifyModule(QObject *parent, const QVariantList &args)
        : KDEDModule(parent)
    {
        Q_UNUSED(args);

        connect(&m_instanceNotifier, &Notifier::actioned, this, &InotifyModule::refresh);
        connect(&m_watchNotifier, &Notifier::actioned, this, &InotifyModule::refresh);

        m_timer.setInterval(10min);
        connect(&m_timer, &QTimer::timeout, this, &InotifyModule::refresh);
        m_timer.start();
    }

public Q_SLOTS:
    Q_SCRIPTABLE void refresh()
    {
        const auto entries = collectEntries();

        quint64 allInstances = 0;
        quint64 allWatches = 0;
        for (const auto &entry : entries) {
            if (entry.uid != getuid()) {
                continue;
            }
            allInstances += entry.instances;
            allWatches += entry.watches;
        }

        const auto capacity = inotifyCapacity();
        const auto instancePercent = 100 * allInstances / capacity.max_user_instances;
        const auto watchPercent = 100 * allWatches / capacity.max_user_watches;
        const auto maxCapacity = maximumINotifyCapacity();

        m_instanceNotifier.process(Notifier::Context {
            .percent = instancePercent,
            .eventId = QStringLiteral("inotifyinstancelow"),
            .title = i18nc("@title", "Inotify Instance Capacity Low"),
            .text = capacity.max_user_instances < maxCapacity.max_user_instances
                ? i18nc("@info",
                        "You have too many applications wanting to monitor file changes! When the capacity is "
                        "exhausted it will prevent further file monitoring from working correctly. Either close "
                        "some applications or increase the limit. "
                        "Currently using %1% of instances and %2% of watches.",
                        QString::number(instancePercent),
                        QString::number(watchPercent))
                : i18nc("@info",
                        "You have too many applications wanting to monitor file changes! When the capacity is "
                        "exhausted it will prevent further file monitoring from working correctly. You capacity cannot "
                        "be increased. Try closing some applications to recover additional resources. "
                        "Currently using %1% of instances and %2% of watches.",
                        QString::number(instancePercent),
                        QString::number(watchPercent)),
            .actionable = capacity.max_user_instances < maxCapacity.max_user_instances,
            .actionLabel = i18nc("@action", "Increase Instance Limit"),
            .authAction = QStringLiteral("org.kde.kded.inotify.increaseinstancelimit"),
        });

        m_watchNotifier.process(Notifier::Context {
            .percent = watchPercent,
            .eventId = QStringLiteral("inotifywatchlow"),
            .title = i18nc("@title", "Inotify Watch Capacity Low"),
            .text = capacity.max_user_watches < maxCapacity.max_user_watches
                ? i18nc("@info",
                        "Your open applications want to watch too many files for changes! When the capacity is "
                        "exhausted it will prevent further file monitoring from working correctly. Either close "
                        "some applications or increase the limit. "
                        "Currently using %1% of instances and %2% of watches.",
                        QString::number(instancePercent),
                        QString::number(watchPercent))
                : i18nc("@info",
                        "Your open applications want to watch too many files for changes! When the capacity is "
                        "exhausted it will prevent further file monitoring from working correctly. You capacity cannot "
                        "be increased. Try closing some applications to recover additional resources. "
                        "Currently using %1% of instances and %2% of watches.",
                        QString::number(instancePercent),
                        QString::number(watchPercent)),
            .actionable = capacity.max_user_watches < maxCapacity.max_user_watches,
            .actionLabel = i18nc("@action", "Increase Watch Limit"),
            .authAction = QStringLiteral("org.kde.kded.inotify.increasewatchlimit"),
        });
    }

private:
    QTimer m_timer;
    Notifier m_instanceNotifier;
    Notifier m_watchNotifier;
};

K_PLUGIN_CLASS_WITH_JSON(InotifyModule, "inotify.json")

#include "kded.moc"
