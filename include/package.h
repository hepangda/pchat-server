#pragma once
#include<string>

const uint16_t PT_LOGIN_REQ = 100;
const uint16_t PT_LOGIN_RES = 101;
const uint16_t PT_REG_REQ = 102;
const uint16_t PT_REG_RES = 103;
const uint16_t PT_RESETPWD_REQ = 104;
const uint16_t PT_RESETPWD_RES = 105;
const uint16_t PT_ADDFRI_REQ = 106;
const uint16_t PT_ADDFRI_RES = 107;
const uint16_t PT_DELFRI_REQ = 108;
const uint16_t PT_DELFRI_RES = 109;
const uint16_t PT_ADDFRI_APP = 110;
const uint16_t PT_ONLINE = 111;
const uint16_t PT_OFFLINE = 112;
const uint16_t PT_REFRESH_FL = 113;
const uint16_t PT_FL = 114;
const uint16_t PT_REFRESH_GL = 115;
const uint16_t PT_GL = 116;
const uint16_t PT_REFRESH_GM = 117;
const uint16_t PT_GM = 118;
const uint16_t PT_MSG_GRP = 119;
const uint16_t PT_MSG_FRI = 120;
const uint16_t PT_PKEY_REQ = 121;
const uint16_t PT_PKEY_RES = 122;
const uint16_t PT_MUTE_REQ = 123;
const uint16_t PT_MUTE_RES = 124;
const uint16_t PT_SCMGR_REQ = 125;
const uint16_t PT_MGRC_NOT = 126;
const uint16_t PT_EXGRP_REQ = 127;
const uint16_t PT_EXGRP_NOT = 128;

struct pkg_head_t {
    uint16_t datasize;
    uint16_t wopr;
};

struct pkg_t {
    pkg_head_t head;
    std::string jsdata;
};

#define EXTERN_PKG_QM extern std::queue<pkg_t> qpkgRecv; \
                      extern std::queue<pkg_t> qpkgSend; \
                      extern std::mutex pkglk_recv; \
                      extern std::mutex pkglk_send;

