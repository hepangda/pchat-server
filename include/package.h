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


/***********************************************************************************
---------------------------------------------------------------------------------------
| wOpr |         Description         |                  Package Format                |
|------|-----------------------------|------------------------------------------------|
|  100 | Login Request               | {"un":"__","pwd":"MD5"}                        |
|  101 | Login Result                | {"un":"__","res":<0S1F>}                       |
|  102 | Register Request            | {"un":"__","pwd:"MD5","qst":__,"ans":"MD5"}    |
|  103 | Register Result             | {"un":"__","res":<0S1F>}                       |
|  104 | Reset Password Request      | {"un":"__","npwd":"MD5","qst":__,"ans":"MD5"}  |
|  105 | Reset Password Result       | {"un":"__","res":<0S1F>}                       |
|  106 | Add Friend Request          | {"un":"__","afwho":"__"}                       |
|  107 | Add Friend Result           | {"un":"__","afwho":"__","res":<0S1F>}          |
|  108 | Delete Friend Request       | {"un":"__","dfwho":"__"}                       |
|  109 | Delete Friend Result        | {"un":"__","dfwho":"__","res":<0S1F>}          |
|  110 | Add Friend Application      | {"un":"__","afwho":"__","agr":<0A1D>}          |
|  111 | Online Notice               | {"un":"__"}                                    |
|  112 | Offline Notice              | {"un":"__"}                                    |
|  113 | Refresh Friendlist Request  | {"un":"__"}                                    |
|  114 | Friendlist Result           | {"un":"__","res":<0S1F>,                       |
|      |                             | "fl":[{"fun":"__","fst":<0O1F2M},...]}         |
|  115 | Refresh Grouplist Request   | {"un":"__"}                                    |
|  116 | Grouplist Result            | {"un":"__","res":<0S1F>,"gl":                  |
|      |                             | [{"gn":"__","gpw":<0L1G2M>},...]}              |
|  117 | Refresh Groupmember Request | {"un":"__"}                                    |
|  118 | Groupmember Result          | {"un":"__","res":<0S1F>,                       |
|      |                             | "gml":["gmn1","gmn2",...]}                     |
|  119 | Group Chatmsg               | {"gid":__,"sd":"__","msg":["RSA",...]}         |
|  120 | Friend Chatmsg              | {"sd":"__","rv":"__","msg":["RSA",...]}        |
|  121 | Publickey Request           | {"un":"__","typ":<0C1S>}                       |
|  122 | Publickey Result            | {"un":"__","typ":<0C1S>,"pky":"__"}            |
|  123 | Mute Request                | {"un":"__","mtwho":"__"}                       |
|  124 | Mute Result                 | {"un":"__","mtwho":"__","res":<0S1F>}          |
|  125 | Set/Cancel Manager Request  | {"un":"__","mgwho":"__","gid":__,"typ":<0S1C>} |
|  126 | Manager Change Notice       | {"mgwho":"__","gid":__,"typ":<0S1C>}           |
|  127 | Exit Group Request          | {"un":"__",“egwho":"__","gid":__}              |
|  128 | Exit Group Notice           | {"egwho":"__","gid":__}                        |
---------------------------------------------------------------------------------------
Explanation:
----------------------------------------------------------------------------------
| un    | username                      |-| fl  | friend list                    |
| pwd   | password                      |-| fun | friend name                    |
| qst   | password reset question no.   |-| fst | friend state(on/offline/muted) |
| ans   | passwordreset question answer |-| gid | group id                       |
| res   | result type                   |-| gn  | group name                     |
| agr   | if agree                      |-| gpw | group power                    |
| npwd  | new password                  |-| gml | group memberlist               |
| afwho | who to add friend             |-| sd  | sender                         |
| dfwho | who to delete friend          |-| rv  | receiver                       |
| mtwho | who to mute                   |-| msg | message                        |
| egwho | who to exit group             |-|     |                                |
----------------------------------------------------------------------------------
---------------------------------------------------
| <0S1F>   | 0=Success,1=Failed                   |
| <0A1D>   | 0=Agree,1=Disagree                   |
| <0O1F2M> | 0=Online,1=Offline,2=Muted           |
| <0L1G2M> | 0=Group Owner,1=Manager,2=Member     |
| <0C1S>   | 0=Client Ask for Server's Publickey  |
|          | 1=Server Ask for Client's Publickey  |
| <0C1S>   | 0=This pkg is Client's Publickey     |
|          | 1=This pkg is Server's Publickey     |
| <0S1C>   | 0=Set Manager,1=Cancel Manager       |
---------------------------------------------------
 ***********************************************************************************/