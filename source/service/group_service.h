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
#include "source/protocol/group.pb.h"
#include "source/service/service.h"

namespace pcs::services {

struct CreateGroup : public Service {
  explicit CreateGroup(Context &context) : Service(context) {}

  Ptr<ProtoMessage> serve(Ptr<ProtoMessage> proto) final;
  Ptr<ProtoMessage> castProtoMessage(const std::string &buffer) final { return CastProtoMessage<CreateGroupReq>(buffer); }
};


struct GroupServiceMapper : public ServiceMapper {
  int service_id() final { return 11; }

  Ptr<Service> fromMethodId(Context &context, int method_id) final {
    if (method_id == 110) {
      return MakePtr<CreateGroup>(context);
    }

    return {};
  }

  Ptr<Service> fromMethodName(Context &context, const std::string &method_name) {
    if (method_name == "CreateGroup" || method_name == "create_group") {
      return MakePtr<CreateGroup>(context);
    }

    return {};
  }
};

}  // namespace pcs::services
