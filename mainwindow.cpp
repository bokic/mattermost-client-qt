#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QShowEvent>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QSettings settings;
    QString domain;

    ui->setupUi(this);

    connect(&m_backend, &QMattermostBackend::on_login, this, [this](QString component, QMattermostBackendUser login) {
        if (component == "") {
            QString username;

            username = login.username;
            m_user_id = login.id;

            if (!username.isEmpty())
                ui->username_label->setText("@" + login.username);
            else
                ui->username_label->clear();

            if (!m_user_id.isEmpty()) {
                m_backend.users__me__teams__members("");
                m_backend.users__me__teams("team_label");
                m_backend.users__image("usericon_label", m_user_id);
            } else {
                ui->usericon_label->clear();
            }
        }
    });

    connect(&m_backend, &QMattermostBackend::on_users_ids, this, [this](QString component, QList<QMattermostBackendUser> users) {
        auto topItem = ui->main_treeWidget->topLevelItem(2);
        auto childCount = topItem->childCount();

        for(int c = 0; c < childCount; c++) {
            auto child = topItem->child(c);

            if (child->text(0).isEmpty()) {
                QStringList childsNames;
                QStringList childsId;

                childsId = child->data(0, Qt::UserRole + 1).toString().split("__");
                childsId.removeDuplicates();
                childsId.removeAll(m_user_id);

                for(const auto &childId: childsId) {
                    for(const auto &user: users) {
                        if (user.id == childId) {
                            childsNames << user.username;
                            break;
                        }
                    }
                }

                child->setText(0, childsNames.join(", "));
            }
        }
    });

    connect(&m_backend, &QMattermostBackend::on_image, this, [this](QString component, QString username_id, QByteArray data) {
        if (component == "usericon_label") {
            if (!data.isEmpty()) {
                QImage img = QImage::fromData(data);
                ui->usericon_label->setPixmap(QPixmap::fromImage(img));
            }
        }
    });

    connect(&m_backend, &QMattermostBackend::on_teams, this, [this](QString component, QList<QMattermostBackendTeam> teams) {
        if (component == "team_label") {
            ui->team_label->setText(teams.first().display_name);
            m_backend.users__me__teams__channels("", teams.first().id);
        }
    });

    connect(&m_backend, &QMattermostBackend::on_channels, this, [this](QString component, QString team, QList<QMattermostBackendChannel> channels) {
        QStringList publicChannels;
        QStringList privateChannels;
        QStringList directMessages;

        for(const auto &channel: channels) {
            if (channel.type == "O") {
                publicChannels.append(channel.id + "__" + channel.display_name);
            } else if (channel.type == "P") {
                privateChannels.append(channel.id + "__" + channel.display_name);
            } else if (channel.type == "D") {
                directMessages.append(channel.id + "__" + channel.name);
            }
        }

        ui->main_treeWidget->clear();

        auto root_item = new QTreeWidgetItem(QStringList() << tr("PUBLIC CHANNELS"));
        root_item->setFlags(root_item->flags() & ~Qt::ItemIsSelectable);
        ui->main_treeWidget->addTopLevelItem(root_item);
        for(const auto &channel: publicChannels) {
            auto channel_id = channel.left(channel.indexOf("__"));
            auto channel_name = channel.right(channel.length() - channel_id.length() - 2);
            auto new_item = new QTreeWidgetItem();
            new_item->setText(0, channel_name);
            new_item->setData(0, Qt::UserRole, channel_id);
            root_item->addChild(new_item);
        }

        root_item = new QTreeWidgetItem(QStringList() << tr("PRIVATE CHANNELS"));
        root_item->setFlags(root_item->flags() & ~Qt::ItemIsSelectable);
        ui->main_treeWidget->addTopLevelItem(root_item);
        for(const auto &channel: privateChannels) {
            auto channel_id = channel.left(channel.indexOf("__"));
            auto channel_name = channel.right(channel.length() - channel_id.length() - 2);
            auto new_item = new QTreeWidgetItem();
            new_item->setText(0, channel_name);
            new_item->setData(0, Qt::UserRole, channel_id);
            root_item->addChild(new_item);
        }

        QStringList all_users_id;

        root_item = new QTreeWidgetItem(QStringList() << tr("DIRECT MESSAGES"));
        root_item->setFlags(root_item->flags() & ~Qt::ItemIsSelectable);
        ui->main_treeWidget->addTopLevelItem(root_item);
        for(const auto &directMessage: directMessages) {
            auto new_item = new QTreeWidgetItem();
            auto channel_id = directMessage.left(directMessage.indexOf("__"));
            auto users_id = directMessage.right(directMessage.length() - channel_id.length() - 2);
            new_item->setData(0, Qt::UserRole, channel_id);
            new_item->setData(0, Qt::UserRole + 1, users_id);
            root_item->addChild(new_item);

            all_users_id << users_id.split("__");
        }

        ui->main_treeWidget->expandAll();

        all_users_id.removeDuplicates();
        all_users_id.removeAll(m_user_id);

        m_backend.users__ids("", all_users_id);
    });

    connect(&m_backend, &QMattermostBackend::on_posts, this, [this](QString component, QString channel_id, QList<QMattermostBackendPost> posts) {
        return;
    });

    connect(&m_backend, &QMattermostBackend::on_error, this, [this](QString component, QString operation, QMattermostBackendError error) {
        ui->statusbar->showMessage(tr("%1(%2) => Error: %3").arg(component, operation, error.message), 5000);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    if (!event->spontaneous())
        m_backend.users__login("");
}

void MainWindow::on_main_treeWidget_itemSelectionChanged()
{
    QString channel_id;

    channel_id = ui->main_treeWidget->currentItem()->data(0, Qt::UserRole).toString();

    if (!channel_id.isEmpty())
        m_backend.channels__posts("", channel_id);
}

void MainWindow::on_lineEdit_2_returnPressed()
{
    QString channel_id;
    QString pending_post_id;
    QString user_id;
    QString message;

    message = ui->lineEdit_2->text();

    if (message.isEmpty()) return;

    if (ui->main_treeWidget->selectedItems().count() > 0)
    {
        channel_id = ui->main_treeWidget->selectedItems().at(0)->data(0, Qt::UserRole).toString();

        m_backend.posts("", channel_id, "", "", message);

        ui->lineEdit_2->setText("");
    }
}
