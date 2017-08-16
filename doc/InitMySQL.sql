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
    fromun varchar(50) NOT NULL,
    toun varchar(50) NOT NULL,
    content text,
    PRIMARY KEY(id)
);


create table groupcr (
    id serial NOT NULL,
    un varchar(50) NOT NULL,
    gn varchar(50) NOT NULL,
    tm timestamp NOT NULL,
    content text,
    PRIMARY KEY(id)
);