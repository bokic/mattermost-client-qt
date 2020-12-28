#include "qmattermostbackend.h"

#include <QNetworkRequest>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QDebug>
#include <QList>
#include <QUrl>


#define API_V4 "api/v4/"

QMattermostBackend::QMattermostBackend(QObject *parent)
    : QObject(parent)
{
}

void QMattermostBackend::users__login(QString component)
{
    QNetworkRequest request;
    QByteArray data;
    QSettings settings;
    QString username;
    QString password;
    QString token;

    QJsonDocument  json;
    QJsonObject  jsonRoot;

    readCreditentials();

    username = settings.value("username").toString();
    password = settings.value("password").toString();
    token = settings.value("token").toString();

    jsonRoot.insert("login_id", username);
    jsonRoot.insert("password", password);
    if (!token.isEmpty())
        jsonRoot.insert("token", token);
    json.setObject(jsonRoot);
    data = json.toJson(QJsonDocument::Compact);

    request.setUrl(QUrl(m_domain + API_V4 + "users/login"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    auto reply = m_networkManager.post(request, data);

    connect(reply, &QNetworkReply::finished, this, [this, component, reply]() {
        auto data = reply->readAll();

        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if (statusCode == 200) {
            QMattermostBackendUser login;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            login.id = root.value("id").toString();
            login.create_at = root.value("create_at").toVariant().toULongLong();
            login.update_at = root.value("update_at").toVariant().toULongLong();
            login.delete_at = root.value("delete_at").toVariant().toULongLong();
            login.username = root.value("username").toString();
            login.auth_data = root.value("auth_data").toString();
            login.auth_service = root.value("auth_service").toString();
            login.email = root.value("email").toString();
            login.nickname = root.value("nickname").toString();
            login.first_name = root.value("first_name").toString();
            login.last_name = root.value("last_name").toString();
            login.position = root.value("position").toString();
            login.roles = root.value("roles").toString().split(',');
            login.allow_marketing = root.value("allow_marketing").toBool();
            login.last_password_update = root.value("last_password_update").toVariant().toULongLong();
            login.locale = root.value("locale").toString();

            auto notify_props = root.value("notify_props").toObject();
            login.notify_preps.channel = notify_props.value("channel").toBool();
            login.notify_preps.comments = notify_props.value("comments").toString();
            login.notify_preps.desktop = notify_props.value("desktop").toString();
            login.notify_preps.desktop_sound = notify_props.value("desktop_sound").toBool();
            login.notify_preps.email = notify_props.value("email").toBool();
            login.notify_preps.first_name = notify_props.value("first_name").toBool();
            login.notify_preps.mention_keys = notify_props.value("mention_keys").toString().split(',');
            login.notify_preps.push = notify_props.value("push").toString();
            login.notify_preps.push_status = notify_props.value("push_status").toString();

            auto timezone = root.value("timezone").toObject();
            login.timezone.automaticTimezone = timezone.value("automaticTimezone").toString();
            login.timezone.manualTimezone = timezone.value("manualTimezone").toString();
            login.timezone.useAutomaticTimezone = timezone.value("useAutomaticTimezone").toBool();

            emit on_login(component, login);

        } else {
            QMattermostBackendError error;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            error.id = root.value("id").toString();
            error.message = root.value("message").toString();
            error.request_id = root.value("request_id").toString();
            error.status_code = root.value("status_code").toInt();
            error.is_oath = root.value("is_oath").toBool();

            emit on_error(component, "users__login", error);
        }
    });
}

void QMattermostBackend::users__me(QString component)
{
    QNetworkRequest request;

    request.setUrl(QUrl(m_domain + API_V4 + "users/me"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    auto reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, component, reply]() {
        auto data = reply->readAll();

        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if (statusCode == 200) {
            QList<QMattermostBackendUser> users;

            QJsonDocument doc = QJsonDocument::fromJson(data);

        } else {
            QMattermostBackendError error;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            error.id = root.value("id").toString();
            error.message = root.value("message").toString();
            error.request_id = root.value("request_id").toString();
            error.status_code = root.value("status_code").toInt();
            error.is_oath = root.value("is_oath").toBool();

            emit on_error(component, "users__me", error);
        }
    });
}

void QMattermostBackend::users__ids(QString component, QStringList ids)
{
    QNetworkRequest request;
    QByteArray data;

    QJsonDocument  json;
    QJsonArray  jsonRoot;

    for(const auto &id: ids) {
        jsonRoot.append(id);
    }

    json.setArray(jsonRoot);
    data = json.toJson(QJsonDocument::Compact);

    request.setUrl(QUrl(m_domain + API_V4 + "users/ids"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    auto reply = m_networkManager.post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, component, reply]() {
        auto data = reply->readAll();

        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if (statusCode == 200) {
            QList<QMattermostBackendUser> users;
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonArray root = doc.array();

            for(const auto &itemRef: qAsConst(root)) {
                QMattermostBackendUser user;

                auto item = itemRef.toObject();

                user.id = item.value("id").toString();
                user.create_at = item.value("create_at").toVariant().toULongLong();
                user.update_at = item.value("update_at").toVariant().toULongLong();
                user.delete_at = item.value("delete_at").toVariant().toULongLong();
                user.username = item.value("username").toString();
                user.auth_data = item.value("auth_data").toString();
                user.auth_service = item.value("auth_service").toString();
                user.email = item.value("email").toString();
                user.nickname = item.value("nickname").toString();
                user.first_name = item.value("first_name").toString();
                user.last_name = item.value("last_name").toString();
                user.position = item.value("position").toString();
                user.roles = item.value("roles").toString().split(' ');
                user.locale = item.value("locale").toString();

                auto timezone = item.value("timezone").toObject();
                user.timezone.automaticTimezone = timezone.value("automaticTimezone").toString();
                user.timezone.manualTimezone = timezone.value("manualTimezone").toString();
                user.timezone.useAutomaticTimezone = timezone.value("useAutomaticTimezone").toBool();

                users.append(user);
            }

        emit on_users_ids(component, users);

        } else {
            QMattermostBackendError error;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            error.id = root.value("id").toString();
            error.message = root.value("message").toString();
            error.request_id = root.value("request_id").toString();
            error.status_code = root.value("status_code").toInt();
            error.is_oath = root.value("is_oath").toBool();

            emit on_error(component, "users__ids", error);
        }
    });
}

void QMattermostBackend::users__image(QString component, QString username_id)
{
    QNetworkRequest request;

    request.setUrl(QUrl(m_domain + API_V4 + "users/" + username_id + "/image"));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    auto reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, component, username_id, reply]() {
        auto data = reply->readAll();

        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if (statusCode == 200) {
            emit on_image(component, username_id, data);
        } else {
            QMattermostBackendError error;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            error.id = root.value("id").toString();
            error.message = root.value("message").toString();
            error.request_id = root.value("request_id").toString();
            error.status_code = root.value("status_code").toInt();
            error.is_oath = root.value("is_oath").toBool();

            emit on_error(component, "users__image", error);
        }
    });
}

void QMattermostBackend::users__me__teams(QString component)
{
    QNetworkRequest request;

    request.setUrl(QUrl(m_domain + API_V4 + "users/me/teams"));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    auto reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, component, reply]() {
        auto data = reply->readAll();

        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if (statusCode == 200) {
            QList<QMattermostBackendTeam> teams;
            QJsonDocument doc;

            doc = QJsonDocument::fromJson(data);
            auto root = doc.array();
            for(const auto &itemRef: qAsConst(root)) {
                QMattermostBackendTeam team;

                auto item = itemRef.toObject();

                team.id = item.value("id").toString();
                team.create_at = item.value("create_at").toVariant().toULongLong();
                team.update_at = item.value("update_at").toVariant().toULongLong();
                team.delete_at = item.value("delete_at").toVariant().toULongLong();
                team.display_name = item.value("display_name").toString();
                team.name = item.value("name").toString();
                team.description = item.value("description").toString();
                team.email = item.value("email").toString();
                team.type = item.value("type").toString();
                team.company_name = item.value("company_name").toString();
                team.allowed_domains = item.value("allowed_domains").toString();
                team.invite_id = item.value("invite_id").toString();
                team.allow_open_invite = item.value("allow_open_invite").toBool();
                team.scheme_id = item.value("scheme_id").toVariant();

                teams.append(team);
            }

            emit on_teams(component, teams);
        } else {
            QMattermostBackendError error;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            error.id = root.value("id").toString();
            error.message = root.value("message").toString();
            error.request_id = root.value("request_id").toString();
            error.status_code = root.value("status_code").toInt();
            error.is_oath = root.value("is_oath").toBool();

            emit on_error(component, "users__me__teams", error);
        }
    });
}

void QMattermostBackend::users__me__teams__members(QString component)
{
    QNetworkRequest request;

    request.setUrl(QUrl(m_domain + API_V4 + "users/me/teams/members"));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    auto reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, component, reply]() {
        auto data = reply->readAll();

        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if (statusCode == 200) {
            QList<QMattermostBackendTeamMember> members;
            QJsonDocument doc;

            doc = QJsonDocument::fromJson(data);
            auto root = doc.array();
            for(const auto &itemRef: qAsConst(root)) {
                QMattermostBackendTeamMember member;
                auto item = itemRef.toObject();

                member.id = item.value("id").toString();
                member.create_at = item.value("create_at").toVariant().toULongLong();
                member.update_at = item.value("update_at").toVariant().toULongLong();
                member.delete_at = item.value("delete_at").toVariant().toULongLong();
                member.display_name = item.value("display_name").toString();
                member.name = item.value("name").toString();
                member.description = item.value("description").toString();
                member.email = item.value("email").toString();
                member.type = item.value("type").toString();
                member.company_name = item.value("company_name").toString();
                member.allowed_domains = item.value("allowed_domains").toString();
                member.invite_id = item.value("invite_id").toString();
                member.allow_open_invite = item.value("allow_open_invite").toBool();
                member.scheme_id = item.value("scheme_id").toVariant();
                member.group_constrained = item.value("group_constrained").toVariant();

                members.append(member);
            }

            emit on_me_teams_members(component, members);

        } else {
            QMattermostBackendError error;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            error.id = root.value("id").toString();
            error.message = root.value("message").toString();
            error.request_id = root.value("request_id").toString();
            error.status_code = root.value("status_code").toInt();
            error.is_oath = root.value("is_oath").toBool();

            emit on_error(component, "users__me__teams__members", error);
        }
    });
}

void QMattermostBackend::users__me__teams__channels(QString component, QString team)
{
    QNetworkRequest request;

    request.setUrl(QUrl(m_domain + API_V4 + "users/me/teams/" + team + "/channels"));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    auto reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, component, team, reply]() {
        auto data = reply->readAll();

        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if (statusCode == 200) {
            QList<QMattermostBackendChannel> channels;
            QJsonDocument doc;

            doc = QJsonDocument::fromJson(data);
            auto root = doc.array();
            for(const auto &itemRef: qAsConst(root)) {
                QMattermostBackendChannel channel;

                auto item = itemRef.toObject();

                channel.id = item.value("id").toString();
                channel.create_at = item.value("create_at").toVariant().toULongLong();
                channel.update_at = item.value("update_at").toVariant().toULongLong();
                channel.delete_at = item.value("delete_at").toVariant().toULongLong();
                channel.team_id = item.value("team_id").toString();
                channel.type = item.value("type").toString();
                channel.display_name = item.value("display_name").toString();
                channel.name = item.value("name").toString();
                channel.header = item.value("header").toString();
                channel.purpose = item.value("purpose").toString();
                channel.last_post_at = item.value("last_post_at").toInt();
                channel.total_msg_count = item.value("total_msg_count").toInt();
                channel.extra_update_at = item.value("extra_update_at").toInt();
                channel.creator_id = item.value("creator_id").toString();
                QVariant scheme_id = item.value("scheme_id").toVariant();
                QVariant props = item.value("props").toVariant();

                channels.append(channel);
            }

            emit on_channels(component, team, channels);
        } else {
            QMattermostBackendError error;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            error.id = root.value("id").toString();
            error.message = root.value("message").toString();
            error.request_id = root.value("request_id").toString();
            error.status_code = root.value("status_code").toInt();
            error.is_oath = root.value("is_oath").toBool();

            emit on_error(component, "users__me__teams__channels", error);
        }
    });
}

void QMattermostBackend::posts(QString component, QString channel_id, QString pending_post_id, QString user_id, QString message)
{
    QNetworkRequest request;
    QByteArray data;

    QJsonDocument  json;
    QJsonObject  jsonRoot;

    jsonRoot.insert("channel_id", channel_id);
    if (!pending_post_id.isEmpty())
        jsonRoot.insert("pending_post_id", pending_post_id);
    if (!user_id.isEmpty())
        jsonRoot.insert("user_id", user_id);
    jsonRoot.insert("message", message);
    json.setObject(jsonRoot);
    data = json.toJson(QJsonDocument::Compact);

    request.setUrl(QUrl(m_domain + API_V4 + "posts"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    auto reply = m_networkManager.post(request, data);

    connect(reply, &QNetworkReply::finished, this, [this, component, reply]() {
        auto data = reply->readAll();

        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if (statusCode == 201) {

        } else {
            QMattermostBackendError error;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            error.id = root.value("id").toString();
            error.message = root.value("message").toString();
            error.request_id = root.value("request_id").toString();
            error.status_code = root.value("status_code").toInt();
            error.is_oath = root.value("is_oath").toBool();

            emit on_error(component, "posts", error);
        }
    });
}

void QMattermostBackend::channels__posts(QString component, QString channel_id, int page, int per_page)
{
    QNetworkRequest request;

    request.setUrl(QUrl(m_domain + API_V4 + "channels/" + channel_id + "/posts?page=" + QString::number(page) + "&per_page=" + QString::number(per_page)));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    auto reply = m_networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, component, channel_id, reply]() {
        auto data = reply->readAll();

        QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
        if (statusCode == 200) {
            QList<QMattermostBackendPost> posts;
            QJsonDocument doc;

            doc = QJsonDocument::fromJson(data);

            auto root = doc.object();
            auto orders_item = root.value("order").toArray();
            auto posts_item = root.value("posts").toObject();
            for(const auto &orderItemRef: qAsConst(orders_item)) {
                QMattermostBackendPost post;
                auto order_id = orderItemRef.toString();
                auto item = posts_item.find(order_id).value().toObject();

                post.id = item.value("id").toString();
                post.create_at = item.value("create_at").toVariant().toULongLong();
                post.update_at = item.value("update_at").toVariant().toULongLong();
                post.edit_at = item.value("edit_at").toVariant().toULongLong();
                post.delete_at = item.value("delete_at").toVariant().toULongLong();
                post.is_pinned = item.value("is_pinned").toBool();
                post.user_id = item.value("user_id").toString();
                post.channel_id = item.value("channel_id").toString();
                post.root_id = item.value("root_id").toString();
                post.parent_id = item.value("parent_id").toString();
                post.original_id = item.value("original_id").toString();
                post.message = item.value("message").toString();
                post.type = item.value("type").toString();
                post.props = item.value("props").toVariant();
                post.hashtags = item.value("hashtags").toString();
                post.pending_post_id = item.value("pending_post_id").toString();

                posts.append(post);
            }
            emit on_posts(component, channel_id, posts);
        } else {
            QMattermostBackendError error;

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            error.id = root.value("id").toString();
            error.message = root.value("message").toString();
            error.request_id = root.value("request_id").toString();
            error.status_code = root.value("status_code").toInt();
            error.is_oath = root.value("is_oath").toBool();

            emit on_error(component, "channels__posts", error);
        }
    });
}

bool QMattermostBackend::hasCreditentials()
{
    QSettings settings;
    QString domain;
    QString username;
    QString password;

    domain = settings.value("domain").toString();
    username = settings.value("username").toString();
    password = settings.value("password").toString();

    if((!domain.isEmpty())&&(!username.isEmpty())&&(!password.isEmpty()))
        return true;

    return false;
}

void QMattermostBackend::readCreditentials()
{
    QSettings settings;

    m_domain = settings.value("domain").toString();

    if (!m_domain.endsWith('/'))
        m_domain.append('/');
}

void QMattermostBackend::setCreditentials(const QString &domain, const QString &username, const QString &password, const QString &alias)
{
    QSettings settings;

    settings.setValue("domain", domain);
    settings.setValue("username", username);
    settings.setValue("password", password);
    if (!alias.isEmpty())
        settings.setValue("alias", alias);
    else
        settings.remove("alias");
}
