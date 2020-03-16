#pragma once

#include "qmattermostbackend.h"
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

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void on_main_treeWidget_itemSelectionChanged();

    void on_lineEdit_2_returnPressed();

private:
    Ui::MainWindow *ui;
    QMattermostBackend m_backend;
    QString m_user_id;
};
