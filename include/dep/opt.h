
#ifndef later_than_cxx14
#if (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define msvc_later_than_cxx14 true
#else
#define msvc_later_than_cxx14 false
#endif
#define later_than_cxx14 (msvc_later_than_cxx14 || __cplusplus > 201402L)
#endif

#if later_than_cxx14

#include <string>

#else
#include <cstring>
#endif

#include <vector>
#include <map>
#include <functional>
#include <mutex>
#include <cassert>
#include <cstdint>
#include <exception>
#include <fmt/core.h>
#include <fmt/format.h>
#include <algorithm>
#include <sstream>

namespace dep {

#if defined(later_than_cxx14) && (later_than_cxx14 == true)
    using StringRef = std::string_view;

    int get_length_of_string_ref(StringRef ref) {
        return ref.length();
    }

    StringRef string_ref_substring(StringRef ref, int l, int r) {
        assert(l <= r);
        return ref.substr(l, r - l);
    }

#ifndef _no_discard
#define _no_discard [[nodiscard]]
#endif
#ifndef _maybe_unused
#define _maybe_unused [[maybe_unused]]
#endif
#else
    using StringRef = const char *;
    int get_length_of_string_ref(StringRef ref) {
        return strlen(ref);
    }

    std::string string_ref_substring(StringRef ref, int l, int r) {
        assert(l <= r);
        return std::string(ref+l, r-l);
    }

#ifndef _no_discard
#define _no_discard;
#endif
#ifndef _maybe_unused
#define _maybe_unused
#endif
#endif
    using opt_has_value = bool;
    using OptMap = std::map<std::string, std::pair<std::string, opt_has_value>>;

#ifndef USE_MY_BUILD_OPT_MAP

    namespace internal {
        bool split(StringRef ref, std::string &k, std::pair<std::string, opt_has_value> &v) {
            int len = get_length_of_string_ref(ref);
            for (int i = 0; i < len; i++) {
                if (ref[i] == '=') {
                    k = string_ref_substring(ref, 0, i);
                    v = {std::string(string_ref_substring(ref, i + 1, len)), true};
                    return true;
                }
            }
            k = ref;
            return v.second = false;
        }

        using long_opt_t = StringRef;
        using short_opt_t = StringRef;
        using category_t = StringRef;
        using desc_t = StringRef;
        using default_value_t = StringRef;

        using help_row_t = std::tuple<
                internal::long_opt_t, internal::short_opt_t, internal::category_t,
                internal::desc_t, internal::default_value_t>;

        StringRef get_long_opt(const help_row_t &row) {
            return std::get<0>(row);
        }

        StringRef get_short_opt(const help_row_t &row) {
            return std::get<1>(row);
        }

        StringRef get_category(const help_row_t &row) {
            return std::get<2>(row);
        }

        StringRef get_desc(const help_row_t &row) {
            return std::get<3>(row);
        }

        StringRef get_default_value(const help_row_t &row) {
            return std::get<4>(row);
        }

#define MAX_STRING_HEAP_SIZE 1024 * 16
        static std::string string_heap[MAX_STRING_HEAP_SIZE];
        static int heap_back = 0;

        static std::mutex string_heap_mutex;

        template<typename Str>
        std::string &alloc_string(Str &&str) {
            string_heap_mutex.lock();
            string_heap[heap_back] = std::forward<Str>(str);
            std::string &heap_string = string_heap[heap_back++];
            string_heap_mutex.unlock();
            return heap_string;
        }

        template<int no_sep>
        int find_sep(StringRef desc, int l, int r) {
            if (r - l < no_sep) {
                return r;
            }
            int res = r - 1;
            while (l < res) {
                if (isspace(desc[res])) {
                    return res + 1;
                }
                res--;
            }
            return r;
        }

        const int desc_width = 74;

        std::string format_description(StringRef desc) {
            int len = get_length_of_string_ref(desc);
            if (len <= desc_width) {
                return std::string(desc);
            }
            std::stringstream ss;
            int slice_left = 0;
            int nc = find_sep<desc_width>(desc, slice_left, std::min(slice_left + desc_width, len));
            ss << string_ref_substring(desc, 0, nc);
            slice_left = nc;
            nc = find_sep<desc_width>(desc, slice_left, std::min(slice_left + desc_width, len));
            while (slice_left < len) {
                ss << fmt::format("\n{: >36}{}", "", "",
                                  string_ref_substring(desc, slice_left, nc));
                slice_left = nc;
                nc = find_sep<desc_width>(desc, slice_left, std::min(slice_left + desc_width, len));
            }

            return ss.str();
        }
    }

    void buildMap(OptMap &map, int &argc, const char **argv) {
        std::string k;
        std::pair<std::string, opt_has_value> v;
        int not_parsed_cursor = 0, omit_prefix_len;
        for (int i = 0; i < argc; i++) {
            if (strncmp(argv[i], "--", 2) == 0) {
                omit_prefix_len = 2;
            } else if (strncmp(argv[i], "-", 1) == 0) {
                omit_prefix_len = 1;
            } else {
                argv[not_parsed_cursor++] = argv[i];
                continue;
            }

            if (!internal::split(argv[i] + omit_prefix_len, k, v)) {
                if (i + 1 < argc && strcmp(argv[i + 1], "-") == 0) {
                    map[k] = {argv[i++], true};
                } else {
                    map[k] = {"", false};
                }
            } else {
                map[k] = v;
            }
        }
        argc = not_parsed_cursor;
    }

#endif

    class Opt {
    protected:
        StringRef long_opt;
        StringRef desc;
        StringRef short_opt;
        StringRef category;

        explicit Opt(StringRef long_opt, StringRef desc, StringRef short_opt, StringRef category) :
                long_opt(long_opt), desc(desc), short_opt(short_opt), category(category) {}

        void operator()(OptMap &opts,
                        const std::function<void(const std::pair<std::string, bool> &)> &parsed_callback,
                        const std::function<void()> &not_parsed_callback) {
            if (get_length_of_string_ref(short_opt) != 0 && opts.count(std::string(short_opt))) {
                parsed_callback(opts.at(std::string(short_opt)));
                opts.erase(std::string(short_opt));
                return;
            } else if (get_length_of_string_ref(long_opt) != 0 && opts.count(std::string(long_opt))) {
                parsed_callback(opts.at(std::string(long_opt)));
                opts.erase(std::string(long_opt));
                return;
            } else {
                not_parsed_callback();
            }
        }
    };

    struct InvalidConversion : std::invalid_argument {
        explicit InvalidConversion(const std::string &msg)
        noexcept: std::invalid_argument(msg) {}
    };

    class StringOpt : public Opt {
        std::string *storage;
        const char *default_value;
    public:
        StringOpt() noexcept:
                Opt("", "", "", ""), storage(nullptr), default_value("") {}

        StringOpt(StringRef long_opt, std::string *storage,
                  StringRef desc = "", StringRef category = "", const char *default_value = "",
                  StringRef short_opt = "")
                : Opt(long_opt, desc, short_opt, category), storage(storage), default_value(default_value) {
        }

        void operator()(OptMap &opts) {
            if (storage != nullptr) {
                Opt::operator()(opts, [this](auto parsed_value) {
                    *storage = parsed_value.first;
                }, [this]() {
                    *storage = default_value;
                });
            }
        }
    };

    class Int64Opt : public Opt {
        int64_t *storage;
        int64_t default_value;
    public:
        Int64Opt() noexcept:
                Opt("", "", "", ""), storage(nullptr), default_value(0) {}

        Int64Opt(StringRef long_opt, int64_t *storage,
                 StringRef desc = "", StringRef category = "", int64_t default_value = 0, StringRef short_opt = "")
                : Opt(long_opt, desc, short_opt, category), storage(storage), default_value(default_value) {
        }

        void operator()(OptMap &opts) {
            if (storage != nullptr) {
                Opt::operator()(opts, [this](auto parsed_value) {
                    try {
                        *storage = std::stoll(parsed_value.first);
                    } catch (std::exception &e) {
                        throw InvalidConversion(
                                fmt::format("cant not convert {} to {}(int64):",
                                            parsed_value.first, long_opt) + std::string(e.what()));
                    }

                }, [this]() {
                    *storage = default_value;
                });
            }
        }
    };

    class BooleanOpt : public Opt {
        bool *storage;
        bool default_value;
    public:
        BooleanOpt() noexcept:
                Opt("", "", "", ""), storage(nullptr), default_value(false) {}

        BooleanOpt(StringRef long_opt, bool *storage,
                   StringRef desc = "", StringRef category = "", bool default_value = false, StringRef short_opt = "")
                : Opt(long_opt, desc, short_opt, category), storage(storage), default_value(default_value) {
        }

        void operator()(OptMap &opts) {
            if (storage != nullptr) {
                Opt::operator()(opts, [this](auto parsed_value) {
                    *storage = true;
                }, [this]() {
                    *storage = default_value;
                });
            }
        }
    };

    class OptParser {
    public:
        using ParseCallback = std::function<
                void(OptMap &opts)>;

    protected:
        bool _parsed = false;

        std::mutex mutex;

        std::vector<ParseCallback> callbacks;
        std::vector<internal::help_row_t> help_messages;

    public:
        OptParser() = default;

        // not copyable
        OptParser(OptParser &&parser) noexcept:
                _parsed(parser._parsed),
                callbacks(std::move(parser.callbacks)),
                help_messages(std::move(parser.help_messages)) {}

        const OptParser &operator=(const OptParser &) = delete;

        void add_help_message(StringRef long_opt, StringRef short_opt, StringRef category, StringRef desc,
                              StringRef default_value) {
            help_messages.emplace_back(long_opt, short_opt, category, desc, default_value);
        }

        void addOpts(StringRef long_opt, std::string *storage,
                     StringRef desc = "", StringRef category = "", const char *default_value = "",
                     StringRef short_opt = "") {
            add_help_message(long_opt, short_opt, category, desc, default_value);
            addOpts(StringOpt(long_opt, storage, desc, category, default_value, short_opt));
        }

        void addOpts(StringRef long_opt, int64_t *storage,
                     StringRef desc = "", StringRef category = "", int64_t default_value = 0,
                     StringRef short_opt = "") {
            add_help_message(long_opt, short_opt, category, desc,
                             internal::alloc_string(fmt::format("{}", default_value)));
            addOpts(Int64Opt(long_opt, storage, desc, category, default_value, short_opt));
        }

        void addOpts(StringRef long_opt, bool *storage,
                     StringRef desc = "", StringRef category = "", bool default_value = false,
                     StringRef short_opt = "") {
            add_help_message(long_opt, short_opt, category, desc,
                             internal::alloc_string(fmt::format("{}", default_value)));
            addOpts(BooleanOpt(long_opt, storage, desc, category, default_value, short_opt));
        }

        OptParser &addOpts(ParseCallback &&callback) {
            callbacks.push_back(callback);
            return *this;
        }

        _maybe_unused OptParser &addOpts(const ParseCallback &callback) {
            callbacks.push_back(callback);
            return *this;
        }

        void parseOpts(int &argc, const char *argv[]) {
            if (!set_parsed()) {
                return;
            }

            OptMap stackStore;
            buildMap(stackStore, argc, argv);
            parseOpts(stackStore);
        }

        void parseOpts(OptMap &opts) {
            if (!set_parsed()) {
                return;
            }
            _parseOpts(opts);
        }

        _no_discard bool parsed() const {
            return _parsed;
        }

        std::string print_helps() {
            sort_help_message();

            std::stringstream ss;

            StringRef last_category;
            for (const auto &help_row: help_messages) {
                auto long_opt = internal::get_long_opt(help_row);
                auto short_opt = internal::get_short_opt(help_row);
                auto category = internal::get_category(help_row);
                auto description = internal::get_desc(help_row);
                auto default_value = internal::get_default_value(help_row);
                if (last_category != category) {
                    ss << "\n" << category << "\n";
                    last_category = category;
                }
                if (get_length_of_string_ref(short_opt) > 0) {
                    ss << fmt::format("--{:<15}, -{:<3}\t= {:<8}",
                                      long_opt, short_opt, default_value);
                } else {
                    ss << fmt::format("--{:<20}\t= {:<8}",
                                      long_opt, default_value);
                }
                if (get_length_of_string_ref(default_value) > 7) {
                    ss << fmt::format("\n{:36}", "");
                }
                ss << internal::format_description(description) << "\n";
            }
            return ss.str();
        }

    private:
        bool set_parsed() {
            if (!_parsed) {
                mutex.lock();
                if (_parsed) {
                    mutex.unlock();
                    return false;
                }
                _parsed = true;
                mutex.unlock();
            }
            return true;
        }

        void sort_help_message() {
            std::sort(help_messages.begin(), help_messages.end(), [](auto l, auto r) {
                return internal::get_category(l) < internal::get_category(r);
            });
        }

        void _parseOpts(OptMap &opts) {
            for (auto &callback : callbacks) {
                callback(opts);
            }
        }
    };

    static OptParser _global;

    _maybe_unused void addOpts(StringRef long_opt, std::string *storage,
                               StringRef desc = "", StringRef category = "", const char *default_value = "",
                               StringRef short_opt = "") {
        _global.addOpts(long_opt, storage, desc, category, default_value, short_opt);
    }

    _maybe_unused void addOpts(StringRef long_opt, int64_t *storage,
                               StringRef desc = "", StringRef category = "", int64_t default_value = 0,
                               StringRef short_opt = "") {
        _global.addOpts(long_opt, storage, desc, category, default_value, short_opt);
    }

    _maybe_unused void addOpts(StringRef long_opt, bool *storage,
                               StringRef desc = "", StringRef category = "", bool default_value = false,
                               StringRef short_opt = "") {
        _global.addOpts(long_opt, storage, desc, category, default_value, short_opt);
    }

    _maybe_unused OptParser &addOpts(typename OptParser::ParseCallback &&callback) {
        return _global.addOpts(callback);
    }

    _maybe_unused _maybe_unused OptParser &addOpts(const typename OptParser::ParseCallback &callback) {
        return _global.addOpts(callback);
    }

    _maybe_unused void parseOpts(int &argc, const char *argv[]) {
        _global.parseOpts(argc, argv);
    }

    _maybe_unused std::string print_helps() {
        return _global.print_helps();
    }


    bool parsed() {
        return _global.parsed();
    }
}



