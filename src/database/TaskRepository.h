#ifndef TASKREPOSITORY_H
#define TASKREPOSITORY_H

#include "models/Task.h"

#include <QList>
#include <optional>

/**
 * @brief Criterios opcionales para consultar tareas de un usuario.
 */
struct TaskFilter
{
    QString searchText;
    int status = -1;
    int priority = -1;
};

/**
 * @brief Encapsula todas las operaciones SQL relacionadas con tareas.
 */
class TaskRepository
{
public:
    /** @brief Devuelve las tareas del usuario aplicando filtros opcionales. */
    QList<Task> findByUser(qint64 userId,
                           const TaskFilter &filter = TaskFilter(),
                           QString *errorMessage = nullptr) const;

    /** @brief Busca una tarea asegurando que pertenezca al usuario. */
    std::optional<Task> findById(qint64 taskId,
                                 qint64 userId,
                                 QString *errorMessage = nullptr) const;

    /** @brief Inserta una tarea y devuelve el registro creado. */
    std::optional<Task> createTask(const Task &task,
                                   QString *errorMessage = nullptr) const;

    /** @brief Actualiza una tarea existente del usuario. */
    bool updateTask(const Task &task,
                    QString *errorMessage = nullptr) const;

    /** @brief Elimina una tarea perteneciente al usuario. */
    bool deleteTask(qint64 taskId,
                    qint64 userId,
                    QString *errorMessage = nullptr) const;

    /** @brief Marca una tarea como completada. */
    bool markCompleted(qint64 taskId,
                       qint64 userId,
                       QString *errorMessage = nullptr) const;

    /** @brief Obtiene recordatorios vencidos aún no enviados. */
    QList<Task> findDueReminders(qint64 userId,
                                 const QDateTime &nowUtc,
                                 QString *errorMessage = nullptr) const;

    /** @brief Marca un recordatorio como notificado. */
    bool markReminderSent(qint64 taskId,
                          qint64 userId,
                          QString *errorMessage = nullptr) const;
};

#endif // TASKREPOSITORY_H
