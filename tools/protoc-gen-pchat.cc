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

#include <fmt/core.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include "source/protocol/common.pb.h"

const char *common_comments = R"(/**
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
)";

const char *service_header_template = R"({common_comments}
#pragma once

#include "source/core/context.h"
#include "source/foundation/foundation.h"
#include "source/protocol/{snakecase_service_name}.pb.h"
#include "source/service/service.h"

namespace pcs::services {{

{service_definition}
struct {proper_service_name}ServiceMapper : public ServiceMapper {{
  int service_id() final {{ return {service_id}; }}

  Ptr<Service> fromMethodId(Context &context, int method_id) final {{
{mapper_definitions}
    return {{}};
  }}

  Ptr<Service> fromMethodName(Context &context, const std::string &method_name) {{
{mapper_name_definitions}
    return {{}};
  }}
}};

}}  // namespace pcs::services
)";

const char *mapper_definition_template = R"(    if (method_id == {method_id}) {{
      return MakePtr<{proper_service_name}>(context);
    }}
)";

const char *mapper_name_definition_template =
    R"(    if (method_name == "{proper_service_name}" || method_name == "{snakecase_service_name}") {{
      return MakePtr<{proper_service_name}>(context);
    }}
)";

const char *service_definition_template = R"(struct {proper_service_name} : public Service {{
  explicit {proper_service_name}(Context &context) : Service(context) {{}}

  Ptr<ProtoMessage> getService(Ptr<ProtoMessage> proto) final;
  Ptr<ProtoMessage> castProtoMessage(const std::string &buffer) final {{ return CastProtoMessage<{request_type_name}>(buffer); }}
}};

)";

const char *service_source_template = R"({common_comments}
#include "{snakecase_service_name}_service.h"

namespace pcs::services {{
{service_implements}
}}  // namespace pcs::services

)";

const char *service_implements_template = R"(
Ptr<ProtoMessage> {method_name}::getService(Ptr<ProtoMessage> proto) {{
  auto req = CastProtoMessage<{request_type_name}>(*proto);

  auto rsp = MakePtr<{response_type_name}>();
  return rsp;
}}
)";

class PchatGenerator : public google::protobuf::compiler::CodeGenerator {
 public:
  ~PchatGenerator() override = default;

  static void WriteToFile(google::protobuf::io::ZeroCopyOutputStream *fd, const std::string &str) {
    char *buffer, *write = const_cast<char *>(str.data());
    int buffer_size, write_size = str.size();
    do {
      fd->Next(reinterpret_cast<void **>(&buffer), &buffer_size);
      if (buffer_size > 0) {
        int s = std::min(write_size, buffer_size);
        memcpy(buffer, write, s);
        write_size -= s;
        buffer_size -= s;
        write += s;
      }
    } while (write_size > 0);
    if (buffer_size > 0) {
      fd->BackUp(buffer_size);
    }
  }

  bool Generate(const google::protobuf::FileDescriptor *file, const std::string &parameter,
                google::protobuf::compiler::GeneratorContext *generator_context, std::string *error) const override {
    for (int i = 0; i < file->service_count(); i++) {
      auto service = file->service(i);

      int service_id = service->options().GetExtension(pcs::opt_service_id);
      auto service_proper_name = service->name();
      auto service_snakecase_name = GenSnakecaseName(service_proper_name);
      std::string service_definitions, service_implements, mapper_definitions, mapper_name_definitions;

      auto header_fd = generator_context->Open(fmt::format("{}_service.h", service_snakecase_name));
      auto source_fd = generator_context->Open(fmt::format("{}_service.cc", service_snakecase_name));

      for (int j = 0; j < service->method_count(); j++) {
        auto method = service->method(j);
        int method_id = method->options().GetExtension(pcs::opt_method_id);
        auto method_proper_name = method->name();
        auto method_snakecase_name = GenSnakecaseName(method_proper_name);

        service_definitions +=
            fmt::format(service_definition_template, fmt::arg("proper_service_name", method_proper_name),
                        fmt::arg("request_type_name", method->input_type()->name()));
        service_implements += fmt::format(service_implements_template, fmt::arg("method_name", method_proper_name),
                                          fmt::arg("request_type_name", method->input_type()->name()),
                                          fmt::arg("response_type_name", method->output_type()->name()));
        mapper_definitions += fmt::format(mapper_definition_template, fmt::arg("method_id", method_id),
                                          fmt::arg("proper_service_name", method_proper_name));
        mapper_name_definitions += fmt::format(mapper_name_definition_template, fmt::arg("method_id", method_id),
                                               fmt::arg("proper_service_name", method_proper_name),
                                               fmt::arg("snakecase_service_name", method_snakecase_name));
      }

      auto header_file_str = fmt::format(
          service_header_template, fmt::arg("common_comments", common_comments), fmt::arg("service_id", service_id),
          fmt::arg("proper_service_name", service_proper_name),
          fmt::arg("snakecase_service_name", service_snakecase_name),
          fmt::arg("service_definition", service_definitions), fmt::arg("mapper_definitions", mapper_definitions),
          fmt::arg("mapper_name_definitions", mapper_name_definitions));

      auto source_file_str = fmt::format(service_source_template, fmt::arg("common_comments", common_comments),
                                         fmt::arg("snakecase_service_name", service_snakecase_name),
                                         fmt::arg("service_implements", service_implements));

      WriteToFile(header_fd, header_file_str);
      WriteToFile(source_fd, source_file_str);
    }
    return true;
  }
  bool GenerateAll(const std::vector<const google::protobuf::FileDescriptor *> &files, const std::string &parameter,
                   google::protobuf::compiler::GeneratorContext *generator_context, std::string *error) const override {
    return CodeGenerator::GenerateAll(files, parameter, generator_context, error);
  }

  [[nodiscard]] uint64_t GetSupportedFeatures() const override { return CodeGenerator::GetSupportedFeatures(); }

 private:
  static std::string GenSnakecaseName(const std::string &proper_name) {
    auto res = std::make_unique<char[]>(static_cast<int>(static_cast<double>(proper_name.size()) * 1.4));
    auto size = 1u;
    res[0] = std::tolower(proper_name[0]);
    for (int i = 1; i < proper_name.size(); i++) {
      res[size++] = std::tolower(proper_name[i]);
      if (i + 1 < proper_name.size() && std::isupper(proper_name[i + 1])) {
        res[size++] = '_';
      }
    }

    return std::string{res.get(), size};
  }
};

int main(int argc, char *argv[]) {
  PchatGenerator mana_generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &mana_generator);
}
