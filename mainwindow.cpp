#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "models/Task.h"
#include "services/AuthService.h"
#include "services/ReminderService.h"
#include "ui/DashboardWidget.h"
#include "ui/LoginWidget.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyle>
#include <QSystemTrayIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_authService(new AuthService(this)),
      m_reminderService(new ReminderService(this)),
      m_loginWidget(new LoginWidget(this)),
      m_dashboardWidget(new DashboardWidget(this)),
      m_trayIcon(new QSystemTrayIcon(this)),
      m_darkMode(true)
{
    ui->setupUi(this);
    configureWindow();
    connectApplicationFlow();

    ui->mainStack->addWidget(m_loginWidget);
    ui->mainStack->addWidget(m_dashboardWidget);

    const QSettings settings;
    applyTheme(settings.value(QStringLiteral("appearance/darkMode"), true).toBool(),
               false);

    m_trayIcon->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    m_trayIcon->setToolTip(QStringLiteral("Task Management"));
    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        m_trayIcon->show();
    }

    showLogin();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::configureWindow()
{
    setWindowTitle(QStringLiteral("Task Management"));
    resize(1200, 720);
    setMinimumSize(960, 600);
}

void MainWindow::connectApplicationFlow()
{
    connect(m_loginWidget, &LoginWidget::loginRequested,
            this, &MainWindow::handleLogin);
    connect(m_loginWidget, &LoginWidget::registerRequested,
            this, &MainWindow::handleRegister);
    connect(m_authService, &AuthService::authenticated,
            this, &MainWindow::showDashboard);
    connect(m_authService, &AuthService::loggedOut,
            this, &MainWindow::showLogin);
    connect(m_dashboardWidget, &DashboardWidget::logoutRequested,
            this, &MainWindow::handleLogout);
    connect(m_dashboardWidget, &DashboardWidget::tasksChanged,
            this, [this]()
            {
                const std::optional<User> user = m_authService->currentUser();
                if (user.has_value())
                {
                    m_reminderService->start(user->id());
                }
            });
    connect(m_loginWidget, &LoginWidget::themeToggleRequested,
            this, &MainWindow::toggleTheme);
    connect(m_dashboardWidget, &DashboardWidget::themeToggleRequested,
            this, &MainWindow::toggleTheme);
    connect(m_reminderService, &ReminderService::reminderDue,
            this, &MainWindow::showReminder);
}

void MainWindow::toggleTheme()
{
    applyTheme(!m_darkMode);
}

void MainWindow::applyTheme(bool darkMode, bool persist)
{
    const QString resourcePath = darkMode
                                     ? QStringLiteral(":/styles/dark-red.qss")
                                     : QStringLiteral(":/styles/light-red.qss");
    QFile styleFile(resourcePath);
    if (!styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        return;
    }

    qApp->setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    m_darkMode = darkMode;
    m_loginWidget->setDarkMode(m_darkMode);
    m_dashboardWidget->setDarkMode(m_darkMode);

    if (persist)
    {
        QSettings settings;
        settings.setValue(QStringLiteral("appearance/darkMode"), m_darkMode);
    }
}

void MainWindow::handleLogin(const QString &username, const QString &password)
{
    m_loginWidget->setBusy(true);
    QString error;
    const bool success = m_authService->login(username, password, &error);
    m_loginWidget->setBusy(false);

    if (!success)
    {
        m_loginWidget->showError(error);
    }
}

void MainWindow::handleRegister(const QString &username, const QString &password)
{
    m_loginWidget->setBusy(true);
    QString error;
    const bool success = m_authService->registerUser(username, password, &error);
    m_loginWidget->setBusy(false);

    if (!success)
    {
        m_loginWidget->showError(error);
    }
}

void MainWindow::showLogin()
{
    m_reminderService->stop();
    m_loginWidget->clearSensitiveData();
    ui->mainStack->setCurrentWidget(m_loginWidget);
    statusBar()->showMessage(QStringLiteral("Inicia sesión para continuar."));
}

void MainWindow::showDashboard()
{
    const std::optional<User> user = m_authService->currentUser();
    if (!user.has_value())
    {
        showLogin();
        return;
    }

    m_loginWidget->clearSensitiveData();
    m_dashboardWidget->setUser(*user);
    ui->mainStack->setCurrentWidget(m_dashboardWidget);
    m_reminderService->start(user->id());
    statusBar()->showMessage(
        QStringLiteral("Sesión iniciada como %1.").arg(user->username()),
        5000);
}

void MainWindow::handleLogout()
{
    const QMessageBox::StandardButton answer = QMessageBox::question(
        this,
        QStringLiteral("Cerrar sesión"),
        QStringLiteral("¿Deseas cerrar la sesión actual?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (answer == QMessageBox::Yes)
    {
        m_authService->logout();
    }
}

void MainWindow::showReminder(const Task &task)
{
    const QString message = task.dueAt().isValid()
                                ? QStringLiteral("%1\nVence: %2")
                                      .arg(task.title(),
                                           task.dueAt().toLocalTime().toString(
                                               QStringLiteral("dd/MM/yyyy hh:mm AP")))
                                : task.title();

    QApplication::beep();
    statusBar()->showMessage(
        QStringLiteral("Recordatorio: %1").arg(task.title()),
        10000);

    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        m_trayIcon->showMessage(
            QStringLiteral("Recordatorio de tarea"),
            message,
            QSystemTrayIcon::Information,
            10000);
    }
}
