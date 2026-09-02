#ifndef REMINDERSERVICE_H
#define REMINDERSERVICE_H

#include "database/TaskRepository.h"

#include <QObject>
#include <QTimer>

/**
 * @brief Revisa periódicamente los recordatorios del usuario autenticado.
 */
class ReminderService : public QObject
{
    Q_OBJECT

public:
    /** @brief Construye el servicio y configura su temporizador. */
    explicit ReminderService(QObject *parent = nullptr);

    /** @brief Inicia la vigilancia de recordatorios para un usuario. */
    void start(qint64 userId);

    /** @brief Detiene la vigilancia y limpia el usuario activo. */
    void stop();

signals:
    /** @brief Se emite por cada recordatorio que debe mostrarse. */
    void reminderDue(const Task &task);

private slots:
    /** @brief Consulta SQLite y despacha recordatorios vencidos. */
    void checkReminders();

private:
    QTimer m_timer;
    TaskRepository m_taskRepository;
    qint64 m_userId;
};

#endif // REMINDERSERVICE_H
