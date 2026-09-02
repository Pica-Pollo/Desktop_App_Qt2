#include "PasswordHasher.h"

#include <QCryptographicHash>
#include <QPasswordDigestor>
#include <QRandomGenerator>

namespace
{
constexpr int SALT_SIZE = 16;
constexpr int HASH_ITERATIONS = 210000;
constexpr int DERIVED_KEY_SIZE = 32;
const QByteArray PBKDF2_PREFIX = QByteArrayLiteral("pbkdf2$");

QByteArray legacyHash(const QString &password, const QByteArray &salt)
{
    const QByteArray passwordBytes = password.toUtf8();
    QByteArray result = QCryptographicHash::hash(
        salt + passwordBytes,
        QCryptographicHash::Sha256);

    for (int iteration = 1; iteration < 120000; ++iteration)
    {
        result = QCryptographicHash::hash(
            result + salt + passwordBytes,
            QCryptographicHash::Sha256);
    }
    return result;
}

bool constantTimeEquals(const QByteArray &left, const QByteArray &right)
{
    if (left.size() != right.size())
    {
        return false;
    }

    unsigned char difference = 0;
    for (int index = 0; index < left.size(); ++index)
    {
        difference |= static_cast<unsigned char>(left.at(index))
                      ^ static_cast<unsigned char>(right.at(index));
    }
    return difference == 0;
}
}

QByteArray PasswordHasher::generateSalt()
{
    QByteArray salt(SALT_SIZE, '\0');

    for (int index = 0; index < salt.size(); ++index)
    {
        salt[index] = static_cast<char>(
            QRandomGenerator::system()->bounded(256));
    }

    return salt;
}

QByteArray PasswordHasher::hashPassword(
    const QString &password,
    const QByteArray &salt)
{
    const QByteArray derivedKey = QPasswordDigestor::deriveKeyPbkdf2(
        QCryptographicHash::Sha256,
        password.toUtf8(),
        salt,
        HASH_ITERATIONS,
        DERIVED_KEY_SIZE);

    return PBKDF2_PREFIX + derivedKey;
}

bool PasswordHasher::verifyPassword(
    const QString &password,
    const QByteArray &salt,
    const QByteArray &expectedHash)
{
    if (expectedHash.startsWith(PBKDF2_PREFIX))
    {
        return constantTimeEquals(hashPassword(password, salt), expectedHash);
    }

    return constantTimeEquals(legacyHash(password, salt), expectedHash);
}
