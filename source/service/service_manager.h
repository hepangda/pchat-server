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

#include <mutex>
#include <unordered_map>

#include "source/core/context.h"
#include "source/foundation/foundation.h"
#include "source/service/service.h"

namespace pcs {

class ServiceManager {
 public:
  explicit ServiceManager(Context &context);
  ~ServiceManager();

  Ptr<Service> getService(int service_id, int methoid_id);

  Ptr<Service> getService(int service_id, const std::string &method_name);

  void build();

 private:
  void register_(Ptr<ServiceMapper> mapper);

  Context &context_;
  std::unordered_map<int, Ptr<ServiceMapper>> service_mapper;
  std::once_flag mapper_built_;
};

}  // namespace pcs
