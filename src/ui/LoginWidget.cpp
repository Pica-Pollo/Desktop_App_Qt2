#include "LoginWidget.h"
#include "ui_LoginWidget.h"

#include <QLineEdit>
#include <QPushButton>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::LoginWidget),
      m_registrationMode(false)
{
    ui->setupUi(this);

    connect(ui->primaryButton, &QPushButton::clicked,
            this, &LoginWidget::submit);
    connect(ui->switchModeButton, &QPushButton::clicked,
            this, &LoginWidget::toggleMode);
    connect(ui->themeButton, &QPushButton::clicked,
            this, &LoginWidget::themeToggleRequested);
    connect(ui->usernameEdit, &QLineEdit::returnPressed,
            this, [this]() {
                ui->passwordEdit->setFocus();
            });
    connect(ui->passwordEdit, &QLineEdit::returnPressed,
            this, [this]() {
                if (m_registrationMode)
                {
                    ui->confirmPasswordEdit->setFocus();
                    return;
                }

                submit();
            });
    connect(ui->confirmPasswordEdit, &QLineEdit::returnPressed,
            this, &LoginWidget::submit);

    updateMode();
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::showError(const QString &message)
{
    ui->errorLabel->setText(message);
    ui->errorLabel->setVisible(!message.isEmpty());
}

void LoginWidget::clearSensitiveData()
{
    ui->passwordEdit->clear();
    ui->confirmPasswordEdit->clear();
    showError(QString());
}

void LoginWidget::setBusy(bool busy)
{
    ui->usernameEdit->setEnabled(!busy);
    ui->passwordEdit->setEnabled(!busy);
    ui->confirmPasswordEdit->setEnabled(!busy);
    ui->primaryButton->setEnabled(!busy);
    ui->switchModeButton->setEnabled(!busy);
}

void LoginWidget::setDarkMode(bool darkMode)
{
    ui->themeButton->setText(
        darkMode ? QStringLiteral("Modo claro") : QStringLiteral("Modo oscuro"));
    ui->themeButton->setToolTip(
        darkMode
            ? QStringLiteral("Cambiar al modo claro")
            : QStringLiteral("Cambiar al modo oscuro"));
}

void LoginWidget::submit()
{
    showError(QString());

    const QString username = ui->usernameEdit->text().trimmed();
    const QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        showError(QStringLiteral("Completa el usuario y la contraseña."));
        return;
    }

    if (m_registrationMode)
    {
        if (password != ui->confirmPasswordEdit->text())
        {
            showError(QStringLiteral("Las contraseñas no coinciden."));
            return;
        }
        emit registerRequested(username, password);
        return;
    }

    emit loginRequested(username, password);
}

void LoginWidget::toggleMode()
{
    m_registrationMode = !m_registrationMode;
    clearSensitiveData();
    updateMode();
}

void LoginWidget::updateMode()
{
    ui->confirmPasswordLabel->setVisible(m_registrationMode);
    ui->confirmPasswordEdit->setVisible(m_registrationMode);

    if (m_registrationMode)
    {
        ui->formTitleLabel->setText(QStringLiteral("Crear una cuenta"));
        ui->formSubtitleLabel->setText(
            QStringLiteral("Tus tareas quedarán separadas de los demás usuarios."));
        ui->primaryButton->setText(QStringLiteral("Crear cuenta"));
        ui->switchModeButton->setText(QStringLiteral("Ya tengo una cuenta"));
    }
    else
    {
        ui->formTitleLabel->setText(QStringLiteral("Bienvenido"));
        ui->formSubtitleLabel->setText(
            QStringLiteral("Inicia sesión para continuar con tus tareas."));
        ui->primaryButton->setText(QStringLiteral("Iniciar sesión"));
        ui->switchModeButton->setText(QStringLiteral("Crear una cuenta nueva"));
    }

    ui->usernameEdit->setFocus();
}
