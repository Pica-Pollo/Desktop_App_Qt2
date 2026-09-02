#include "User.h"

User::User()
    : m_id(0)
{
}

User::User(qint64 id,
           const QString &username,
           const QByteArray &passwordHash,
           const QByteArray &passwordSalt,
           const QDateTime &createdAt)
    : m_id(id),
    m_username(username),
    m_passwordHash(passwordHash),
    m_passwordSalt(passwordSalt),
    m_createdAt(createdAt)
{
}

qint64 User::id() const
{
    return m_id;
}

QString User::username() const
{
    return m_username;
}

QByteArray User::passwordHash() const
{
    return m_passwordHash;
}

QByteArray User::passwordSalt() const
{
    return m_passwordSalt;
}

QDateTime User::createdAt() const
{
    return m_createdAt;
}

bool User::isValid() const
{
    return m_id > 0 && !m_username.isEmpty();
}