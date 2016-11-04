/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/variant.h>
#include <fatal/test/string.h>
#include <fatal/test/type.h>
#include <fatal/type/call_traits.h>
#include <fatal/type/find.h>
#include <fatal/type/foreach.h>
#include <fatal/type/get.h>
#include <fatal/type/get_type.h>
#include <fatal/type/sequence.h>
#include <fatal/type/transform.h>
#include <fatal/type/trie.h>
#include <fatal/type/type.h>
#include <fatal/type/unique.h>

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace fatal;

struct request_args {
  explicit request_args(std::string const &s): offset_(0) {
    // poor man's request_args
    std::istringstream args(s);
    for (std::string token; args >> token; ) {
      tokens_.push_back(std::move(token));
    }
  }

  template <typename T>
  T next() {
    if (offset_ < tokens_.size()) { return parse<T>(tokens_[offset_++]); }
    throw std::runtime_error("expected: token");
  }

  template <typename T>
  T get(std::size_t index) { return parse<T>(tokens_[index + offset_]); }
  std::size_t size() const { return tokens_.size() - offset_; }

private:
  std::vector<std::string> tokens_;
  std::size_t offset_;
};

namespace metadata {
namespace str {

FATAL_S(list, "list"); FATAL_S(map, "map"); FATAL_S(string, "string");

FATAL_S(append, "append"); FATAL_S(at, "at"); FATAL_S(create, "create");
FATAL_S(get, "get"); FATAL_S(help, "help"); FATAL_S(insert, "insert");
FATAL_S(json, "json"); FATAL_S(size, "size"); FATAL_S(substr, "substr");

} // namespace str {

struct method {
  FATAL_CALL_TRAITS(operator_square_bracket, operator []);
  FATAL_CALL_TRAITS(append, append);
  FATAL_CALL_TRAITS(at, at);
  FATAL_CALL_TRAITS(c_str, c_str);
  FATAL_CALL_TRAITS(emplace, emplace);
  FATAL_CALL_TRAITS(emplace_back, emplace_back);
  FATAL_CALL_TRAITS(size, size);
  FATAL_CALL_TRAITS(substr, substr);
};

template <typename... Args> struct constructor { using args = list<Args...>; };

template <typename...> struct operation;
template <typename Verb, typename Method, typename Result, typename... Args>
struct operation<Verb, Method, Result(Args...)> {
  using verb = Verb;
  using method = Method;
  using result = Result;
  using args = list<Args...>;
};

template <typename Name, typename T, typename Constructor, typename... Operations>
struct data_type {
  using name = Name;
  using type = T;
  using constructor = Constructor;
  using operations = list<Operations...>;
};

template <typename Name, typename T, typename Args>
struct constructor_command {
  using name = Name;
  using type = T;
  using args = Args;
};

template <typename DataType>
using to_constructor_command = metadata::constructor_command<
  typename DataType::name, typename DataType::type, typename DataType::constructor::args
>;

template <typename Name, typename T, typename Verb, typename Method, typename Result, typename Args>
struct operation_command {
  using name = Name;
  using type = T;
  using verb = Verb;
  using method = Method;
  using result = Result;
  using args = Args;
};

template <typename DataType>
struct to_operation_command {
  template <typename Operation>
  using apply = metadata::operation_command<
    typename DataType::name, typename DataType::type, typename Operation::verb,
    typename Operation::method, typename Operation::result, typename Operation::args
  >;
};

template <typename DataType>
using to_operation_command_list = transform<
  typename DataType::operations,
  to_operation_command<DataType>
>;

//////////////
// metadata //
//////////////

using known = list<
  data_type<
    str::list, std::vector<std::string>,
    constructor<>,
    operation<str::at, method::at::member_function, std::string(std::size_t)>,
    operation<str::insert, method::emplace_back::member_function, void(std::string)>,
    operation<str::size, method::size::member_function, std::size_t()>
  >,
  data_type<
    str::string, std::string,
    constructor<std::string>,
    operation<str::get, method::c_str::member_function, std::string()>,
    operation<str::substr, method::substr::member_function, std::string(std::size_t, std::size_t)>,
    operation<str::append, method::append::member_function, void(std::string)>,
    operation<str::size, method::size::member_function, std::size_t()>
  >,
  data_type<
    str::map, std::unordered_map<std::string, std::string>,
    constructor<>,
    operation<str::get, method::operator_square_bracket::member_function, std::string(std::string)>,
    operation<str::insert, method::emplace::member_function, void(std::string, std::string)>,
    operation<str::size, method::size::member_function, std::size_t()>
  >
>;

} // namespace metadata {

struct ytse_jam {
  using supported = metadata::known;
  using op_list = apply_to<transform<supported, applier<metadata::to_operation_command_list>>, cat>;
  using instance_t = apply_to<transform<supported, get_type::type>, auto_variant>;
  // TODO: unique BEFORE apply_to
  using result_t = apply_to<
    reject<transform<op_list, get_type::result>, curry<applier<std::is_same>, void>>,
    auto_variant
  >;

  result_t handle(std::string const &command, request_args &args);

private:
  using built_ins = list<metadata::str::create, metadata::str::json, metadata::str::help>;
  using instances_map = std::unordered_map<std::string, instance_t>;

  template <typename T, typename ArgsList, std::size_t... Indexes>
  static void call_ctor(index_sequence<Indexes...>, instance_t &instance, request_args &args) {
    instance.template emplace<T>(args.request_args::template get<at<ArgsList, Indexes>>(Indexes)...);
  }

  template <typename Method, typename Result, typename ArgsList, typename T, std::size_t... Indexes>
  static void call_method(
    index_sequence<Indexes...>, result_t &out, T &&instance, request_args &args
  ) {
    out.set_result_of([&]() {
      return static_cast<Result>(
        Method()(std::forward<T>(instance), args.request_args::template get<at<ArgsList, Indexes>>(Indexes)...)
      );
    });
  }

  template <typename Verb>
  struct call_visitor {
    template <typename T>
    void perform(not_found, T &, result_t &, request_args &) {
      throw std::invalid_argument("invalid operation");
    }

    template <typename Operation, typename T>
    void perform(Operation, T &instance, result_t &out, request_args &args) {
      if (size<typename Operation::args>::value != args.size()) {
        throw std::invalid_argument("arguments list size mismatch");
      }

      using arg_indexes = make_index_sequence<size<typename Operation::args>::value>;

      call_method<typename Operation::method, typename Operation::result, typename Operation::args>(
        arg_indexes(), out, instance, args
      );
    }

    template <typename T>
    void operator ()(T &instance, result_t &out, request_args &args) {
      using data_type = get<supported, T, get_type::type>;
      using operation = find<typename data_type::operations, Verb, not_found, get_type::verb>;

      perform(operation(), instance, out, args);
    }
  };

  struct command_parser {
    template <typename Verb>
    void operator ()(tag<Verb>, instances_map &instances, request_args &args, result_t &out) const {
      auto i = instances.find(args.next<std::string>());
      if (i == instances.end()) {
        throw std::invalid_argument("instance not found");
      }

      if (!i->second.visit(call_visitor<Verb>(), out, args)) {
        throw std::invalid_argument("unitialized instance");
      }
    }

    // built-ins

    void operator ()(
      tag<metadata::str::create>, instances_map &instances, request_args &args, result_t &
    ) const {
      // data_type_name -> ctor
      using ctor_index = transform<supported, applier<metadata::to_constructor_command>>;

      auto type = args.next<std::string>();
      auto instance = args.next<std::string>();
      auto found = trie_find<transform<supported, get_type::name>>(
        type.begin(), type.end(),
        [&](auto data_type_name) { // tag<sequence>
          using ctor = get<ctor_index, type_of<decltype(data_type_name)>, get_type::name>;
          using arg_indexes = make_index_sequence<size<typename ctor::args>::value>;

          if (size<typename ctor::args>::value != args.size()) {
            throw std::invalid_argument("arguments list size mismatch");
          }

          call_ctor<typename ctor::type, typename ctor::args>(arg_indexes(), instances[instance], args);
        }
      );

      if (!found) {
        throw std::invalid_argument("unknown type");
      }
    }

    void operator ()(
      tag<metadata::str::help>, instances_map &, request_args &, result_t &
    ) const {
      foreach<supported>([](auto data_type_tag) { // indexed<data_type>
        using data_type = type_of<decltype(data_type_tag)>;
        if (decltype(data_type_tag)::value) { std::cout << std::endl; }
        std::cout << z_data<typename data_type::name>() << '(';
        foreach<typename data_type::constructor::args>([](auto arg_tag) { // indexed<arg_type>
          using arg = type_of<decltype(arg_tag)>;
          if (decltype(arg_tag)::value) { std::cout << ", "; }
          std::cout << type_str<arg>();
        });
        std::cout << ')' << std::endl;

        foreach<typename data_type::operations>([](auto op_tag) { // indexed<operation>
          using op = type_of<decltype(op_tag)>;
          std::cout << "- " << z_data<typename op::verb>() << '(';
          foreach<typename op::args>([](auto arg_tag) { // indexed<arg_type>
            using arg = type_of<decltype(arg_tag)>;
            if (decltype(arg_tag)::value) { std::cout << ", "; }
            std::cout << type_str<arg>();
          });
          std::cout << ')' << std::endl;
        });
      });
    }

    void operator ()(
      tag<metadata::str::json>, instances_map &, request_args &, result_t &
    ) const {
      std::cout << '{' << std::endl;
      foreach<supported>([](auto data_type_tag) { // indexed<data_type>
        using data_type = type_of<decltype(data_type_tag)>;
        std::cout << "  \"" << z_data<typename data_type::name>() << "\": {" << std::endl;
        std::cout << "    \"type\": \"" << type_str<typename data_type::type>() << "\"," << std::endl;
        std::cout << "    \"constructor\": {" << std::endl;
        std::cout << "      \"args\": {" << std::endl;
        foreach<typename data_type::constructor::args>([](auto arg_tag) { // indexed<arg_type>
          using arg = type_of<decltype(arg_tag)>;
          std::cout << "        \"" << decltype(arg_tag)::value << "\": \"" << type_str<arg>() << '"';
          if (decltype(arg_tag)::value + 1 < size<typename data_type::constructor::args>::value) { std::cout << ','; }
          std::cout << std::endl;
        });
        std::cout << "      }" << std::endl; // args
        std::cout << "    }," << std::endl; // constructor
        std::cout << "    \"operations\": {" << std::endl;
        foreach<typename data_type::operations>([](auto op_tag) { // indexed<operation>
          using op = type_of<decltype(op_tag)>;
          std::cout << "      \"" << z_data<typename op::verb>() << "\": {" << std::endl;
          std::cout << "        \"result\": \"" << type_str<typename op::result>() << "\"," << std::endl;
          std::cout << "        \"args\": {" << std::endl;
          foreach<typename op::args>([](auto arg_tag) { // indexed<arg_type>
            using arg = type_of<decltype(arg_tag)>;
            std::cout << "          \"" << decltype(arg_tag)::value << "\": \"" << type_str<arg>() << '"';
            if (decltype(arg_tag)::value + 1 < size<typename op::args>::value) { std::cout << ','; }
            std::cout << std::endl;
          });
          std::cout << "        }" << std::endl; // args
          std::cout << "      }"; // operation
          if (decltype(op_tag)::value + 1 < size<typename data_type::operations>::value) { std::cout << ','; }
          std::cout << std::endl;
        });
        std::cout << "    }" << std::endl; // operations
        std::cout << "  }"; // data_type
        if (decltype(data_type_tag)::value + 1 < size<supported>::value) { std::cout << ','; }
        std::cout << std::endl;
      });
      std::cout << '}' << std::endl;
    }
  };

  instances_map instances_;
};

ytse_jam::result_t ytse_jam::handle(std::string const &command, request_args &args) {
  result_t result;

  if (!trie_find<adjacent_unique<sort<cat<built_ins, transform<op_list, get_type::verb>>, sequence_compare<less>>>>(
    command.begin(), command.end(), command_parser(), instances_, args, result
  )) {
    throw std::invalid_argument("command unknown");
  }

  return result;
}

int main() {
  ytse_jam engine;
  std::cout << "ytse jam db engine: ready" << std::endl << std::endl;

  auto read_request = [](auto &s) {
    std::cout << "$ ";
    return static_cast<bool>(std::getline(std::cin, s));
  };

  for (std::string request; read_request(request); std::cout << std::endl) {
    try {
      request_args args(request);
      auto command = args.next<std::string>();
      auto result = engine.handle(command, args);
      result.visit([](auto const &value) { std::cout << "result: " << std::boolalpha << value << std::endl; });
    } catch (std::exception const &e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
    }
  }

  return 0;
}
