#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "database/UserRepository.h"
#include "models/User.h"

#include <QObject>
#include <optional>

/**
 * @brief Administra el registro, login y sesión del usuario.
 */
class AuthService : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construye el servicio de autenticación.
     */
    explicit AuthService(QObject *parent = nullptr);

    /**
     * @brief Registra un usuario nuevo.
     * @return true cuando el registro fue correcto.
     */
    bool registerUser(
        const QString &username,
        const QString &password,
        QString *errorMessage = nullptr);

    /**
     * @brief Verifica las credenciales e inicia una sesión.
     * @return true cuando las credenciales son correctas.
     */
    bool login(
        const QString &username,
        const QString &password,
        QString *errorMessage = nullptr);

    /**
     * @brief Finaliza la sesión actual.
     */
    void logout();

    /**
     * @brief Indica si existe una sesión activa.
     */
    bool isAuthenticated() const;

    /**
     * @brief Devuelve el usuario de la sesión actual.
     */
    std::optional<User> currentUser() const;

signals:
    /**
     * @brief Se emite cuando el usuario inicia sesión.
     */
    void authenticated();

    /**
     * @brief Se emite cuando se cierra la sesión.
     */
    void loggedOut();

private:
    UserRepository m_userRepository;
    std::optional<User> m_currentUser;
};

#endif // AUTHSERVICE_H