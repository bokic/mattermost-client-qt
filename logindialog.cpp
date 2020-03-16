#include "logindialog.h"
#include "ui_logindialog.h"

#include <QSettings>


LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    QSettings settings;
    QString domain;
    QString username;
    QString password;
    QString alias;

    ui->setupUi(this);

    domain = settings.value("domain").toString();
    username = settings.value("username").toString();
    password = settings.value("password").toString();
    alias = settings.value("alias").toString();

    ui->domain_lineEdit->setText(domain);
    ui->username_lineEdit->setText(username);
    ui->password_lineEdit->setText(password);
    ui->alias_lineEdit->setText(alias);

    connect(&m_backend, &QMattermostBackend::on_login, this, &LoginDialog::backend_on_login);
    connect(&m_backend, &QMattermostBackend::on_error, this, &LoginDialog::backend_on_error);

    if ((!domain.isEmpty())&&(!username.isEmpty())&&(!password.isEmpty()))
        loginToServer();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_login_pushButton_clicked()
{
    QSettings settings;
    QString domain;
    QString username;
    QString password;
    QString alias;

    domain = ui->domain_lineEdit->text();
    username = ui->username_lineEdit->text();
    password = ui->password_lineEdit->text();
    alias = ui->alias_lineEdit->text();

    settings.setValue("domain", domain);
    settings.setValue("username", username);
    settings.setValue("password", password);
    settings.setValue("alias", alias);

    loginToServer();
}

void LoginDialog::backend_on_login(QString component, QMattermostBackendUser login)
{
    Q_UNUSED(component);

    accept();
}

void LoginDialog::backend_on_error(QString component, QString operation, QMattermostBackendError error)
{
    Q_UNUSED(component);
    Q_UNUSED(operation);

    ui->domain_lineEdit->setEnabled(true);
    ui->username_lineEdit->setEnabled(true);
    ui->password_lineEdit->setEnabled(true);
    ui->alias_lineEdit->setEnabled(true);
    ui->login_pushButton->setEnabled(true);
    ui->error_label->setText(error.message);
}

void LoginDialog::loginToServer()
{
    ui->domain_lineEdit->setEnabled(false);
    ui->username_lineEdit->setEnabled(false);
    ui->password_lineEdit->setEnabled(false);
    ui->alias_lineEdit->setEnabled(false);
    ui->login_pushButton->setEnabled(false);
    ui->error_label->clear();

    m_backend.users__login("");
}
