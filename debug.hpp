#if __has_include(<cpp-dump.hpp>)
#include <cpp-dump.hpp>
namespace cp = cpp_dump;
CPP_DUMP_SET_OPTION_GLOBAL(max_line_width, 100);
CPP_DUMP_SET_OPTION_GLOBAL(max_iteration_count, 20);
CPP_DUMP_SET_OPTION_GLOBAL(max_depth, 5);
CPP_DUMP_SET_OPTION_GLOBAL(log_label_func, cp::log_label::filename());
CPP_DUMP_SET_OPTION_GLOBAL(enable_asterisk, true);
#define dump(...) cpp_dump(__VA_ARGS__)
#endif
