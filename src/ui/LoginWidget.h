#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

namespace Ui
{
class LoginWidget;
}

/**
 * @brief Pantalla de inicio de sesión y creación de cuentas.
 */
class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    /** @brief Construye y conecta el formulario de autenticación. */
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget() override;

    /** @brief Muestra un error debajo de los campos. */
    void showError(const QString &message);

    /** @brief Limpia contraseñas y mensajes del formulario. */
    void clearSensitiveData();

    /** @brief Bloquea o habilita el formulario durante una operación. */
    void setBusy(bool busy);

    /** @brief Actualiza el botón que permite cambiar el tema visual. */
    void setDarkMode(bool darkMode);

signals:
    /** @brief Solicita validar credenciales existentes. */
    void loginRequested(const QString &username, const QString &password);

    /** @brief Solicita registrar una cuenta nueva. */
    void registerRequested(const QString &username, const QString &password);

    /** @brief Solicita alternar entre el modo claro y el oscuro. */
    void themeToggleRequested();

private slots:
    void submit();
    void toggleMode();

private:
    /** @brief Actualiza textos y campos según login o registro. */
    void updateMode();

    Ui::LoginWidget *ui;
    bool m_registrationMode;
};

#endif // LOGINWIDGET_H
