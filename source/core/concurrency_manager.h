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

#include <thread>
#include <vector>

#include "source/core/context.h"
#include "source/foundation/foundation.h"

namespace pcs {

class ConcurrencyManager {
 public:
  explicit ConcurrencyManager(Context& context);
  ~ConcurrencyManager();

  template <typename Callable>
  void async(Callable callable) {
    threads_.emplace_back(callable);
  }

  void joinAll();

 private:
  Context& context_;

  std::vector<std::thread> threads_;
};

}  // namespace pcs
