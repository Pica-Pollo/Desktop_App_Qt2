#ifndef USER_H
#define USER_H

#include <QByteArray>
#include <QDateTime>
#include <QString>

/**
 * @brief Representa un usuario almacenado en la aplicación.
 */
class User
{
public:
    /**
     * @brief Construye un usuario vacío.
     */
    User();

    /**
     * @brief Construye un usuario con todos sus datos.
     */
    User(qint64 id,
         const QString &username,
         const QByteArray &passwordHash,
         const QByteArray &passwordSalt,
         const QDateTime &createdAt);

    /** @brief Devuelve el identificador del usuario. */
    qint64 id() const;

    /** @brief Devuelve el nombre del usuario. */
    QString username() const;

    /** @brief Devuelve el hash de la contraseña. */
    QByteArray passwordHash() const;

    /** @brief Devuelve la sal utilizada para proteger la contraseña. */
    QByteArray passwordSalt() const;

    /** @brief Devuelve la fecha de creación de la cuenta. */
    QDateTime createdAt() const;

    /** @brief Indica si el usuario representa un registro válido. */
    bool isValid() const;

private:
    qint64 m_id;
    QString m_username;
    QByteArray m_passwordHash;
    QByteArray m_passwordSalt;
    QDateTime m_createdAt;
};

#endif // USER_H