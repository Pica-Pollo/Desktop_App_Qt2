#include "ReminderService.h"

#include <QDateTime>

ReminderService::ReminderService(QObject *parent)
    : QObject(parent),
      m_userId(0)
{
    m_timer.setInterval(30000);
    connect(&m_timer, &QTimer::timeout,
            this, &ReminderService::checkReminders);
}

void ReminderService::start(qint64 userId)
{
    m_userId = userId;
    checkReminders();
    m_timer.start();
}

void ReminderService::stop()
{
    m_timer.stop();
    m_userId = 0;
}

void ReminderService::checkReminders()
{
    if (m_userId <= 0)
    {
        return;
    }

    const QList<Task> reminders =
        m_taskRepository.findDueReminders(
            m_userId,
            QDateTime::currentDateTimeUtc());

    for (const Task &task : reminders)
    {
        emit reminderDue(task);
        m_taskRepository.markReminderSent(task.id(), m_userId);
    }
}
