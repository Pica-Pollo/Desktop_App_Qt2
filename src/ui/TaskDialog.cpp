#include "TaskDialog.h"
#include "ui_TaskDialog.h"

#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>

TaskDialog::TaskDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::TaskDialog),
      m_taskId(0)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Save)->setText(QStringLiteral("Guardar"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Cancelar"));

    ui->priorityComboBox->addItem(QStringLiteral("Baja"),
                                  static_cast<int>(Task::Priority::Low));
    ui->priorityComboBox->addItem(QStringLiteral("Normal"),
                                  static_cast<int>(Task::Priority::Normal));
    ui->priorityComboBox->addItem(QStringLiteral("Alta"),
                                  static_cast<int>(Task::Priority::High));
    ui->priorityComboBox->setCurrentIndex(1);

    ui->statusComboBox->addItem(QStringLiteral("Pendiente"),
                                static_cast<int>(Task::Status::Pending));
    ui->statusComboBox->addItem(QStringLiteral("En progreso"),
                                static_cast<int>(Task::Status::InProgress));
    ui->statusComboBox->addItem(QStringLiteral("Completada"),
                                static_cast<int>(Task::Status::Completed));

    const QDateTime defaultDate = QDateTime::currentDateTime().addDays(1);
    ui->dueDateTimeEdit->setDateTime(defaultDate);
    ui->reminderDateTimeEdit->setDateTime(defaultDate.addSecs(-3600));
    ui->dueDateTimeEdit->setReadOnly(true);
    ui->reminderDateTimeEdit->setReadOnly(true);

    connect(ui->dueCheckBox, &QCheckBox::toggled,
            this, [this](bool checked) {
                ui->dueDateTimeEdit->setReadOnly(!checked);
            });
    connect(ui->reminderCheckBox, &QCheckBox::toggled,
            this, [this](bool checked) {
                ui->reminderDateTimeEdit->setReadOnly(!checked);
            });
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &TaskDialog::validateAndAccept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

TaskDialog::~TaskDialog()
{
    delete ui;
}

void TaskDialog::setTask(const Task &task)
{
    m_taskId = task.id();
    m_createdAt = task.createdAt();
    m_originalCompletedAt = task.completedAt();

    setWindowTitle(QStringLiteral("Editar tarea"));
    ui->dialogTitleLabel->setText(QStringLiteral("Editar tarea"));
    ui->titleEdit->setText(task.title());
    ui->descriptionEdit->setPlainText(task.description());

    ui->priorityComboBox->setCurrentIndex(
        ui->priorityComboBox->findData(static_cast<int>(task.priority())));
    ui->statusComboBox->setCurrentIndex(
        ui->statusComboBox->findData(static_cast<int>(task.status())));

    ui->dueCheckBox->setChecked(task.dueAt().isValid());
    if (task.dueAt().isValid())
    {
        ui->dueDateTimeEdit->setDateTime(task.dueAt().toLocalTime());
    }

    ui->reminderCheckBox->setChecked(task.reminderAt().isValid());
    if (task.reminderAt().isValid())
    {
        ui->reminderDateTimeEdit->setDateTime(task.reminderAt().toLocalTime());
    }
}

Task TaskDialog::task(qint64 userId) const
{
    const Task::Status selectedStatus = static_cast<Task::Status>(
        ui->statusComboBox->currentData().toInt());

    QDateTime completedAt;
    if (selectedStatus == Task::Status::Completed)
    {
        completedAt = m_originalCompletedAt.isValid()
                          ? m_originalCompletedAt
                          : QDateTime::currentDateTimeUtc();
    }

    return Task(
        m_taskId,
        userId,
        ui->titleEdit->text().trimmed(),
        ui->descriptionEdit->toPlainText().trimmed(),
        static_cast<Task::Priority>(ui->priorityComboBox->currentData().toInt()),
        selectedStatus,
        ui->dueCheckBox->isChecked()
            ? ui->dueDateTimeEdit->dateTime().toUTC()
            : QDateTime(),
        ui->reminderCheckBox->isChecked()
            ? ui->reminderDateTimeEdit->dateTime().toUTC()
            : QDateTime(),
        false,
        m_createdAt,
        QDateTime::currentDateTimeUtc(),
        completedAt);
}

void TaskDialog::validateAndAccept()
{
    if (ui->titleEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this,
                             QStringLiteral("Título requerido"),
                             QStringLiteral("Escribe un título para la tarea."));
        ui->titleEdit->setFocus();
        return;
    }

    if (ui->reminderCheckBox->isChecked()
        && ui->dueCheckBox->isChecked()
        && ui->reminderDateTimeEdit->dateTime() > ui->dueDateTimeEdit->dateTime())
    {
        QMessageBox::warning(
            this,
            QStringLiteral("Fecha de recordatorio"),
            QStringLiteral("El recordatorio debe ocurrir antes de la fecha límite."));
        return;
    }

    accept();
}
