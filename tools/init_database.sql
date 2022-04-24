CREATE TABLE `Account`
(
    uid            SERIAL        NOT NULL,
    username       VARCHAR(32)   NOT NULL UNIQUE,
    passwd         CHAR(32)      NOT NULL,
    reset_question INT DEFAULT 1 NOT NULL,
    reset_answer   CHAR(32)      NOT NULL,
    PRIMARY KEY (UID)
);

CREATE TABLE `Friendship`
(
    uid        BIGINT UNSIGNED NOT NULL,
    friend_uid BIGINT UNSIGNED NOT NULL,
    is_muted   INT DEFAULT 0   NOT NULL,
    PRIMARY KEY (uid, friend_uid)
);

CREATE TABLE `Chatgroup`
(
    gid               SERIAL          NOT NULL,
    group_name        VARCHAR(64)     NOT NULL,
    group_creator_uid BIGINT UNSIGNED NOT NULL,
    PRIMARY KEY (gid)
);

CREATE TABLE `ChatgroupMembership`
(
    gid              BIGINT UNSIGNED NOT NULL,
    member_uid       BIGINT UNSIGNED NOT NULL,
    member_privilege INT             NOT NULL DEFAULT 0,
    PRIMARY KEY (gid, member_uid)
);

CREATE TABLE `ChatRecord`
(
    cid      SERIAL          NOT NULL,
    from_uid BIGINT UNSIGNED NOT NULL,
    to_uid   BIGINT UNSIGNED NOT NULL,
    on_time  TIMESTAMP       NOT NULL,
    content  TEXT            NOT NULL,
    PRIMARY KEY (cid),
    INDEX idx_search (from_uid, to_uid)
);

create table `ChatgroupChatRecord`
(
    gcid     SERIAL          NOT NULL,
    from_uid BIGINT UNSIGNED NOT NULL,
    to_gid   BIGINT UNSIGNED NOT NULL,
    on_time  TIMESTAMP       NOT NULL,
    content  TEXT            NOT NULL,
    PRIMARY KEY (gcid),
    INDEX idx_search (from_uid, to_gid),
    INDEX idx_time_search (on_time, from_uid)
);