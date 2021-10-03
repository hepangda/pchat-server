use pcp_protocol::pcp_packet;

pub struct PcpPacket {
    opr_id: u16,
    data_size: u16,
    json_data: String,
}

pub trait GenericPcpPacket {
    fn pcp_opr_id() -> u16;
    fn gen_packet(&self) -> PcpPacket;
}

#[derive(pcp_packet)]
#[pcp_opr_id(100)]
pub struct LoginReq {
    pub username: String,
    pub password: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(101)]
pub struct LoginRsp {
    username: String,
    result: i32,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(102)]
pub struct RegisterReq {
    username: String,
    password: String,
    question: i32,
    answer: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(103)]
pub struct RegisterRsp {
    username: String,
    result: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(104)]
pub struct ResetPasswordReq {
    username: String,
    new_password: String,
    question: i32,
    answer: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(105)]
pub struct ResetPasswordRsp {
    username: String,
    result: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(106)]
pub struct AddFriendReq {
    username: String,
    friend_username: String,
    token: String,
}


#[derive(pcp_packet)]
#[pcp_opr_id(107)]
pub struct AddFriendRsp {
    username: String,
    friend_username: String,
    result: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(110)]
pub struct AddFriendApplication {
    username: String,
    friend_username: String,
    agreement: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(108)]
pub struct DeleteFriendReq {
    username: String,
    friend_username: String,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(109)]
pub struct DeleteFriendRsp {
    username: String,
    friend_username: String,
    result: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(111)]
pub struct OnlineNotice {
    username: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(112)]
pub struct OfflineNotice {
    username: String,
}


#[derive(pcp_packet)]
#[pcp_opr_id(113)]
pub struct RefreshFriendListReq {
    username: String,
    token: String,
}

pub struct Friend {
    username: String,
    state: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(114)]
pub struct RefreshFriendListRsp {
    username: String,
    result: i32,
    friend_list: Vec<Friend>,
}

#[derive(pcp_packet)]
#[pcp_opr_id(115)]
pub struct RefreshGroupListReq {
    username: String,
    token: String,
}

pub struct Group {
    groupname: String,
    privilege: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(116)]
pub struct RefreshGroupListRsp {
    username: String,
    res: i32,
    group_list: Vec<Group>,
}

#[derive(pcp_packet)]
#[pcp_opr_id(117)]
pub struct RefreshGroupMemberReq {
    username: String,
    groupname: String,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(118)]
pub struct RefreshGroupMemberRsp {
    username: String,
    result: i32,
    group_member_list: Vec<String>,
}

#[derive(pcp_packet)]
#[pcp_opr_id(119)]
pub struct GroupChatMsg {
    groupname: String,
    sender: String,
    message: String,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(120)]
pub struct FriendChatMsg {
    sender: String,
    receiver: String,
    message: String,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(121)]
pub struct PublicKeyReq {
    username: String,
    key_type: i32,
    pub_key: String,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(122)]
pub struct PublicKeyRsp {
    username: String,
    key_type: i32,
    pub_key: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(123)]
pub struct MuteReq {
    username: String,
    mute_username: String,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(124)]
pub struct MuteRsp {
    username: String,
    mute_username: String,
    result: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(125)]
pub struct SetManagerReq {
    username: String,
    manager_username: String,
    group_name: String,
    set_type: i32,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(126)]
pub struct SetManagerNotice {
    manager_username: String,
    group_name: String,
    set_type: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(127)]
pub struct ExitGroupReq {
    username: String,
    group_name: String,
    exit_name: String,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(128)]
pub struct ExitGroupNotice {
    group_name: String,
    exit_name: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(129)]
pub struct CreateGroupReq {
    username: String,
    groupname: String,
    token: String
}

#[derive(pcp_packet)]
#[pcp_opr_id(139)]
pub struct CreateGroupRsp {
    username: String,
    groupname: String,
    result: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(130)]
pub struct AddToGroupReq {
    username: String,
    groupname: String,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(132)]
pub struct AddToGroupRsp {
    username: String,
    groupname: String,
    result: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(133)]
pub struct DismissGroupReq {
    username: String,
    groupname: String,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(134)]
pub struct DismissGroupNotice {
    username: String,
    groupname: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(135)]
pub struct DismuteReq {
    username: String,
    mute_username: String,
    token: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(136)]
pub struct DismuteRsp {
    username: String,
    mute_username: String,
    result: i32,
}

#[derive(pcp_packet)]
#[pcp_opr_id(137)]
pub struct FetchRecordReq {
    username: String,
    record_type: i32,
    group_name: String,
    friend_name: String,
    token: String,

}

pub struct Record {
    from_username: String,
    to_username: String,
    content: String,
}

#[derive(pcp_packet)]
#[pcp_opr_id(138)]
pub struct FetchRecordRsp {
    username: String,
    record_type: i32,
    records_list: Vec<Record>,
}

#[derive(pcp_packet)]
#[pcp_opr_id(140)]
pub struct ExitGroupPersonalNotice {
    username: String,
    group_name: String,
}
