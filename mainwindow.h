#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class AuthService;
class DashboardWidget;
class LoginWidget;
class ReminderService;
class QSystemTrayIcon;
class Task;

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Ventana principal de la aplicación.
 *
 * Coordina la autenticación, la navegación principal y los recordatorios.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Construye la ventana principal.
     * @param parent Widget propietario de la ventana.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Libera los recursos creados por Qt Designer.
     */
    ~MainWindow() override;

private:
    /**
     * @brief Configura las propiedades generales de la ventana.
     *
     * Establece el título, tamaño inicial y tamaño mínimo.
     */
    void configureWindow();

    /** @brief Conecta servicios y eventos de las pantallas. */
    void connectApplicationFlow();

private slots:
    /** @brief Procesa una solicitud de inicio de sesión. */
    void handleLogin(const QString &username, const QString &password);

    /** @brief Procesa el registro de una cuenta nueva. */
    void handleRegister(const QString &username, const QString &password);

    /** @brief Muestra el formulario de autenticación. */
    void showLogin();

    /** @brief Muestra el panel del usuario autenticado. */
    void showDashboard();

    /** @brief Cierra la sesión actual. */
    void handleLogout();

    /** @brief Presenta una notificación de recordatorio. */
    void showReminder(const Task &task);

    /** @brief Alterna entre los temas claro y oscuro. */
    void toggleTheme();

private:
    /**
     * @brief Aplica el tema indicado a toda la aplicación.
     * @param darkMode Verdadero para el tema oscuro; falso para el claro.
     * @param persist Verdadero para guardar la selección del usuario.
     */
    void applyTheme(bool darkMode, bool persist = true);

    Ui::MainWindow *ui;
    AuthService *m_authService;
    ReminderService *m_reminderService;
    LoginWidget *m_loginWidget;
    DashboardWidget *m_dashboardWidget;
    QSystemTrayIcon *m_trayIcon;
    bool m_darkMode;
};

#endif // MAINWINDOW_H
