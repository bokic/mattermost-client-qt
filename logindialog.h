#pragma once

#include "qmattermostbackend.h"
#include <QDialog>


namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void on_login_pushButton_clicked();
    void backend_on_login(QString component, QMattermostBackendUser login);
    void backend_on_error(QString component, QString operation, QMattermostBackendError error);

signals:
    void login(QString component, QString username, QString password, QString token = QString());

private:
    void loginToServer();
    Ui::LoginDialog *ui;

    QMattermostBackend m_backend;
};
