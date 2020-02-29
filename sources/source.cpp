// Copyright 2018 Your Name <your_email>

#include <header.hpp>
static std::vector<std::thread> threads;
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

void print_trace(const std::string &arg) {
    BOOST_LOG_TRIVIAL(trace) << picosha2::hash256_hex_string(arg) << "id:"
                             << std::this_thread::get_id();
}

void print_info(const std::string &arg) {
    BOOST_LOG_TRIVIAL(info) << picosha2::hash256_hex_string(arg)
                            << "id:"
                            << std::this_thread::get_id();
}

void sha() {
    std::string tmp;
    while (!status) {
        tmp = generator();
        if (picosha2::hash256_hex_string
        (std::forward<std::string>(tmp)).find("00", 60) == std::string::npos) {
            print_trace(tmp);
        } else {
            print_info(tmp);
            status = true;
        }
        std::cout << std::endl;
    }
}

void start() {
    for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i) {
        threads.push_back(std::thread(sha));
    }
    for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i) {
        threads[i].join();
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
    start();
}

int main() {
    logg();
    return 0;
}
