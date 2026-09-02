#include "UserRepository.h"

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

QDateTime parseDatabaseDate(const QString &value)
{
    QDateTime date =
        QDateTime::fromString(
            value,
            QStringLiteral("yyyy-MM-dd HH:mm:ss"));

    date.setTimeZone(QTimeZone::UTC);
    return date;
}
}

std::optional<User> UserRepository::findByUsername(
    const QString &username,
    QString *errorMessage) const
{
    QSqlQuery query(
        DatabaseManager::instance().database());

    query.prepare(QStringLiteral(R"(
        SELECT
            id,
            username,
            password_hash,
            password_salt,
            created_at
        FROM users
        WHERE username = :username COLLATE NOCASE
        LIMIT 1
    )"));

    query.bindValue(
        QStringLiteral(":username"),
        username.trimmed());

    if (!query.exec())
    {
        assignError(
            errorMessage,
            QStringLiteral("Error buscando el usuario: %1")
                .arg(query.lastError().text()));

        return std::nullopt;
    }

    if (!query.next())
    {
        return std::nullopt;
    }

    return User(
        query.value(0).toLongLong(),
        query.value(1).toString(),
        QByteArray::fromHex(query.value(2).toByteArray()),
        QByteArray::fromHex(query.value(3).toByteArray()),
        parseDatabaseDate(query.value(4).toString()));
}

std::optional<User> UserRepository::createUser(
    const QString &username,
    const QByteArray &passwordHash,
    const QByteArray &passwordSalt,
    QString *errorMessage) const
{
    QSqlQuery query(
        DatabaseManager::instance().database());

    query.prepare(QStringLiteral(R"(
        INSERT INTO users
        (
            username,
            password_hash,
            password_salt
        )
        VALUES
        (
            :username,
            :passwordHash,
            :passwordSalt
        )
    )"));

    query.bindValue(
        QStringLiteral(":username"),
        username.trimmed());

    query.bindValue(
        QStringLiteral(":passwordHash"),
        QString::fromLatin1(passwordHash.toHex()));

    query.bindValue(
        QStringLiteral(":passwordSalt"),
        QString::fromLatin1(passwordSalt.toHex()));

    if (!query.exec())
    {
        assignError(
            errorMessage,
            QStringLiteral("No se pudo crear el usuario: %1")
                .arg(query.lastError().text()));

        return std::nullopt;
    }

    return findByUsername(username, errorMessage);
}
