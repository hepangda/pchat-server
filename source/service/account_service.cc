/**
*  Pchat-server: a simple chatroom server
*  Copyright (C) 2017- Pangda

*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, version 3 of the License

*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.

*  You should have received a copy of the GNU General Public License
*  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "account_service.h"

#include <fmt/format.h>
#include <soci/soci.h>

#include "source/core/persistence_manager.h"

namespace pcs::services {

Ptr<ProtoMessage> Login::serve(Ptr<ProtoMessage> proto) {
  static const char *sql_template = "SELECT uid FROM `Account` WHERE username='{}' AND passwd='{}' LIMIT 1";

  auto req = CastProtoMessage<LoginReq>(*proto);
  auto session = context().persistence_manager().session();
  auto sql = fmt::format(sql_template, req->username(), req->password());

  Int64 uid = -1;
  (*session) << sql, soci::into(uid);

  auto rsp = MakePtr<LoginRsp>();
  if (uid < 0) {
    rsp->set_status(ASS_InvalidLoginInfo);
  } else {
    rsp->set_status(ASS_Ok);
    rsp->set_uid(uid);
  }
  return rsp;
}

Ptr<ProtoMessage> RegisterAccount::serve(Ptr<ProtoMessage> proto) {
  static const char *sql_template =
      "INSERT INTO `Account`(uid,username,passwd,reset_question,reset_answer) "
      "(DEFAULT,'{}','{}',{},'{}')";

  auto req = CastProtoMessage<RegisterReq>(*proto);
  auto session = context().persistence_manager().session();
  auto sql = fmt::format(sql_template, req->username(), req->password(), req->reset_question(), req->reset_answer());

  (*session) << sql;
  Int64 user_id;
  session->get_last_insert_id("Account", user_id);

  auto rsp = MakePtr<RegisterRsp>();
  rsp->set_status(ASS_Ok);
  rsp->set_uid(user_id);
  return rsp;
}

Ptr<ProtoMessage> ResetPassword::serve(Ptr<ProtoMessage> proto) {
  static const char *sql_template_with_uid =
      "UPDATE `Account` SET passwd='{}' WHERE uid={} AND reset_question={} AND reset_answer='{}' LIMIT 1";
  static const char *sql_template_with_username =
      "UPDATE `Account` SET passwd='{}' WHERE username={} AND reset_question={} AND reset_answer='{}' LIMIT 1";

  auto req = CastProtoMessage<ResetReq>(*proto);
  auto session = context().persistence_manager().session();
  std::string sql;
  if (req->uid() == 0) {
    sql = fmt::format(sql_template_with_username, req->new_password(), req->username(), req->reset_question(),
                      req->reset_answer());
  } else {
    sql = fmt::format(sql_template_with_uid, req->new_password(), req->uid(), req->reset_question(), req->reset_answer());
  }

  soci::statement statement = (session->prepare << sql);
  statement.execute();

  auto rsp = MakePtr<ResetRsp>();
  if (statement.get_affected_rows() == 0) {
    rsp->set_status(ASS_InvalidResetInfo);
  } else {
    rsp->set_status(ASS_Ok);
  }
  return rsp;
}

}  // namespace pcs::services
