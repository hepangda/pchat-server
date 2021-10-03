#include <iostream>
#include <sstream>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/compiler/code_generator.h>
#include "protocol/pchat.pb.h"
#include <fmt/core.h>
#include <fmt/format.h>
//#include "mysql++/mysql++.h"
//
//template<typename ProtoTypes>
//void ProcessMessage(const ProtoTypes &req, fmt::dynamic_format_arg_store<fmt::format_context> &store) {
//  auto refl = ProtoTypes::GetReflection();
//  auto desc = ProtoTypes::GetDescriptor();
//
//  for (int i = 0; i < desc->field_count(); i++) {
//    auto it = desc->field(i);
//
//    if (it->options().HasExtension(pchat::db_field_name)) {
//      auto field_name = it->options().GetExtension(pchat::db_field_name);
//
//      using namespace google::protobuf;
//#define IMPL_TYPE(type_name, val_getter)                                       \
//  case FieldDescriptor::type_name:                                            \
//    store.push_back(fmt::arg(field_name.c_str(), refl->val_getter(req, it))); \
//    break
//      switch (it->type()) {
////        IMPL_TYPE(TYPE_INT32, GetInt32);
////        IMPL_TYPE(TYPE_INT64, GetInt64);
////        IMPL_TYPE(TYPE_DOUBLE, GetDouble);
////        IMPL_TYPE(TYPE_STRING, GetString);
////        IMPL_TYPE(TYPE_UINT32, GetUInt32);
////        IMPL_TYPE(TYPE_UINT64, GetUInt64);
////        IMPL_TYPE(TYPE_SINT32, GetInt32);
////        IMPL_TYPE(TYPE_SINT64, GetInt64);
////        IMPL_TYPE(TYPE_MESSAGE, GetMessage);
//      }
//#undef IMPL_TYPE
//    }
//  }
//};
//
//template<typename ProtoTypes>
//std::string ProtoToSqls(const ProtoTypes &req, const std::string &sql_template) {
////  fmt::dynamic_format_arg_store<fmt::format_context> store;
////  ProcessMessage(req, store);
////  return fmt::vformat(sql_template, store);
//}

class FuckGenerator : public google::protobuf::compiler::CodeGenerator {
 public:
  ~FuckGenerator() override = default;

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

  bool Generate(const google::protobuf::FileDescriptor *file,
                const std::string &parameter,
                google::protobuf::compiler::GeneratorContext *generator_context,
                std::string *error) const override {
    auto fd = generator_context->Open("framework/pchat.txt");

    for (int i = 0; i < file->service_count(); i++) {
      auto it = file->service(i);

      for (int j = 0; j < it->method_count(); j++) {
        auto jt = it->method(j);

        std::stringstream s;
        s << "rpc " << jt->full_name() << "(" << jt->input_type()->full_name() << ") returns ("
          << jt->output_type()->full_name() << ");\n";
        WriteToFile(fd, s.str());
      }
    }
    return true;
  }
  bool GenerateAll(const std::vector<const google::protobuf::FileDescriptor *> &files,
                   const std::string &parameter,
                   google::protobuf::compiler::GeneratorContext *generator_context,
                   std::string *error) const override {
    return CodeGenerator::GenerateAll(files, parameter, generator_context, error);
  }

  [[nodiscard]] uint64_t GetSupportedFeatures() const override {
    return CodeGenerator::GetSupportedFeatures();
  }

  bool HasGenerateAll() const override {
    return CodeGenerator::HasGenerateAll();
  }
};

int main(int argc, char *argv[]) {
//  pchat::LoginReq req;
//  google::protobuf::Reflection refl;
//  req.set_username("fuck");
//  req.set_magic_val(8000);

//  std::cout << ProtoToSqls(req,
//                           "INSERT INTO user VALUES('{un}', '{abc}') "
//  ) << std::endl;

  FuckGenerator fuck_generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &fuck_generator);
}
