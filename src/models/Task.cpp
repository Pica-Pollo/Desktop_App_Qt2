#include "Task.h"

Task::Task()
    : m_id(0),
      m_userId(0),
      m_priority(Priority::Normal),
      m_status(Status::Pending),
      m_reminderSent(false)
{
}

Task::Task(qint64 id,
           qint64 userId,
           const QString &title,
           const QString &description,
           Priority priority,
           Status status,
           const QDateTime &dueAt,
           const QDateTime &reminderAt,
           bool reminderSent,
           const QDateTime &createdAt,
           const QDateTime &updatedAt,
           const QDateTime &completedAt)
    : m_id(id),
      m_userId(userId),
      m_title(title),
      m_description(description),
      m_priority(priority),
      m_status(status),
      m_dueAt(dueAt),
      m_reminderAt(reminderAt),
      m_reminderSent(reminderSent),
      m_createdAt(createdAt),
      m_updatedAt(updatedAt),
      m_completedAt(completedAt)
{
}

qint64 Task::id() const { return m_id; }
void Task::setId(qint64 id) { m_id = id; }
qint64 Task::userId() const { return m_userId; }
void Task::setUserId(qint64 userId) { m_userId = userId; }
QString Task::title() const { return m_title; }
void Task::setTitle(const QString &title) { m_title = title; }
QString Task::description() const { return m_description; }
void Task::setDescription(const QString &description) { m_description = description; }
Task::Priority Task::priority() const { return m_priority; }
void Task::setPriority(Priority priority) { m_priority = priority; }
Task::Status Task::status() const { return m_status; }
void Task::setStatus(Status status) { m_status = status; }
QDateTime Task::dueAt() const { return m_dueAt; }
void Task::setDueAt(const QDateTime &dueAt) { m_dueAt = dueAt; }
QDateTime Task::reminderAt() const { return m_reminderAt; }
void Task::setReminderAt(const QDateTime &reminderAt) { m_reminderAt = reminderAt; }
bool Task::reminderSent() const { return m_reminderSent; }
void Task::setReminderSent(bool reminderSent) { m_reminderSent = reminderSent; }
QDateTime Task::createdAt() const { return m_createdAt; }
void Task::setCreatedAt(const QDateTime &createdAt) { m_createdAt = createdAt; }
QDateTime Task::updatedAt() const { return m_updatedAt; }
void Task::setUpdatedAt(const QDateTime &updatedAt) { m_updatedAt = updatedAt; }
QDateTime Task::completedAt() const { return m_completedAt; }
void Task::setCompletedAt(const QDateTime &completedAt) { m_completedAt = completedAt; }

QString Task::priorityText(Priority priority)
{
    switch (priority)
    {
    case Priority::Low:
        return QStringLiteral("Baja");
    case Priority::High:
        return QStringLiteral("Alta");
    case Priority::Normal:
    default:
        return QStringLiteral("Normal");
    }
}

QString Task::statusText(Status status)
{
    switch (status)
    {
    case Status::InProgress:
        return QStringLiteral("En progreso");
    case Status::Completed:
        return QStringLiteral("Completada");
    case Status::Pending:
    default:
        return QStringLiteral("Pendiente");
    }
}
