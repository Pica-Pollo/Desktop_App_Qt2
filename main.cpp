#include "mainwindow.h"

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;

    QVBoxLayout *layout = new QVBoxLayout(&window);

    QLabel *label = new QLabel("Hello");
    QPushButton *button = new QPushButton("Change");

    layout->addWidget(label);
    layout->addWidget(button);

    QObject::connect(button, &QPushButton::clicked,
                     [label]()
                     {
                         label->setText("Hello Aruk");
                     });

    window.show();

    return app.exec();
}
