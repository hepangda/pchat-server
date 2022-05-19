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

#include "group_service.h"

namespace pcs::services {

Ptr<ProtoMessage> CreateGroup::serve(Ptr<ProtoMessage> proto) {
  auto req = CastProtoMessage<CreateGroupReq>(*proto);

  auto rsp = MakePtr<CreateGroupRsp>();
  return rsp;
}

}  // namespace pcs::services

