#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "models/User.h"

#include <optional>

/**
 * @brief Realiza las operaciones SQLite relacionadas con usuarios.
 */
class UserRepository
{
public:
    /**
     * @brief Busca un usuario por nombre.
     * @param username Nombre que se desea buscar.
     * @param errorMessage Recibe un posible error de SQLite.
     * @return El usuario encontrado o std::nullopt.
     */
    std::optional<User> findByUsername(
        const QString &username,
        QString *errorMessage = nullptr) const;

    /**
     * @brief Inserta un usuario nuevo.
     * @param username Nombre único.
     * @param passwordHash Hash protegido.
     * @param passwordSalt Sal aleatoria.
     * @param errorMessage Recibe un posible error de SQLite.
     * @return El usuario creado o std::nullopt.
     */
    std::optional<User> createUser(
        const QString &username,
        const QByteArray &passwordHash,
        const QByteArray &passwordSalt,
        QString *errorMessage = nullptr) const;
};

#endif // USERREPOSITORY_H