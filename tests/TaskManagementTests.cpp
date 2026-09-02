#include "database/DatabaseManager.h"
#include "database/TaskRepository.h"
#include "services/AuthService.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QtTest>

/**
 * @brief Pruebas integradas del flujo principal contra una SQLite de pruebas.
 */
class TaskManagementTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void authenticationAndTaskCrud();
    void cleanupTestCase();

private:
    QTemporaryDir m_temporaryDirectory;
};

void TaskManagementTests::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
    QCoreApplication::setOrganizationName(QStringLiteral("TaskManagementTests"));
    QCoreApplication::setApplicationName(QStringLiteral("TaskManagementTests"));

    QVERIFY(m_temporaryDirectory.isValid());
    qputenv("TASK_MANAGEMENT_DATA_DIR",
            m_temporaryDirectory.path().toUtf8());

    QString error;
    QVERIFY2(DatabaseManager::instance().initialize(&error),
             qPrintable(error));
}

void TaskManagementTests::authenticationAndTaskCrud()
{
    const QString username = QStringLiteral("test_%1")
                                 .arg(QDateTime::currentMSecsSinceEpoch());
    const QString password = QStringLiteral("Testing1!");

    AuthService authService;
    QString error;

    QVERIFY2(authService.registerUser(username, password, &error),
             qPrintable(error));
    QVERIFY(authService.isAuthenticated());
    QVERIFY(authService.currentUser().has_value());

    const User user = *authService.currentUser();
    authService.logout();
    QVERIFY(!authService.isAuthenticated());

    QVERIFY2(authService.login(username, password, &error),
             qPrintable(error));
    QCOMPARE(authService.currentUser()->id(), user.id());

    TaskRepository repository;
    Task task;
    task.setUserId(user.id());
    task.setTitle(QStringLiteral("Tarea de integración"));
    task.setDescription(QStringLiteral("Verifica creación, edición y filtros."));
    task.setPriority(Task::Priority::High);
    task.setStatus(Task::Status::Pending);
    task.setDueAt(QDateTime::currentDateTimeUtc().addDays(1));
    task.setReminderAt(QDateTime::currentDateTimeUtc().addSecs(3600));

    const std::optional<Task> created = repository.createTask(task, &error);
    QVERIFY2(created.has_value(), qPrintable(error));
    QVERIFY(created->id() > 0);

    TaskFilter filter;
    filter.searchText = QStringLiteral("integración");
    filter.priority = static_cast<int>(Task::Priority::High);
    const QList<Task> filtered = repository.findByUser(user.id(), filter, &error);
    QVERIFY2(error.isEmpty(), qPrintable(error));
    QCOMPARE(filtered.size(), 1);

    Task updated = *created;
    updated.setTitle(QStringLiteral("Tarea actualizada"));
    updated.setStatus(Task::Status::InProgress);
    QVERIFY2(repository.updateTask(updated, &error), qPrintable(error));

    const std::optional<Task> reloaded =
        repository.findById(updated.id(), user.id(), &error);
    QVERIFY(reloaded.has_value());
    QCOMPARE(reloaded->title(), QStringLiteral("Tarea actualizada"));
    QCOMPARE(reloaded->status(), Task::Status::InProgress);

    QVERIFY2(repository.markCompleted(updated.id(), user.id(), &error),
             qPrintable(error));
    const std::optional<Task> completed =
        repository.findById(updated.id(), user.id(), &error);
    QVERIFY(completed.has_value());
    QCOMPARE(completed->status(), Task::Status::Completed);
    QVERIFY(completed->completedAt().isValid());

    QVERIFY2(repository.deleteTask(updated.id(), user.id(), &error),
             qPrintable(error));
    QVERIFY(!repository.findById(updated.id(), user.id(), &error).has_value());
}

void TaskManagementTests::cleanupTestCase()
{
    DatabaseManager::instance().close();
}

QTEST_GUILESS_MAIN(TaskManagementTests)

#include "TaskManagementTests.moc"
