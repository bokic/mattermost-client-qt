#include "logindialog.h"
#include "mainwindow.h"
#include "qmattermostbackend.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("mattermost");
    QCoreApplication::setApplicationName("client-qt");

    QApplication app(argc, argv);
    MainWindow main;

    if (!QMattermostBackend::hasCreditentials()) {
        LoginDialog login;
        if (login.exec() != QDialog::Accepted)
            return 1;
    }

    main.show();
    return app.exec();
}
