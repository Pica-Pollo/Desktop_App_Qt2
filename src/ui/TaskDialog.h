#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include "models/Task.h"

#include <QDialog>

namespace Ui
{
class TaskDialog;
}

/**
 * @brief Diálogo reutilizable para crear o editar una tarea.
 */
class TaskDialog : public QDialog
{
    Q_OBJECT

public:
    /** @brief Construye el formulario y configura sus selectores. */
    explicit TaskDialog(QWidget *parent = nullptr);
    ~TaskDialog() override;

    /** @brief Carga una tarea existente en el formulario. */
    void setTask(const Task &task);

    /** @brief Construye una tarea con los valores escritos. */
    Task task(qint64 userId) const;

private slots:
    /** @brief Valida el formulario antes de cerrar con éxito. */
    void validateAndAccept();

private:
    Ui::TaskDialog *ui;
    qint64 m_taskId;
    QDateTime m_createdAt;
    QDateTime m_originalCompletedAt;
};

#endif // TASKDIALOG_H
