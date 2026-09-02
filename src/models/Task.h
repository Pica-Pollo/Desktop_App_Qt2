#ifndef TASK_H
#define TASK_H

#include <QDateTime>
#include <QMetaType>
#include <QString>

/**
 * @brief Representa una tarea perteneciente a un usuario.
 */
class Task
{
public:
    /** @brief Prioridad utilizada para ordenar visualmente las tareas. */
    enum class Priority
    {
        Low = 0,
        Normal = 1,
        High = 2
    };

    /** @brief Estado actual del flujo de trabajo. */
    enum class Status
    {
        Pending = 0,
        InProgress = 1,
        Completed = 2
    };

    /** @brief Construye una tarea vacía con valores predeterminados. */
    Task();

    /** @brief Construye una tarea con todos sus valores persistidos. */
    Task(qint64 id,
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
         const QDateTime &completedAt);

    qint64 id() const;
    void setId(qint64 id);
    qint64 userId() const;
    void setUserId(qint64 userId);
    QString title() const;
    void setTitle(const QString &title);
    QString description() const;
    void setDescription(const QString &description);
    Priority priority() const;
    void setPriority(Priority priority);
    Status status() const;
    void setStatus(Status status);
    QDateTime dueAt() const;
    void setDueAt(const QDateTime &dueAt);
    QDateTime reminderAt() const;
    void setReminderAt(const QDateTime &reminderAt);
    bool reminderSent() const;
    void setReminderSent(bool reminderSent);
    QDateTime createdAt() const;
    void setCreatedAt(const QDateTime &createdAt);
    QDateTime updatedAt() const;
    void setUpdatedAt(const QDateTime &updatedAt);
    QDateTime completedAt() const;
    void setCompletedAt(const QDateTime &completedAt);

    /** @brief Devuelve el texto visible de una prioridad. */
    static QString priorityText(Priority priority);

    /** @brief Devuelve el texto visible de un estado. */
    static QString statusText(Status status);

private:
    qint64 m_id;
    qint64 m_userId;
    QString m_title;
    QString m_description;
    Priority m_priority;
    Status m_status;
    QDateTime m_dueAt;
    QDateTime m_reminderAt;
    bool m_reminderSent;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    QDateTime m_completedAt;
};

Q_DECLARE_METATYPE(Task)

#endif // TASK_H
