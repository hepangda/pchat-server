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

#include "arguments_manager.h"

#include <gflags/gflags.h>

#include <thread>

#include "3rdparty/json2pb/json2pb.h"
#include "source/utils/file.h"

namespace pcs {

DEFINE_string(config_file, "./pchat.json", "pchat server config-file path");

ArgumentsManager::ArgumentsManager(Context &context) : context_(context) {}

void ArgumentsManager::parse(int argc, char **argv) {
  argc_ = argc;
  argv_ = argv;

  gflags::ParseCommandLineFlags(&argc_, &argv_, false);

  File config_file{FLAGS_config_file, "r"};
  auto config_file_str = config_file.readToEnd();
  json2pb::json2pb(configs_, config_file_str.data(), config_file_str.size());

  if (configs_.network().has_rep_concurrency_strategy() &&
      configs_.network().rep_concurrency_strategy() == "hardware_limit") {
    configs_.mutable_network()->set_rep_concurrency_count(std::thread::hardware_concurrency());
  }
}

}  // namespace pcs