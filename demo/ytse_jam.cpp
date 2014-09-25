/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/variant.h>
#include <fatal/type/call_traits.h>
#include <fatal/type/prefix_tree.h>
#include <fatal/type/string.h>

#include <folly/Conv.h>
#include <folly/Demangle.h>

#include <iostream>
#include <unordered_map>

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
    if (offset_ < tokens_.size()) { return folly::to<T>(tokens_[offset_++]); }
    throw std::runtime_error("expected: token");
  }

  template <typename T>
  T get(std::size_t index) { return folly::to<T>(tokens_[index + offset_]); }
  std::size_t size() const { return tokens_.size() - offset_; }

private:
  std::vector<std::string> tokens_;
  std::size_t offset_;
};

struct get_member {
  template <typename T> using name = typename T::name;
  template <typename T> using verb = typename T::verb;
  template <typename T> using result = typename T::result;
};

namespace metadata {
namespace str {

FATAL_STR(list, "list"); FATAL_STR(map, "map"); FATAL_STR(string, "string");

FATAL_STR(append, "append"); FATAL_STR(at, "at"); FATAL_STR(create, "create");
FATAL_STR(get, "get"); FATAL_STR(help, "help"); FATAL_STR(insert, "insert");
FATAL_STR(json, "json"); FATAL_STR(size, "size"); FATAL_STR(substr, "substr");

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

template <typename... Args> struct constructor { using args = fatal::type_list<Args...>; };

template <typename...> struct operation;
template <typename TVerb, typename TMethod, typename TResult, typename... Args>
struct operation<TVerb, TMethod, TResult(Args...)> {
  using verb = TVerb;
  using method = TMethod;
  using result = TResult;
  using args = fatal::type_list<Args...>;
};

template <typename TName, typename T, typename... Args>
struct data_type {
  using name = TName;
  using type = T;

private:
  using info = fatal::type_list<Args...>;
  using filtered_ctors = typename info::template separate<fatal::is_template<constructor>::template type>;
  using filtered_ops = typename filtered_ctors::second::template separate<fatal::is_template<operation>::template type>;

public:
  static_assert(filtered_ctors::first::size == 1, "data type needs exactly one constructor");
  using constructor = typename filtered_ctors::first::template at<0>;
  using operations = typename filtered_ops::first;
  using extra_info = typename filtered_ops::second;
};

template <typename TName, typename T, typename TArgs>
struct constructor_command {
  using name = TName;
  using type = T;
  using args = TArgs;
};

template <typename TDataType>
using to_constructor_command = metadata::constructor_command<
  typename TDataType::name, typename TDataType::type, typename TDataType::constructor::args
>;

template <typename TName, typename T, typename TVerb, typename TMethod, typename TResult, typename TArgs>
struct operation_command {
  using name = TName;
  using type = T;
  using verb = TVerb;
  using method = TMethod;
  using result = TResult;
  using args = TArgs;
};

template <typename TDataType>
struct to_operation_command {
  template <typename TOperation>
  using type = metadata::operation_command<
    typename TDataType::name, typename TDataType::type, typename TOperation::verb,
    typename TOperation::method, typename TOperation::result, typename TOperation::args
  >;
};

template <typename TDataType>
using to_operation_command_list = typename TDataType::operations::template transform<
  to_operation_command<TDataType>::template type
>;

//////////////
// metadata //
//////////////

using known = fatal::type_list<
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
  using op_list = supported::transform<metadata::to_operation_command_list>::flatten<1>;
  using instance_t = supported::transform<fatal::get_member_typedef::type>::apply<fatal::auto_variant>;
  using result_t = op_list::transform<get_member::result>::reject<fatal::transform_alias<std::is_same, void>::apply>
    ::unique<>::apply<fatal::auto_variant>;

  result_t handle(std::string const &command, request_args &args);

private:
  using data_type_trie = supported::transform<get_member::name>::apply<fatal::type_prefix_tree_builder<>::build>;
  // data_type_name -> ctor
  using ctor_index = fatal::type_map_from<get_member::name>::list<
    supported::transform<metadata::to_constructor_command>
  >;
  using built_ins = fatal::type_list<metadata::str::create, metadata::str::json, metadata::str::help>;
  using command_trie = op_list::transform<get_member::verb>::concat<built_ins>
    ::apply<fatal::type_prefix_tree_builder<>::build>;
  using op_trie = op_list::transform<get_member::verb>::apply<fatal::type_prefix_tree_builder<>::build>;
  // data_type -> verb -> op
  using op_index = fatal::clustered_index<op_list, fatal::get_member_typedef::type, get_member::verb>;
  using instances_map = std::unordered_map<std::string, instance_t>;

  template <typename T, typename TArgsList, std::size_t... Indexes>
  static void call_ctor(fatal::constant_sequence<std::size_t, Indexes...>, instance_t &instance, request_args &args) {
    instance.template emplace<T>(args.template get<typename TArgsList::template at<Indexes>>(Indexes)...);
  }

  template <typename TMethod, typename TResult, typename TArgsList, typename T, std::size_t... Indexes>
  static void call_method(
    fatal::constant_sequence<std::size_t, Indexes...>, result_t &out, T &&instance, request_args &args
  ) {
    out.set_result_of([&]() {
      return static_cast<TResult>(
        TMethod()(std::forward<T>(instance), args.template get<typename TArgsList::template at<Indexes>>(Indexes)...)
      );
    });
  }

  template <typename TVerb>
  struct call_visitor {
    template <typename T>
    void operator ()(T &&instance, result_t &out, request_args &args) {
      using op_map = op_index::template find<typename std::decay<T>::type>;

      auto found = op_map::template visit<TVerb>([&](auto op_pair) { // type_pair<type_string, operation>
        using op = typename decltype(op_pair)::second;

        if (op::args::size != args.size()) { throw std::invalid_argument("arguments list size mismatch"); }

        using arg_indexes = fatal::constant_range<std::size_t, 0, op::args::size>;

        call_method<typename op::method, typename op::result, typename op::args>(
          arg_indexes(), out, std::forward<T>(instance), args
        );
      });

      if (!found) { throw std::invalid_argument("invalid operation"); }
    }
  };

  struct command_parser {
    template <typename TVerb>
    void operator ()(fatal::type_tag<TVerb>, instances_map &instances, request_args &args, result_t &out) const {
      auto i = instances.find(args.next<std::string>());
      if (i == instances.end()) { throw std::invalid_argument("instance not found"); }
      if (!i->second.visit(call_visitor<TVerb>(), out, args)) {
        throw std::invalid_argument("unitialized instance");
      }
    }

    // built-ins

    void operator ()(
      fatal::type_tag<metadata::str::create>, instances_map &instances, request_args &args, result_t &out
    ) const {
      auto type = args.next<std::string>();
      auto instance = args.next<std::string>();
      auto found = data_type_trie::match<>::exact(
        type.begin(), type.end(),
        [&](auto data_type_name) { // type_tag<type_string>
          ctor_index::visit<typename decltype(data_type_name)::type>(
            [&](auto ctor_pair) { // type_pair<type_string, constructor>
              using ctor = fatal::type_get_second<decltype(ctor_pair)>;
              using arg_indexes = fatal::constant_range<std::size_t, 0, ctor::args::size>;

              if (ctor::args::size != args.size()) { throw std::invalid_argument("arguments list size mismatch"); }

              call_ctor<typename ctor::type, typename ctor::args>(arg_indexes(), instances[instance], args);
            }
          );
        }
      );

      if (!found) { throw std::invalid_argument("unknown type"); }
    }

    void operator ()(
      fatal::type_tag<metadata::str::help>, instances_map &instances, request_args &args, result_t &out
    ) const {
      supported::foreach([](auto data_type_tag) { // indexed_type_tag<data_type>
        using data_type = decltype(data_type_tag);
        if (data_type::value) { std::cout << std::endl; }
        auto const name = data_type::type::name::z_array();
        std::cout << name.data() << '(';
        data_type::type::constructor::args::foreach([](auto arg_tag) { // indexed_type_tag<arg_type>
          using arg = decltype(arg_tag);
          if (arg::value) { std::cout << ", "; }
          std::cout << folly::demangle(typeid(typename arg::type));
        });
        std::cout << ')' << std::endl;

        data_type::type::operations::foreach([](auto op_tag) { // indexed_type_tag<operation>
          using op = typename decltype(op_tag)::type;
          auto const verb = op::verb::z_array();
          std::cout << "- " << verb.data() << '(';
          op::args::foreach([](auto arg_tag) { // indexed_type_tag<arg_type>
            using arg = decltype(arg_tag);
            if (arg::value) { std::cout << ", "; }
            std::cout << folly::demangle(typeid(typename arg::type));
          });
          std::cout << ')' << std::endl;
        });
      });
    }

    void operator ()(
      fatal::type_tag<metadata::str::json>, instances_map &instances, request_args &args, result_t &out
    ) const {
      std::cout << '{' << std::endl;
      supported::foreach([](auto data_type_tag) { // indexed_type_tag<data_type>
        using data_type = typename decltype(data_type_tag)::type;
        auto const name = data_type::name::z_array();
        std::cout << "  \"" << name.data() << "\": {" << std::endl;
        std::cout << "    \"type\": \"" << folly::demangle(typeid(typename data_type::type)) << "\"," << std::endl;
        std::cout << "    \"constructor\": {" << std::endl;
        std::cout << "      \"args\": {" << std::endl;
        data_type::constructor::args::foreach([](auto arg_tag) { // indexed_type_tag<arg_type>
          using arg = decltype(arg_tag);
          std::cout << "        \"" << arg::value << "\": \"" << folly::demangle(typeid(typename arg::type)) << '"';
          if (arg::value + 1 < data_type::constructor::args::size) { std::cout << ','; }
          std::cout << std::endl;
        });
        std::cout << "      }" << std::endl; // args
        std::cout << "    }," << std::endl; // constructor
        std::cout << "    \"operations\": {" << std::endl;
        data_type::operations::foreach([](auto op_tag) { // indexed_type_tag<operation>
          using op = typename decltype(op_tag)::type;
          auto const verb = op::verb::z_array();
          std::cout << "      \"" << verb.data() << "\": {" << std::endl;
          std::cout << "        \"result\": \"" << folly::demangle(typeid(typename op::result)) << "\"," << std::endl;
          std::cout << "        \"args\": {" << std::endl;
          op::args::foreach([](auto arg_tag) { // indexed_type_tag<arg_type>
            using arg = decltype(arg_tag);
            std::cout << "          \"" << arg::value << "\": \"" << folly::demangle(typeid(typename arg::type)) << '"';
            if (arg::value + 1 < op::args::size) { std::cout << ','; }
            std::cout << std::endl;
          });
          std::cout << "        }" << std::endl; // args
          std::cout << "      }"; // operation
          if (decltype(op_tag)::value + 1 < data_type::operations::size) { std::cout << ','; }
          std::cout << std::endl;
        });
        std::cout << "    }" << std::endl; // operations
        std::cout << "  }"; // data_type
        if (decltype(data_type_tag)::value + 1 < supported::size) { std::cout << ','; }
        std::cout << std::endl;
      });
      std::cout << '}' << std::endl;
    }
  };

  instances_map instances_;
};

ytse_jam::result_t ytse_jam::handle(std::string const &command, request_args &args) {
  result_t result;

  if (!command_trie::match<>::exact(command.begin(), command.end(), command_parser(), instances_, args, result)) {
    throw std::invalid_argument("command unknown");
  }

  return result;
}

int main() {
  ytse_jam engine;
  std::cout << "ytse jam db engine: ready" << std::endl << std::endl;

  auto read_request = [](auto &s) -> bool {
    std::cout << "$ ";
    return std::getline(std::cin, s);
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
