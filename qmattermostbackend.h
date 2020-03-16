#pragma once

#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QObject>
#include <QList>


struct QMattermostBackendTimezone;
struct QMattermostBackendNotifyPreps;
struct QMattermostBackendError;
struct QMattermostBackendUser;
struct QMattermostBackendTeam;
struct QMattermostBackendTeamMember;
struct QMattermostBackendChannel;
struct QMattermostBackendPost;

class QMattermostBackend : public QObject
{
    Q_OBJECT
public:
    explicit QMattermostBackend(QObject *parent = nullptr);
    void users__login(QString component);
    void users__me(QString component);
    void users__ids(QString component, QStringList ids);
    void users__image(QString component, QString username_id);
    void users__me__teams(QString component);
    void users__me__teams__members(QString component);
    void users__me__teams__channels(QString component, QString team);
    void posts(QString component, QString channel_id, QString pending_post_id, QString user_id, QString message);
    void channels__posts(QString component, QString channel_id, int page = 0, int per_page = 30);

    static bool hasCreditentials();
    void readCreditentials();
    static void setCreditentials(const QString &domain, const QString &username, const QString &password, const QString &alias = QStringLiteral(""));

signals:
    void on_login(QString component, QMattermostBackendUser login);
    void on_users_ids(QString component, QList<QMattermostBackendUser> users);
    void on_error(QString component, QString operation, QMattermostBackendError error);
    void on_image(QString component, QString username_id, QByteArray data);
    void on_teams(QString component, QList<QMattermostBackendTeam> teams);
    void on_me_teams_members(QString component, QList<QMattermostBackendTeamMember> team_members);
    void on_channels(QString component, QString team, QList<QMattermostBackendChannel> channels);
    void on_posts(QString component, QString channel_id, QList<QMattermostBackendPost> posts);

private:
    QNetworkAccessManager m_networkManager;
    QString m_domain;
};

struct QMattermostBackendTimezone {
    QString automaticTimezone;
    QString manualTimezone;
    bool useAutomaticTimezone;
};

struct QMattermostBackendNotifyPreps {
    bool channel;
    QString comments;
    QString desktop;
    bool desktop_sound;
    bool email;
    bool first_name;
    QStringList mention_keys;
    QString push;
    QString push_status;
};

struct QMattermostBackendError {
    QString id;
    QString message;
    QString request_id;
    int status_code;
    bool is_oath;
};

struct QMattermostBackendUser {
    QString id;
    uint64_t create_at;
    uint64_t update_at;
    uint64_t delete_at;
    QString username;
    QString auth_data;
    QString auth_service;
    QString email;
    QString nickname;
    QString first_name;
    QString last_name;
    QString position;
    QStringList roles;
    bool allow_marketing;
    QMattermostBackendNotifyPreps notify_preps;
    uint64_t last_password_update;
    QString locale;
    QMattermostBackendTimezone timezone;
};

struct QMattermostBackendTeam {
    QString id;
    uint64_t create_at;
    uint64_t update_at;
    uint64_t delete_at;
    QString display_name;
    QString name;
    QString description;
    QString email;
    QString type;
    QString company_name;
    QString allowed_domains;
    QString invite_id;
    bool allow_open_invite;
    QVariant scheme_id;
};

struct QMattermostBackendTeamMember {
    QString id;
    uint64_t create_at;
    uint64_t update_at;
    uint64_t delete_at;
    QString display_name;
    QString name;
    QString description;
    QString email;
    QString type;
    QString company_name;
    QString allowed_domains;
    QString invite_id;
    bool allow_open_invite;
    QVariant scheme_id;
    QVariant group_constrained;
};

struct QMattermostBackendChannel {
    QString id;
    uint64_t create_at;
    uint64_t update_at;
    uint64_t delete_at;
    QString team_id;
    QString type;
    QString display_name;
    QString name;
    QString header;
    QString purpose;
    int last_post_at;
    int total_msg_count;
    int extra_update_at;
    QString creator_id;
    QVariant scheme_id;
    QVariant props;
};

struct QMattermostBackendPost {
    QString id;
    uint64_t create_at;
    uint64_t update_at;
    uint64_t edit_at;
    uint64_t delete_at;
    bool is_pinned;
    QString user_id;
    QString channel_id;
    QString root_id;
    QString parent_id;
    QString original_id;
    QString message;
    QString type;
    QVariant props;
    QString hashtags;
    QString pending_post_id;
};
