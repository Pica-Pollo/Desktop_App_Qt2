#include "TaskRepository.h"

#include "database/DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QTimeZone>
#include <QVariant>

namespace
{
void assignError(QString *target, const QString &message)
{
    if (target != nullptr)
    {
        *target = message;
    }
}

QVariant databaseDate(const QDateTime &date)
{
    if (!date.isValid())
    {
        return QVariant();
    }

    return date.toUTC().toString(Qt::ISODate);
}

QDateTime parseDatabaseDate(const QVariant &value)
{
    if (value.isNull() || value.toString().isEmpty())
    {
        return QDateTime();
    }

    QDateTime date = QDateTime::fromString(value.toString(), Qt::ISODate);

    if (!date.isValid())
    {
        date = QDateTime::fromString(
            value.toString(),
            QStringLiteral("yyyy-MM-dd HH:mm:ss"));
        date.setTimeZone(QTimeZone::UTC);
    }

    return date;
}

Task taskFromQuery(const QSqlQuery &query)
{
    return Task(
        query.value(QStringLiteral("id")).toLongLong(),
        query.value(QStringLiteral("user_id")).toLongLong(),
        query.value(QStringLiteral("title")).toString(),
        query.value(QStringLiteral("description")).toString(),
        static_cast<Task::Priority>(query.value(QStringLiteral("priority")).toInt()),
        static_cast<Task::Status>(query.value(QStringLiteral("status")).toInt()),
        parseDatabaseDate(query.value(QStringLiteral("due_at"))),
        parseDatabaseDate(query.value(QStringLiteral("reminder_at"))),
        query.value(QStringLiteral("reminder_sent")).toBool(),
        parseDatabaseDate(query.value(QStringLiteral("created_at"))),
        parseDatabaseDate(query.value(QStringLiteral("updated_at"))),
        parseDatabaseDate(query.value(QStringLiteral("completed_at"))));
}

const QString taskColumns = QStringLiteral(R"(
    id, user_id, title, description, priority, status,
    due_at, reminder_at, reminder_sent,
    created_at, updated_at, completed_at
)");
}

QList<Task> TaskRepository::findByUser(
    qint64 userId,
    const TaskFilter &filter,
    QString *errorMessage) const
{
    QString sql = QStringLiteral("SELECT %1 FROM tasks WHERE user_id = :userId")
                      .arg(taskColumns);

    if (!filter.searchText.trimmed().isEmpty())
    {
        sql += QStringLiteral(" AND (title LIKE :search OR description LIKE :search)");
    }
    if (filter.status >= 0)
    {
        sql += QStringLiteral(" AND status = :status");
    }
    if (filter.priority >= 0)
    {
        sql += QStringLiteral(" AND priority = :priority");
    }

    sql += QStringLiteral(
        " ORDER BY status = 2 ASC, due_at IS NULL ASC, due_at ASC, updated_at DESC");

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(sql);
    query.bindValue(QStringLiteral(":userId"), userId);

    if (!filter.searchText.trimmed().isEmpty())
    {
        query.bindValue(QStringLiteral(":search"),
                        QStringLiteral("%%1%").arg(filter.searchText.trimmed()));
    }
    if (filter.status >= 0)
    {
        query.bindValue(QStringLiteral(":status"), filter.status);
    }
    if (filter.priority >= 0)
    {
        query.bindValue(QStringLiteral(":priority"), filter.priority);
    }

    if (!query.exec())
    {
        assignError(errorMessage,
                    QStringLiteral("No se pudieron consultar las tareas: %1")
                        .arg(query.lastError().text()));
        return {};
    }

    QList<Task> tasks;
    while (query.next())
    {
        tasks.append(taskFromQuery(query));
    }
    return tasks;
}

std::optional<Task> TaskRepository::findById(
    qint64 taskId,
    qint64 userId,
    QString *errorMessage) const
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral("SELECT %1 FROM tasks WHERE id = :id AND user_id = :userId LIMIT 1")
                      .arg(taskColumns));
    query.bindValue(QStringLiteral(":id"), taskId);
    query.bindValue(QStringLiteral(":userId"), userId);

    if (!query.exec())
    {
        assignError(errorMessage,
                    QStringLiteral("No se pudo buscar la tarea: %1")
                        .arg(query.lastError().text()));
        return std::nullopt;
    }

    if (!query.next())
    {
        return std::nullopt;
    }
    return taskFromQuery(query);
}

std::optional<Task> TaskRepository::createTask(
    const Task &task,
    QString *errorMessage) const
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(R"(
        INSERT INTO tasks
        (user_id, title, description, priority, status, due_at, reminder_at,
         reminder_sent, completed_at)
        VALUES
        (:userId, :title, :description, :priority, :status, :dueAt, :reminderAt,
         0, :completedAt)
    )"));

    query.bindValue(QStringLiteral(":userId"), task.userId());
    query.bindValue(QStringLiteral(":title"), task.title().trimmed());
    query.bindValue(QStringLiteral(":description"), task.description().trimmed());
    query.bindValue(QStringLiteral(":priority"), static_cast<int>(task.priority()));
    query.bindValue(QStringLiteral(":status"), static_cast<int>(task.status()));
    query.bindValue(QStringLiteral(":dueAt"), databaseDate(task.dueAt()));
    query.bindValue(QStringLiteral(":reminderAt"), databaseDate(task.reminderAt()));
    query.bindValue(QStringLiteral(":completedAt"), databaseDate(task.completedAt()));

    if (!query.exec())
    {
        assignError(errorMessage,
                    QStringLiteral("No se pudo crear la tarea: %1")
                        .arg(query.lastError().text()));
        return std::nullopt;
    }

    return findById(query.lastInsertId().toLongLong(), task.userId(), errorMessage);
}

bool TaskRepository::updateTask(
    const Task &task,
    QString *errorMessage) const
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(R"(
        UPDATE tasks
        SET title = :title,
            description = :description,
            priority = :priority,
            status = :status,
            due_at = :dueAt,
            reminder_at = :reminderAt,
            reminder_sent = 0,
            updated_at = CURRENT_TIMESTAMP,
            completed_at = :completedAt
        WHERE id = :id AND user_id = :userId
    )"));

    query.bindValue(QStringLiteral(":title"), task.title().trimmed());
    query.bindValue(QStringLiteral(":description"), task.description().trimmed());
    query.bindValue(QStringLiteral(":priority"), static_cast<int>(task.priority()));
    query.bindValue(QStringLiteral(":status"), static_cast<int>(task.status()));
    query.bindValue(QStringLiteral(":dueAt"), databaseDate(task.dueAt()));
    query.bindValue(QStringLiteral(":reminderAt"), databaseDate(task.reminderAt()));
    query.bindValue(QStringLiteral(":completedAt"), databaseDate(task.completedAt()));
    query.bindValue(QStringLiteral(":id"), task.id());
    query.bindValue(QStringLiteral(":userId"), task.userId());

    if (!query.exec())
    {
        assignError(errorMessage,
                    QStringLiteral("No se pudo actualizar la tarea: %1")
                        .arg(query.lastError().text()));
        return false;
    }
    return query.numRowsAffected() == 1;
}

bool TaskRepository::deleteTask(
    qint64 taskId,
    qint64 userId,
    QString *errorMessage) const
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral("DELETE FROM tasks WHERE id = :id AND user_id = :userId"));
    query.bindValue(QStringLiteral(":id"), taskId);
    query.bindValue(QStringLiteral(":userId"), userId);

    if (!query.exec())
    {
        assignError(errorMessage,
                    QStringLiteral("No se pudo eliminar la tarea: %1")
                        .arg(query.lastError().text()));
        return false;
    }
    return query.numRowsAffected() == 1;
}

bool TaskRepository::markCompleted(
    qint64 taskId,
    qint64 userId,
    QString *errorMessage) const
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(R"(
        UPDATE tasks
        SET status = 2,
            completed_at = :completedAt,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = :id AND user_id = :userId
    )"));
    query.bindValue(QStringLiteral(":completedAt"), databaseDate(QDateTime::currentDateTimeUtc()));
    query.bindValue(QStringLiteral(":id"), taskId);
    query.bindValue(QStringLiteral(":userId"), userId);

    if (!query.exec())
    {
        assignError(errorMessage,
                    QStringLiteral("No se pudo completar la tarea: %1")
                        .arg(query.lastError().text()));
        return false;
    }
    return query.numRowsAffected() == 1;
}

QList<Task> TaskRepository::findDueReminders(
    qint64 userId,
    const QDateTime &nowUtc,
    QString *errorMessage) const
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(R"(
        SELECT %1 FROM tasks
        WHERE user_id = :userId
          AND reminder_at IS NOT NULL
          AND reminder_at <= :now
          AND reminder_sent = 0
          AND status <> 2
        ORDER BY reminder_at ASC
    )").arg(taskColumns));
    query.bindValue(QStringLiteral(":userId"), userId);
    query.bindValue(QStringLiteral(":now"), databaseDate(nowUtc));

    if (!query.exec())
    {
        assignError(errorMessage,
                    QStringLiteral("No se pudieron consultar los recordatorios: %1")
                        .arg(query.lastError().text()));
        return {};
    }

    QList<Task> tasks;
    while (query.next())
    {
        tasks.append(taskFromQuery(query));
    }
    return tasks;
}

bool TaskRepository::markReminderSent(
    qint64 taskId,
    qint64 userId,
    QString *errorMessage) const
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(R"(
        UPDATE tasks SET reminder_sent = 1
        WHERE id = :id AND user_id = :userId
    )"));
    query.bindValue(QStringLiteral(":id"), taskId);
    query.bindValue(QStringLiteral(":userId"), userId);

    if (!query.exec())
    {
        assignError(errorMessage,
                    QStringLiteral("No se pudo actualizar el recordatorio: %1")
                        .arg(query.lastError().text()));
        return false;
    }
    return query.numRowsAffected() == 1;
}
