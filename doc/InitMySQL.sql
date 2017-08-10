create table users (
    uid serial NOT NULL,
    un varchar(50) NOT NULL,
    pwd text NOT NULL,
    qst int DEFAULT 1,
    ans text,
    fl text,
    gl text,
    mute text,
    PRIMARY KEY(uid),
    UNIQUE(un)
);

create table groups (
    gid serial NOT NULL,
    gn varchar(50) NOT NULL,
    gml text,
    gcreater text,
    gmgr text,
    PRIMARY KEY(gid),
    UNIQUE(gn)
);

create table privatecr (
    id serial NOT NULL,
    fromid int NOT NULL,
    toid int NOT NULL,
    tm timestamp NOT NULL,
    content text,
    PRIMARY KEY(id)
);

create table groupcr (
    id serial NOT NULL,
    gid int NOT NULL,
    tm timestamp NOT NULL,
    content text,
    PRIMARY KEY(id)
);