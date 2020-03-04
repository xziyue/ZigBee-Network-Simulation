#include "picojson.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <regex>

#include "settings_from_pipe.h"

using namespace std;

using map_type = map<string, string>;

vector<char> sharedBuffer;

void *read_and_parse_stdin() {
    vector<char> buffer;

    char val;
    while(cin.peek() != decltype(cin)::traits_type::eof()){
        cin.read(&val, 1);
        buffer.push_back(val);
    }

    // terminate the string
    buffer.push_back('\0');

    picojson::value jsonVal;
    // parse json
    auto err = picojson::parse(jsonVal, buffer.data());
    if(!err.empty()){
        throw runtime_error(string{"error parsing JSON parameters: "} + err);
    }

    // check if the type of the value is "object"
    if (!jsonVal.is<picojson::object>()) {
        throw runtime_error{"JSON is not an object."};
    }

    auto ret = new map_type();
    size_t sharedBufferSize = 0;

    // iterate through objects and build map
    const picojson::value::object& obj = jsonVal.get<picojson::object>();
    for (auto i = obj.begin(); i != obj.end(); ++i) {
        if(!i->second.is<string>()){
            throw runtime_error{"a non-string value is found, only string type is allowed!"};
        }
        sharedBufferSize = max(sharedBufferSize, i->second.get<string>().size() + 1);
        ret->insert(make_pair(i->first, i->second.get<string>()));
    }

    // allocate shared buffer
    sharedBuffer.resize(sharedBufferSize);

    return reinterpret_cast<void*>(ret);
}

void free_parsed_stdin(void *res) {
    auto mapPtr = reinterpret_cast<map_type*>(res);
    delete mapPtr;
    sharedBuffer.resize(0);
}

bool check_has_item(void *res, const char *item) {
    auto &mapObj = *reinterpret_cast<map_type*>(res);
    if(mapObj.find(string{item}) != mapObj.end()){
        return true;
    }
    return false;
}

void load_item_into_buffer(void *res, const char *item) {
    auto &mapObj = *reinterpret_cast<map_type*>(res);
    auto iter = mapObj.find(string{item});
    if(iter == mapObj.end()){
        throw runtime_error{"accessing nonexistent key"};
    }
    // copy the data into shared buffer
    strcpy(sharedBuffer.data(), iter->second.c_str());
}

const char *get_item_buffer() {
    return sharedBuffer.data();
}

_eightbytes get_ieee_addr_from_buffer(){
    _eightbytes ret;

    auto addrRegex = regex("([a-fA-F0-9]{2}):([a-fA-F0-9]{2}):([a-fA-F0-9]{2}):([a-fA-F0-9]{2}):([a-fA-F0-9]{2}):([a-fA-F0-9]{2}):([a-fA-F0-9]{2}):([a-fA-F0-9]{2})");

    cmatch matches;
    regex_match(get_item_buffer(), matches, addrRegex);

    if(matches.size() != 9){
        throw runtime_error(string{"invalid IEEE address string: "} + string{get_item_buffer()});
    }

    for(auto i = 0; i < 8; ++i){
        ret.data[i] = stoul(string{matches[i + 1]}, nullptr, 16);
    }

    return ret;
}

unsigned short get_pan_id_from_buffer(){
    unsigned short ret;

    auto itemString = string{get_item_buffer()};
    if(itemString[0] != '0'){
        throw runtime_error{"invalid PAN id: first char must be \'0\'"};
    }
    if(!(itemString[1] == 'x' or itemString[1] == 'X')){
        throw runtime_error{"invalid PAN id: second char must be \'x\' or \'X\'"};
    }
    if(itemString.size() != 6){
        throw runtime_error{"invalid PAN id: length of PAN id string must be 6"};
    }

    auto panStr = itemString.substr(2);
    ret = stoul(panStr, nullptr, 16);

    return ret;
}