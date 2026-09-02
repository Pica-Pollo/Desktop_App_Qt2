#ifndef PASSWORDHASHER_H
#define PASSWORDHASHER_H

#include <QByteArray>
#include <QString>

/**
 * @brief Genera y verifica representaciones protegidas de contraseñas.
 *
 * Las contraseñas nunca deben almacenarse directamente en SQLite.
 */
class PasswordHasher
{
public:
    /**
     * @brief Genera una sal aleatoria para una contraseña.
     */
    static QByteArray generateSalt();

    /**
     * @brief Deriva una clave PBKDF2-SHA256 de la contraseña y su sal.
     */
    static QByteArray hashPassword(
        const QString &password,
        const QByteArray &salt);

    /**
     * @brief Comprueba una contraseña sin comparar texto plano.
     */
    static bool verifyPassword(
        const QString &password,
        const QByteArray &salt,
        const QByteArray &expectedHash);

private:
    PasswordHasher() = delete;
};

#endif // PASSWORDHASHER_H
