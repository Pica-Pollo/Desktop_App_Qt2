#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include "database/TaskRepository.h"
#include "models/User.h"

#include <QWidget>

namespace Ui
{
class DashboardWidget;
}

/**
 * @brief Panel principal para visualizar y administrar tareas.
 */
class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    /** @brief Construye el panel, tabla, filtros y conexiones. */
    explicit DashboardWidget(QWidget *parent = nullptr);
    ~DashboardWidget() override;

    /** @brief Establece el usuario cuyas tareas serán mostradas. */
    void setUser(const User &user);

    /** @brief Actualiza el botón que permite cambiar el tema visual. */
    void setDarkMode(bool darkMode);

public slots:
    /** @brief Recarga la tabla y los indicadores desde SQLite. */
    void refreshTasks();

signals:
    /** @brief Solicita cerrar la sesión actual. */
    void logoutRequested();

    /** @brief Informa que las tareas cambiaron. */
    void tasksChanged();

    /** @brief Solicita alternar entre el modo claro y el oscuro. */
    void themeToggleRequested();

private slots:
    /** @brief Oculta o muestra el menú lateral y actualiza su botón. */
    void toggleSidebar();

    void createTask();
    void editSelectedTask();
    void deleteSelectedTask();
    void completeSelectedTask();
    void updateActionButtons();

private:
    /** @brief Devuelve el identificador de la fila seleccionada. */
    qint64 selectedTaskId() const;

    /** @brief Llena la tabla con la consulta filtrada. */
    void populateTable(const QList<Task> &tasks);

    /** @brief Actualiza los contadores del encabezado. */
    void updateSummary(const QList<Task> &allTasks);

    /** @brief Muestra un mensaje no intrusivo en el pie del panel. */
    void showStatus(const QString &message, bool error = false);

    /** @brief Actualiza la flecha del menú con contraste para el tema actual. */
    void updateSidebarToggleIcon();

    Ui::DashboardWidget *ui;
    User m_user;
    TaskRepository m_taskRepository;
    bool m_darkMode;
};

#endif // DASHBOARDWIDGET_H
