//
// Created by kamiyoru on 2020/6/19.
//

#include <pascal-s/token.h>

#include <map>
#include <string>

using keyword_mapping = std::map<std::string, KeywordType>;
using reverse_keyword_mapping = std::map<KeywordType, std::string>;
using marker_mapping = std::map<std::string, MarkerType>;
using reverse_marker_mapping = std::map<MarkerType, std::string>;

keyword_mapping key_map = {
        keyword_mapping::value_type{"to", KeywordType::To},
        keyword_mapping::value_type{"do", KeywordType::Do},
        keyword_mapping::value_type{"of", KeywordType::Of},
        keyword_mapping::value_type{"for", KeywordType::For},
        keyword_mapping::value_type{"else", KeywordType::Else},
        keyword_mapping::value_type{"if", KeywordType::If},
        keyword_mapping::value_type{"then", KeywordType::Then},
        keyword_mapping::value_type{"char", KeywordType::Char},
        keyword_mapping::value_type{"boolean", KeywordType::Boolean},
        keyword_mapping::value_type{"real", KeywordType::Real},
        keyword_mapping::value_type{"integer", KeywordType::Integer},
        keyword_mapping::value_type{"array", KeywordType::Array},
        keyword_mapping::value_type{"end", KeywordType::End},
        keyword_mapping::value_type{"begin", KeywordType::Begin},
        keyword_mapping::value_type{"function", KeywordType::Function},
        keyword_mapping::value_type{"procedure", KeywordType::Procedure},
        keyword_mapping::value_type{"program", KeywordType::Program},
        keyword_mapping::value_type{"var", KeywordType::Var},
        keyword_mapping::value_type{"const", KeywordType::Const},
//        keyword_mapping::value_type{"div", KeywordType::Div},
//        keyword_mapping::value_type{"mod", KeywordType::Mod},
//        keyword_mapping::value_type{"and", KeywordType::And},
//        keyword_mapping::value_type{"or", KeywordType::Or},
//        keyword_mapping::value_type{"not", KeywordType::Not},
        keyword_mapping::value_type{"read", KeywordType::Read},
        keyword_mapping::value_type{"write", KeywordType::Write}
};

marker_mapping marker_map = {
        marker_mapping::value_type{"<>", MarkerType::NEQ},
        marker_mapping::value_type{"<=", MarkerType::LE},
        marker_mapping::value_type{">=", MarkerType::GE},
        marker_mapping::value_type{"<", MarkerType::LT},
        marker_mapping::value_type{"=", MarkerType::EQ},
        marker_mapping::value_type{">", MarkerType::GT},
        marker_mapping::value_type{"..", MarkerType::Range},

        marker_mapping::value_type{":=", MarkerType::Assign},
        marker_mapping::value_type{"+", MarkerType::Add},
        marker_mapping::value_type{"-", MarkerType::Sub},
        marker_mapping::value_type{"*", MarkerType::Mul},
        marker_mapping::value_type{"/", MarkerType::Div},
        marker_mapping::value_type{"div", MarkerType::Div},
        marker_mapping::value_type{"mod", MarkerType::Mod},

        marker_mapping::value_type{"(", MarkerType::LParen},
        marker_mapping::value_type{")", MarkerType::RParen},
        marker_mapping::value_type{"[", MarkerType::LBracket},
        marker_mapping::value_type{"]", MarkerType::RBracket},

        marker_mapping::value_type{",", MarkerType::Comma},
        marker_mapping::value_type{".", MarkerType::Dot},
        marker_mapping::value_type{";", MarkerType::Semicolon},
        marker_mapping::value_type{":", MarkerType::Colon},


        marker_mapping::value_type{"and", MarkerType::LogicAnd},
        marker_mapping::value_type{"or", MarkerType::LogicOr},
        marker_mapping::value_type{"not", MarkerType::LogicNot},
};

KeywordType get_keyword_type(const std::string &kt) {
    return key_map.at(kt);
}

MarkerType get_marker_type(const std::string &mt) {
    return marker_map.at(mt);
}

reverse_keyword_mapping reverse_key_map;

const std::string &get_keyword_type_reversed(KeywordType kt) {
    if (reverse_key_map.empty()) {
        for (auto &kv: key_map) {
            reverse_key_map[kv.second] = kv.first;
        }
    }
    return reverse_key_map.at(kt);
}

pascal_s::marker_type_underlying_type get_marker_pri(MarkerType marker_type) {
    return static_cast<pascal_s::marker_type_underlying_type>(marker_type)
            >> 0x4U;
}

reverse_marker_mapping reverse_marker_map;

const std::string &get_marker_type_reversed(MarkerType mt) {
    if (reverse_marker_map.empty()) {
        for (auto &mv : marker_map) {
            reverse_marker_map[mv.second] = mv.first;
        }
    }
    return reverse_marker_map.at(mt);
}



