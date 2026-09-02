#include "AuthService.h"

#include "security/PasswordHasher.h"

#include <QRegularExpression>

namespace
{
void assignError(QString *target, const QString &message)
{
    if (target != nullptr)
    {
        *target = message;
    }
}
}

AuthService::AuthService(QObject *parent)
    : QObject(parent)
{
}

bool AuthService::registerUser(
    const QString &username,
    const QString &password,
    QString *errorMessage)
{
    const QString normalizedUsername = username.trimmed();

    if (normalizedUsername.length() < 3)
    {
        assignError(
            errorMessage,
            QStringLiteral(
                "El usuario debe tener al menos 3 caracteres."));

        return false;
    }

    if (normalizedUsername.length() > 30
        || !QRegularExpression(QStringLiteral("^[A-Za-z0-9_.-]+$"))
                .match(normalizedUsername)
                .hasMatch())
    {
        assignError(
            errorMessage,
            QStringLiteral(
                "El usuario solo puede contener letras, números, punto, guion y guion bajo."));

        return false;
    }

    if (password.length() < 8)
    {
        assignError(
            errorMessage,
            QStringLiteral(
                "La contraseña debe tener al menos 8 caracteres."));

        return false;
    }

    QString repositoryError;

    const std::optional<User> existingUser =
        m_userRepository.findByUsername(
            normalizedUsername,
            &repositoryError);

    if (!repositoryError.isEmpty())
    {
        assignError(errorMessage, repositoryError);
        return false;
    }

    if (existingUser.has_value())
    {
        assignError(
            errorMessage,
            QStringLiteral(
                "Ese nombre de usuario ya está registrado."));

        return false;
    }

    const QByteArray salt =
        PasswordHasher::generateSalt();

    const QByteArray hash =
        PasswordHasher::hashPassword(password, salt);

    const std::optional<User> createdUser =
        m_userRepository.createUser(
            normalizedUsername,
            hash,
            salt,
            &repositoryError);

    if (!createdUser.has_value())
    {
        assignError(errorMessage, repositoryError);
        return false;
    }

    m_currentUser = createdUser;
    emit authenticated();

    return true;
}

bool AuthService::login(
    const QString &username,
    const QString &password,
    QString *errorMessage)
{
    QString repositoryError;

    const std::optional<User> user =
        m_userRepository.findByUsername(
            username.trimmed(),
            &repositoryError);

    if (!repositoryError.isEmpty())
    {
        assignError(errorMessage, repositoryError);
        return false;
    }

    if (!user.has_value())
    {
        assignError(
            errorMessage,
            QStringLiteral(
                "El usuario o la contraseña son incorrectos."));

        return false;
    }

    const bool passwordIsValid =
        PasswordHasher::verifyPassword(
            password,
            user->passwordSalt(),
            user->passwordHash());

    if (!passwordIsValid)
    {
        assignError(
            errorMessage,
            QStringLiteral(
                "El usuario o la contraseña son incorrectos."));

        return false;
    }

    m_currentUser = user;
    emit authenticated();

    return true;
}

void AuthService::logout()
{
    if (!m_currentUser.has_value())
    {
        return;
    }

    m_currentUser.reset();
    emit loggedOut();
}

bool AuthService::isAuthenticated() const
{
    return m_currentUser.has_value();
}

std::optional<User> AuthService::currentUser() const
{
    return m_currentUser;
}
