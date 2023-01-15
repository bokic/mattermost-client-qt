#pragma once

#include "qmattermostbackend.h"
#include <QSystemTrayIcon>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void canShow();

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_main_treeWidget_itemSelectionChanged();

    void on_lineEdit_2_returnPressed();

private:
    Ui::MainWindow *ui = nullptr;
    QSystemTrayIcon *m_trayIcon = nullptr;
    bool m_canShow = false;
    QMattermostBackend m_backend;
    QString m_user_id;
};
