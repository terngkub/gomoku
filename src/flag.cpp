#include "Flag.hpp"
#include <iostream>
#include <memory>
#include <regex>

/* ------------------------------------------- Constructors - Destructors ------------------------------------------- */
Flag::Flag() : _bin(""), _options(std::vector<Opt>()) {};

Flag::Flag(Flag const &rhs) : _bin("") { *this = rhs; }

Flag::~Flag() = default;

/* ------------------------------------------------ Static functions ------------------------------------------------ */
void
Flag::displayHelp() const {

    std::cout << "Usage of " << _bin << ":" << std::endl;
    for (auto const &option : _options) {
        std::cout << "  -" << option._handler << std::endl;
        std::cout << "\t" << option._help_message << " (default \"";
        switch (option._type) {
        case INT:
            std::cout << *std::static_pointer_cast<int>(option._default_value);
            break;
        case STRING:
            std::cout << *std::static_pointer_cast<std::string>(option._default_value);
            break;
        }
        std::cout << "\")" << std::endl;
    }

    exit(2);
}

/* ----------------------------------------------- Operator overloads ----------------------------------------------- */
Flag &
Flag::operator=(Flag const &rhs) {

    _bin = rhs._bin;
    for (auto const &option : rhs._options) {
        _options.push_back(Opt{
            ._default_value = option._default_value,
            ._handler = option._handler,
            ._help_message = option._help_message,
            ._type = option._type,
            ._value = option._value,
        });
    }

    return *this;
}

/* ------------------------------------------------ Member functions ------------------------------------------------ */
std::shared_ptr<int>
Flag::optInt(std::string const handler, int const default_value, std::string const help_message) {

    auto n = std::make_shared<int>(default_value);
    _options.push_back(Opt{
        ._default_value = std::make_shared<int>(default_value),
        ._handler = handler,
        ._help_message = help_message,
        ._type = INT,
        ._value = n
    });

    return n;
}

std::shared_ptr<std::string>
Flag::optString(std::string const handler, std::string const default_value, std::string const help_message) {

    auto s = std::make_shared<std::string>(default_value);
    _options.push_back(Opt{
        ._default_value = std::make_shared<std::string>(default_value),
        ._handler = handler,
        ._help_message = help_message,
        ._type = STRING,
        ._value = s
    });

    return s;
}

void
Flag::parse(int &argc, char const **&argv) {

    _bin = argv[0];
    int index = 1;
    MAIN_LOOP: for (; index < argc && argv[index][0] == '-'; ++index) {

        // Get rid of hyphens and ill-formed arguments
        int k = 0;
        for (; argv[index][k] == '-'; ++k);
        if (k > 2) {
            std::cout << "bad flag syntax: " << argv[index] << std::endl;
            displayHelp();
        }

        auto s = std::string(&argv[index][k]);
        auto arg_handler = s.substr(0, s.find('='));
        auto arg_value = s.substr(s.find('=') + 1);

        for (auto const &option : _options) {
            if (option._handler == arg_handler) {
                switch (option._type) {
                case INT:
                    if (!std::regex_match(arg_value, std::regex("^-?\\d+$"))) {
                        std::cout << "invalid value \"" << arg_value << "\" for flag -" << arg_handler
                                  << ": parse error" << std::endl;
                        displayHelp();
                    }

                    try {
                        *std::static_pointer_cast<int>(option._value) = std::stoi(arg_value);
                    } catch (std::out_of_range const &) {
                        std::cout << "invalid value \"" << arg_value << "\" for flag -" << arg_handler
                            << ": out of range" << std::endl;
                        displayHelp();
                    } catch (std::invalid_argument const &) {
                        std::cout << "invalid value \"" << arg_value << "\" for flag -" << arg_handler
                                  << ": parse error" << std::endl;
                        displayHelp();
                    }

                    break;

                case STRING:
                    *std::static_pointer_cast<std::string>(option._value) = arg_value;
                    break;
                }

                index += 1;
                goto MAIN_LOOP;
            }
        }

        // If this is reached, flag provided is invalid
        std::cout << "flag provided but not defined: -" << arg_handler << std::endl;
        displayHelp();
    }

    if (index > 1)
        argv = &argv[index];
}
