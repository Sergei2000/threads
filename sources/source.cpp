// Copyright 2018 Your Name <your_email>

#include <header.hpp>
#include <picosha2.h>

static std::vector<std::thread> potoki;
static bool status = false;

std::string generator() {
    std::string const default_chars =
    "abcdefghijklmnaoqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    std::string final_str = "";
    for (int i = 0; i < std::rand(); ++i) {
        final_str.push_back(default_chars[std::rand() % default_chars.size()]);
    }

    return final_str;
}

void sha(int potok_num) {
    std::string tmp;
    while (!status) {
        tmp = generator();
        if (picosha2::hash256_hex_string
        (std::forward<std::string>(tmp)).find("00", 60) == std::string::npos) {
            BOOST_LOG_TRIVIAL(trace) << picosha2::hash256_hex_string
            (std::forward<std::string>(tmp)) << "id:"
                                     << std::this_thread::get_id();
        } else {
            BOOST_LOG_TRIVIAL(info) <<
            picosha2::hash256_hex_string(std::forward<std::string>(tmp))
            << "id:"
            << std::this_thread::get_id();
            status = true;
        }
        std::cout << std::endl;
    }
}

void logg() {
    boost::log::register_simple_formatter_factory<boost::log
    ::trivial::severity_level, char>("Severity");
    boost::log::add_file_log(
            boost::log::keywords::file_name = "log.log",
            boost::log::keywords::format =
            "[%ThreadID%][%TimeStamp%][%Severity%]: %Message%");
    boost::log::add_console_log(
            std::cout,
            boost::log::keywords::format =
            "[%ThreadID%][%TimeStamp%][%Severity%]: %Message%");
    boost::log::add_common_attributes();
    for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
        potoki.push_back(std::thread(sha, i));
    }
    for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
        potoki[i].join();
    }
}

int main() {
    logg();
    return 0;
}
