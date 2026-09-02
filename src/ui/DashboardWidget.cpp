#include "DashboardWidget.h"
#include "TaskDialog.h"
#include "ui_DashboardWidget.h"

#include <QBrush>
#include <QColor>
#include <QHeaderView>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QStyle>
#include <QTableWidgetItem>

namespace
{
QString displayDate(const QDateTime &date)
{
    return date.isValid()
               ? date.toLocalTime().toString(QStringLiteral("dd/MM/yyyy  hh:mm AP"))
               : QStringLiteral("—");
}

QIcon sidebarArrowIcon(bool pointsLeft, const QColor &color)
{
    QPixmap pixmap(20, 20);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(color,
                        2.4,
                        Qt::SolidLine,
                        Qt::RoundCap,
                        Qt::RoundJoin));

    if (pointsLeft)
    {
        painter.drawLine(QPointF(13, 4), QPointF(7, 10));
        painter.drawLine(QPointF(7, 10), QPointF(13, 16));
    }
    else
    {
        painter.drawLine(QPointF(7, 4), QPointF(13, 10));
        painter.drawLine(QPointF(13, 10), QPointF(7, 16));
    }

    return QIcon(pixmap);
}
}

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::DashboardWidget),
      m_darkMode(true)
{
    ui->setupUi(this);

    ui->toggleSidebarButton->setIconSize(QSize(18, 18));
    updateSidebarToggleIcon();

    ui->statusFilterComboBox->addItem(QStringLiteral("Todos los estados"), -1);
    ui->statusFilterComboBox->addItem(QStringLiteral("Pendientes"),
                                      static_cast<int>(Task::Status::Pending));
    ui->statusFilterComboBox->addItem(QStringLiteral("En progreso"),
                                      static_cast<int>(Task::Status::InProgress));
    ui->statusFilterComboBox->addItem(QStringLiteral("Completadas"),
                                      static_cast<int>(Task::Status::Completed));

    ui->priorityFilterComboBox->addItem(QStringLiteral("Todas las prioridades"), -1);
    ui->priorityFilterComboBox->addItem(QStringLiteral("Baja"),
                                        static_cast<int>(Task::Priority::Low));
    ui->priorityFilterComboBox->addItem(QStringLiteral("Normal"),
                                        static_cast<int>(Task::Priority::Normal));
    ui->priorityFilterComboBox->addItem(QStringLiteral("Alta"),
                                        static_cast<int>(Task::Priority::High));

    ui->tasksTableWidget->setColumnCount(5);
    ui->tasksTableWidget->setHorizontalHeaderLabels(
        {QStringLiteral("Tarea"),
         QStringLiteral("Prioridad"),
         QStringLiteral("Estado"),
         QStringLiteral("Fecha límite"),
         QStringLiteral("Recordatorio")});
    ui->tasksTableWidget->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::Stretch);
    ui->tasksTableWidget->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::ResizeToContents);
    ui->tasksTableWidget->horizontalHeader()->setSectionResizeMode(
        2, QHeaderView::ResizeToContents);
    ui->tasksTableWidget->horizontalHeader()->setSectionResizeMode(
        3, QHeaderView::ResizeToContents);
    ui->tasksTableWidget->horizontalHeader()->setSectionResizeMode(
        4, QHeaderView::ResizeToContents);

    connect(ui->newTaskButton, &QPushButton::clicked,
            this, &DashboardWidget::createTask);
    connect(ui->toggleSidebarButton, &QPushButton::clicked,
            this, &DashboardWidget::toggleSidebar);
    connect(ui->themeButton, &QPushButton::clicked,
            this, &DashboardWidget::themeToggleRequested);
    connect(ui->editTaskButton, &QPushButton::clicked,
            this, &DashboardWidget::editSelectedTask);
    connect(ui->deleteTaskButton, &QPushButton::clicked,
            this, &DashboardWidget::deleteSelectedTask);
    connect(ui->completeTaskButton, &QPushButton::clicked,
            this, &DashboardWidget::completeSelectedTask);
    connect(ui->refreshButton, &QPushButton::clicked,
            this, &DashboardWidget::refreshTasks);
    connect(ui->logoutButton, &QPushButton::clicked,
            this, &DashboardWidget::logoutRequested);
    connect(ui->allTasksMenuButton, &QPushButton::clicked,
            this, [this]() { ui->statusFilterComboBox->setCurrentIndex(0); });
    connect(ui->pendingMenuButton, &QPushButton::clicked,
            this, [this]() { ui->statusFilterComboBox->setCurrentIndex(1); });
    connect(ui->inProgressMenuButton, &QPushButton::clicked,
            this, [this]() { ui->statusFilterComboBox->setCurrentIndex(2); });
    connect(ui->completedMenuButton, &QPushButton::clicked,
            this, [this]() { ui->statusFilterComboBox->setCurrentIndex(3); });
    connect(ui->searchEdit, &QLineEdit::textChanged,
            this, &DashboardWidget::refreshTasks);
    connect(ui->statusFilterComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &DashboardWidget::refreshTasks);
    connect(ui->statusFilterComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                if (index == 0)
                    ui->allTasksMenuButton->setChecked(true);
                else if (index == 1)
                    ui->pendingMenuButton->setChecked(true);
                else if (index == 2)
                    ui->inProgressMenuButton->setChecked(true);
                else if (index == 3)
                    ui->completedMenuButton->setChecked(true);
            });
    connect(ui->priorityFilterComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &DashboardWidget::refreshTasks);
    connect(ui->tasksTableWidget, &QTableWidget::itemSelectionChanged,
            this, &DashboardWidget::updateActionButtons);
    connect(ui->tasksTableWidget, &QTableWidget::cellDoubleClicked,
            this, &DashboardWidget::editSelectedTask);

    updateActionButtons();
}

DashboardWidget::~DashboardWidget()
{
    delete ui;
}

void DashboardWidget::setDarkMode(bool darkMode)
{
    m_darkMode = darkMode;
    ui->themeButton->setText(
        darkMode ? QStringLiteral("Modo claro") : QStringLiteral("Modo oscuro"));
    ui->themeButton->setToolTip(
        darkMode
            ? QStringLiteral("Cambiar al modo claro")
            : QStringLiteral("Cambiar al modo oscuro"));

    updateSidebarToggleIcon();
}

void DashboardWidget::toggleSidebar()
{
    const bool showSidebar = ui->sidebarFrame->isHidden();
    ui->sidebarFrame->setVisible(showSidebar);

    updateSidebarToggleIcon();
    ui->toggleSidebarButton->setToolTip(
        showSidebar
            ? QStringLiteral("Ocultar menú (Ctrl+M)")
            : QStringLiteral("Mostrar menú (Ctrl+M)"));
    ui->toggleSidebarButton->setAccessibleName(
        showSidebar
            ? QStringLiteral("Ocultar menú lateral")
            : QStringLiteral("Mostrar menú lateral"));
}

void DashboardWidget::setUser(const User &user)
{
    m_user = user;
    ui->usernameLabel->setText(user.username());
    ui->welcomeLabel->setText(
        QStringLiteral("Hola, %1. Estas son tus tareas.").arg(user.username()));
    ui->searchEdit->clear();
    ui->statusFilterComboBox->setCurrentIndex(0);
    ui->allTasksMenuButton->setChecked(true);
    ui->priorityFilterComboBox->setCurrentIndex(0);
    refreshTasks();
}

void DashboardWidget::refreshTasks()
{
    if (!m_user.isValid())
    {
        return;
    }

    TaskFilter filter;
    filter.searchText = ui->searchEdit->text();
    filter.status = ui->statusFilterComboBox->currentData().toInt();
    filter.priority = ui->priorityFilterComboBox->currentData().toInt();

    QString error;
    const QList<Task> filteredTasks =
        m_taskRepository.findByUser(m_user.id(), filter, &error);

    if (!error.isEmpty())
    {
        showStatus(error, true);
        return;
    }

    populateTable(filteredTasks);

    const QList<Task> allTasks =
        m_taskRepository.findByUser(m_user.id(), TaskFilter(), &error);
    if (error.isEmpty())
    {
        updateSummary(allTasks);
    }

    showStatus(QStringLiteral("%1 tarea(s) mostrada(s).").arg(filteredTasks.size()));
}

void DashboardWidget::createTask()
{
    TaskDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    QString error;
    if (!m_taskRepository.createTask(dialog.task(m_user.id()), &error).has_value())
    {
        QMessageBox::critical(this, QStringLiteral("No se pudo guardar"), error);
        return;
    }

    refreshTasks();
    emit tasksChanged();
    showStatus(QStringLiteral("Tarea creada correctamente."));
}

void DashboardWidget::editSelectedTask()
{
    const qint64 taskId = selectedTaskId();
    if (taskId <= 0)
    {
        return;
    }

    QString error;
    const std::optional<Task> existingTask =
        m_taskRepository.findById(taskId, m_user.id(), &error);

    if (!existingTask.has_value())
    {
        QMessageBox::warning(this,
                             QStringLiteral("Tarea no disponible"),
                             error.isEmpty() ? QStringLiteral("La tarea ya no existe.") : error);
        refreshTasks();
        return;
    }

    TaskDialog dialog(this);
    dialog.setTask(*existingTask);

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    if (!m_taskRepository.updateTask(dialog.task(m_user.id()), &error))
    {
        QMessageBox::critical(this, QStringLiteral("No se pudo actualizar"), error);
        return;
    }

    refreshTasks();
    emit tasksChanged();
    showStatus(QStringLiteral("Tarea actualizada correctamente."));
}

void DashboardWidget::deleteSelectedTask()
{
    const qint64 taskId = selectedTaskId();
    if (taskId <= 0)
    {
        return;
    }

    const QMessageBox::StandardButton answer = QMessageBox::question(
        this,
        QStringLiteral("Eliminar tarea"),
        QStringLiteral("¿Deseas eliminar permanentemente la tarea seleccionada?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (answer != QMessageBox::Yes)
    {
        return;
    }

    QString error;
    if (!m_taskRepository.deleteTask(taskId, m_user.id(), &error))
    {
        QMessageBox::critical(this, QStringLiteral("No se pudo eliminar"), error);
        return;
    }

    refreshTasks();
    emit tasksChanged();
    showStatus(QStringLiteral("Tarea eliminada."));
}

void DashboardWidget::completeSelectedTask()
{
    const qint64 taskId = selectedTaskId();
    if (taskId <= 0)
    {
        return;
    }

    QString error;
    if (!m_taskRepository.markCompleted(taskId, m_user.id(), &error))
    {
        QMessageBox::critical(this, QStringLiteral("No se pudo completar"), error);
        return;
    }

    refreshTasks();
    emit tasksChanged();
    showStatus(QStringLiteral("Tarea marcada como completada."));
}

void DashboardWidget::updateActionButtons()
{
    const bool hasSelection = selectedTaskId() > 0;
    ui->editTaskButton->setEnabled(hasSelection);
    ui->completeTaskButton->setEnabled(hasSelection);
    ui->deleteTaskButton->setEnabled(hasSelection);
}

qint64 DashboardWidget::selectedTaskId() const
{
    const int row = ui->tasksTableWidget->currentRow();
    if (row < 0 || ui->tasksTableWidget->item(row, 0) == nullptr)
    {
        return 0;
    }
    return ui->tasksTableWidget->item(row, 0)->data(Qt::UserRole).toLongLong();
}

void DashboardWidget::populateTable(const QList<Task> &tasks)
{
    ui->tasksTableWidget->setUpdatesEnabled(false);
    ui->tasksTableWidget->clearContents();
    ui->tasksTableWidget->setRowCount(tasks.size());

    for (int row = 0; row < tasks.size(); ++row)
    {
        const Task &task = tasks.at(row);

        auto *titleItem = new QTableWidgetItem(task.title());
        titleItem->setData(Qt::UserRole, task.id());
        titleItem->setToolTip(task.description());

        auto *priorityItem = new QTableWidgetItem(Task::priorityText(task.priority()));
        auto *statusItem = new QTableWidgetItem(Task::statusText(task.status()));
        auto *dueItem = new QTableWidgetItem(displayDate(task.dueAt()));
        auto *reminderItem = new QTableWidgetItem(displayDate(task.reminderAt()));

        if (task.priority() == Task::Priority::High)
        {
            priorityItem->setForeground(QBrush(QColor(QStringLiteral("#F02A32"))));
        }
        else if (task.priority() == Task::Priority::Low)
        {
            priorityItem->setForeground(QBrush(QColor(QStringLiteral("#72A7FF"))));
        }

        if (task.status() == Task::Status::Completed)
        {
            statusItem->setForeground(QBrush(QColor(QStringLiteral("#2EBD75"))));
            titleItem->setForeground(QBrush(QColor(QStringLiteral("#8A8A8A"))));
        }

        if (task.dueAt().isValid()
            && task.dueAt() < QDateTime::currentDateTimeUtc()
            && task.status() != Task::Status::Completed)
        {
            dueItem->setForeground(QBrush(QColor(QStringLiteral("#F02A32"))));
            dueItem->setToolTip(QStringLiteral("Esta tarea está vencida."));
        }

        ui->tasksTableWidget->setItem(row, 0, titleItem);
        ui->tasksTableWidget->setItem(row, 1, priorityItem);
        ui->tasksTableWidget->setItem(row, 2, statusItem);
        ui->tasksTableWidget->setItem(row, 3, dueItem);
        ui->tasksTableWidget->setItem(row, 4, reminderItem);
    }

    ui->tasksTableWidget->setUpdatesEnabled(true);
    ui->emptyStateLabel->setVisible(tasks.isEmpty());
    updateActionButtons();
}

void DashboardWidget::updateSummary(const QList<Task> &allTasks)
{
    int pending = 0;
    int completed = 0;
    int overdue = 0;
    const QDateTime now = QDateTime::currentDateTimeUtc();

    for (const Task &task : allTasks)
    {
        if (task.status() == Task::Status::Completed)
        {
            ++completed;
        }
        else
        {
            ++pending;
            if (task.dueAt().isValid() && task.dueAt() < now)
            {
                ++overdue;
            }
        }
    }

    ui->pendingCountLabel->setText(QString::number(pending));
    ui->completedCountLabel->setText(QString::number(completed));
    ui->overdueCountLabel->setText(QString::number(overdue));
}

void DashboardWidget::showStatus(const QString &message, bool error)
{
    ui->statusMessageLabel->setText(message);
    ui->statusMessageLabel->setProperty("error", error);
    ui->statusMessageLabel->style()->unpolish(ui->statusMessageLabel);
    ui->statusMessageLabel->style()->polish(ui->statusMessageLabel);
}

void DashboardWidget::updateSidebarToggleIcon()
{
    const QColor iconColor(
        m_darkMode ? QStringLiteral("#FFFFFF") : QStringLiteral("#1C1C1E"));
    ui->toggleSidebarButton->setIcon(
        sidebarArrowIcon(!ui->sidebarFrame->isHidden(), iconColor));
}
