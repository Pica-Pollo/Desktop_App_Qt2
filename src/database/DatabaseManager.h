#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

/**
 * @brief Administra la conexión SQLite de toda la aplicación.
 *
 * Se implementa como Singleton para evitar abrir conexiones diferentes
 * accidentalmente desde cada ventana o repositorio.
 */
class DatabaseManager
{
public:
    /**
     * @brief Devuelve la única instancia del administrador.
     */
    static DatabaseManager &instance();

    /**
     * @brief Abre la base de datos y crea las tablas necesarias.
     * @param errorMessage Recibe una descripción cuando ocurre un error.
     * @return true cuando la base de datos está lista.
     */
    bool initialize(QString *errorMessage = nullptr);

    /**
     * @brief Devuelve la conexión activa.
     */
    QSqlDatabase database() const;

    /**
     * @brief Devuelve la ubicación física del archivo SQLite.
     */
    QString databasePath() const;

    /**
     * @brief Cierra la conexión de forma segura.
     */
    void close();

    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;

private:
    DatabaseManager();
    ~DatabaseManager();

    /**
     * @brief Crea las tablas e índices iniciales.
     */
    bool createSchema(QString *errorMessage);

    /**
     * @brief Ejecuta una sentencia necesaria para crear el esquema.
     */
    bool executeStatement(const QString &sql, QString *errorMessage);

    QSqlDatabase m_database;
    QString m_databasePath;
};

#endif // DATABASEMANAGER_H