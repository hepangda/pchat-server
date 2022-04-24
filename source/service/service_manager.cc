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

#include "service_manager.h"

#include <array>

#include "source/service/account_service.h"
#include "source/service/group_service.h"

namespace pcs {

ServiceManager::ServiceManager(Context &context) : context_(context) {}

ServiceManager::~ServiceManager() = default;

void ServiceManager::build() {
  std::call_once(mapper_built_, [this] {
    register_(MakePtr<services::AccountServiceMapper>());
    register_(MakePtr<services::GroupServiceMapper>());
  });
}

void ServiceManager::register_(Ptr<ServiceMapper> mapper) {
  int service_id = mapper->service_id();
  service_mapper.emplace(service_id, std::move(mapper));
}

Ptr<Service> ServiceManager::getService(int service_id, int methoid_id) {
  if (service_mapper.find(service_id) != service_mapper.end()) {
    return service_mapper[service_id]->fromMethodId(context_, methoid_id);
  }
  return {};
}

Ptr<Service> ServiceManager::getService(int service_id, const std::string &method_name) {
  if (service_mapper.find(service_id) != service_mapper.end()) {
    return service_mapper[service_id]->fromMethodName(context_, method_name);
  }
  return {};
}

}  // namespace pcs