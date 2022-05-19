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

#pragma once

#include "source/core/context.h"
#include "source/foundation/foundation.h"
#include "source/protocol/account.pb.h"
#include "source/service/service.h"

namespace pcs::services {

struct Login : public Service {
  explicit Login(Context &context) : Service(context) {}

  Ptr<ProtoMessage> serve(Ptr<ProtoMessage> proto) final;
  Ptr<ProtoMessage> castProtoMessage(const std::string &buffer) final { return CastProtoMessage<LoginReq>(buffer); }
};

struct RegisterAccount : public Service {
  explicit RegisterAccount(Context &context) : Service(context) {}

  Ptr<ProtoMessage> serve(Ptr<ProtoMessage> proto) final;
  Ptr<ProtoMessage> castProtoMessage(const std::string &buffer) final { return CastProtoMessage<RegisterReq>(buffer); }
};

struct ResetPassword : public Service {
  explicit ResetPassword(Context &context) : Service(context) {}

  Ptr<ProtoMessage> serve(Ptr<ProtoMessage> proto) final;
  Ptr<ProtoMessage> castProtoMessage(const std::string &buffer) final { return CastProtoMessage<ResetReq>(buffer); }
};

struct AccountServiceMapper : public ServiceMapper {
  int service_id() final { return 10; }

  Ptr<Service> fromMethodId(Context &context, int method_id) final {
    if (method_id == 1) {
      return MakePtr<Login>(context);
    }
    if (method_id == 2) {
      return MakePtr<RegisterAccount>(context);
    }
    if (method_id == 3) {
      return MakePtr<ResetPassword>(context);
    }

    return nullptr;
  }

  Ptr<Service> fromMethodName(Context &context, const std::string &method_name) final {
    if (method_name == "Login" || method_name == "login") {
      return MakePtr<Login>(context);
    }
    if (method_name == "RegisterAccount" || method_name == "register_account") {
      return MakePtr<RegisterAccount>(context);
    }
    if (method_name == "ResetPassword" || method_name == "reset_password") {
      return MakePtr<ResetPassword>(context);
    }

    return nullptr;
  }
};

}  // namespace pcs::services
