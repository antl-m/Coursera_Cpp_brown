#include "ini.h"

namespace Ini {

    std::pair<std::string_view, std::string_view> Split(std::string_view line, char by) {
        size_t pos = line.find(by);
        std::string_view left = line.substr(0, pos);

        if (pos < line.size() && pos + 1 < line.size()) {
            return {left, line.substr(pos + 1)};
        } else {
            return {left, std::string_view()};
        }
    }

    std::string_view Unquote(std::string_view value) {
        if (!value.empty() && value.front() == '[') {
            value.remove_prefix(1);
        }
        if (!value.empty() && value.back() == ']') {
            value.remove_suffix(1);
        }
        return value;
    }

    Section &Document::AddSection(std::string name) {
        return sections[name];
    }

    const Section &Document::GetSection(const std::string &name) const {
        return sections.at(name);
    }

    size_t Document::SectionCount() const {
        return sections.size();
    }

    Section ParseSection(std::istream &input) {
        Section res;
        for (std::string line; std::getline(input, line) && !line.empty() && input.peek() != '[';) {
            auto[key, value] = Split(line, '=');
            res[std::string(key)] = std::string(value);
        }
        return res;
    }

    Document Load(std::istream &input) {
        Document res;
        Section *cur_sec = nullptr;
        for (std::string line; std::getline(input, line);) {
            if (line.empty())
                continue;
            if (line[0] == '['){
                cur_sec = &res.AddSection(std::string(Unquote(line)));
                continue;
            }
            auto[key, value] = Split(line, '=');
            (*cur_sec)[std::string(key)] = std::string(value);
        }
        return res;
    }
}
