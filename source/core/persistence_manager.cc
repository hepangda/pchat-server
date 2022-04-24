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

#include "persistence_manager.h"

#include <soci/mysql/soci-mysql.h>
#include <soci/soci.h>

#include "source/utils/arguments_manager.h"

namespace pcs {

PersistenceManager::PersistenceManager(Context &context) : context_(context) {}

PersistenceManager::~PersistenceManager() = default;

void PersistenceManager::build() {
  std::call_once(pool_created_, [this] { this->build_(); });
}

void PersistenceManager::build_() {
  UInt32 pool_size = context_.arguments_manager().config().persistence().pool_size();
  auto connect_string = context_.arguments_manager().config().persistence().connect_string();

  connection_pool_ = MakePtr<soci::connection_pool>(pool_size);
  for (UInt32 i = 0; i != pool_size; ++i) {
    soci::session &it = connection_pool_->at(i);
    it.open(soci::mysql, connect_string);
  }
}

Ptr<soci::session> PersistenceManager::session() { return std::move(MakePtr<soci::session>(*connection_pool_)); }

}  // namespace pcs