#include "DatabaseManager.h"

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

namespace
{
const QString CONNECTION_NAME =
    QStringLiteral("TaskManagementConnection");

void assignError(QString *target, const QString &message)
{
    if (target != nullptr)
    {
        *target = message;
    }
}
}

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager manager;
    return manager;
}

DatabaseManager::DatabaseManager() = default;

DatabaseManager::~DatabaseManager()
{
    close();
}

bool DatabaseManager::initialize(QString *errorMessage)
{
    if (m_database.isValid() && m_database.isOpen())
    {
        return true;
    }

    QString dataDirectory =
        qEnvironmentVariable("TASK_MANAGEMENT_DATA_DIR");

#ifdef Q_OS_WIN
    if (dataDirectory.isEmpty())
    {
        const QString roamingData = qEnvironmentVariable("APPDATA");
        if (!roamingData.isEmpty())
        {
            dataDirectory = QDir(roamingData).filePath(
                QStringLiteral("TaskManagement"));
        }
    }
#endif

    if (dataDirectory.isEmpty())
    {
        dataDirectory =
            QStandardPaths::writableLocation(
                QStandardPaths::AppDataLocation);
    }

    if (dataDirectory.isEmpty())
    {
        assignError(errorMessage,
                    QStringLiteral(
                        "No se pudo determinar la carpeta de datos."));
        return false;
    }

    QDir directory;

    if (!directory.mkpath(dataDirectory))
    {
        assignError(errorMessage,
                    QStringLiteral(
                        "No se pudo crear la carpeta de datos."));
        return false;
    }

    m_databasePath =
        dataDirectory + QStringLiteral("/task-management.db");

    if (QSqlDatabase::contains(CONNECTION_NAME))
    {
        m_database =
            QSqlDatabase::database(CONNECTION_NAME);
    }
    else
    {
        m_database =
            QSqlDatabase::addDatabase(
                QStringLiteral("QSQLITE"),
                CONNECTION_NAME);
    }

    m_database.setDatabaseName(m_databasePath);

    if (!m_database.open())
    {
        assignError(
            errorMessage,
            QStringLiteral("No se pudo abrir SQLite: %1")
                .arg(m_database.lastError().text()));

        return false;
    }

    QSqlQuery foreignKeysQuery(m_database);

    if (!foreignKeysQuery.exec(
            QStringLiteral("PRAGMA foreign_keys = ON")))
    {
        assignError(
            errorMessage,
            QStringLiteral(
                "No se pudieron activar las claves foráneas: %1")
                .arg(foreignKeysQuery.lastError().text()));

        return false;
    }

    QSqlQuery configurationQuery(m_database);
    configurationQuery.exec(QStringLiteral("PRAGMA busy_timeout = 5000"));
    configurationQuery.exec(QStringLiteral("PRAGMA journal_mode = WAL"));

    return createSchema(errorMessage);
}

QSqlDatabase DatabaseManager::database() const
{
    return m_database;
}

QString DatabaseManager::databasePath() const
{
    return m_databasePath;
}

void DatabaseManager::close()
{
    if (m_database.isValid() && m_database.isOpen())
    {
        m_database.close();
    }
}

bool DatabaseManager::createSchema(QString *errorMessage)
{
    const QString createUsersTable = QStringLiteral(R"(
        CREATE TABLE IF NOT EXISTS users
        (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL COLLATE NOCASE UNIQUE,
            password_hash TEXT NOT NULL,
            password_salt TEXT NOT NULL,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
        )
    )");

    const QString createTasksTable = QStringLiteral(R"(
        CREATE TABLE IF NOT EXISTS tasks
        (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            title TEXT NOT NULL,
            description TEXT NOT NULL DEFAULT '',
            priority INTEGER NOT NULL DEFAULT 1,
            status INTEGER NOT NULL DEFAULT 0,
            due_at TEXT,
            reminder_at TEXT,
            reminder_sent INTEGER NOT NULL DEFAULT 0,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            completed_at TEXT,
            FOREIGN KEY(user_id)
                REFERENCES users(id)
                ON DELETE CASCADE
        )
    )");

    const QString createUserTaskIndex = QStringLiteral(R"(
        CREATE INDEX IF NOT EXISTS idx_tasks_user_id
        ON tasks(user_id)
    )");

    const QString createReminderIndex = QStringLiteral(R"(
        CREATE INDEX IF NOT EXISTS idx_tasks_reminder
        ON tasks(reminder_at, reminder_sent)
    )");

    return executeStatement(createUsersTable, errorMessage)
           && executeStatement(createTasksTable, errorMessage)
           && executeStatement(createUserTaskIndex, errorMessage)
           && executeStatement(createReminderIndex, errorMessage);
}

bool DatabaseManager::executeStatement(
    const QString &sql,
    QString *errorMessage)
{
    QSqlQuery query(m_database);

    if (!query.exec(sql))
    {
        assignError(
            errorMessage,
            QStringLiteral("Error creando la base de datos: %1")
                .arg(query.lastError().text()));

        return false;
    }

    return true;
}
