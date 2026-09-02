#include "mainwindow.h"

#include "database/DatabaseManager.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>

#include <cstdlib>

/**
 * @brief Inicializa y ejecuta Task Management.
 */
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QApplication::setStyle(QStringLiteral("Fusion"));

    QApplication::setApplicationName("Task Management");
    QApplication::setOrganizationName("TaskManagement");
    QApplication::setApplicationVersion("0.1.0");

    QString databaseError;
    const bool initializeDatabaseOnly =
        application.arguments().contains(
            QStringLiteral("--initialize-database"));

    if (!DatabaseManager::instance().initialize(
            &databaseError))
    {
        if (initializeDatabaseOnly)
        {
            qCritical() << databaseError;
        }
        else
        {
            QMessageBox::critical(
                nullptr,
                QStringLiteral("Error de base de datos"),
                databaseError);
        }

        return EXIT_FAILURE;
    }

    if (initializeDatabaseOnly)
    {
        qInfo() << "Base de datos inicializada en:"
                << DatabaseManager::instance().databasePath();
        DatabaseManager::instance().close();
        return EXIT_SUCCESS;
    }

    MainWindow mainWindow;
    mainWindow.show();

    const int result = application.exec();

    DatabaseManager::instance().close();

    return result;
}
