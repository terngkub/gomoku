#pragma once
#include <memory>
# include <string>
# include <vector>

class Flag {
private:
    enum type {
        INT,
        STRING
    };

    struct Opt {
        std::shared_ptr<void>           _default_value;
        std::string const               _handler;
        std::string const               _help_message;
        enum type                       _type;
        std::shared_ptr<void>           _value;
    };

    /* Attributes */
    char const *                        _bin;
    std::vector<Opt>                    _options;

public:
    /* Constructors - Destructors */
                                        Flag();
                                        Flag(Flag const &);
    virtual                             ~Flag();

    /* Operator overloads */
    Flag &                              operator=(Flag const &);

    /* Member functions */
    void                                displayHelp() const;
    std::shared_ptr<int>                optInt(std::string, int, std::string);
    std::shared_ptr<std::string>        optString(std::string, std::string, std::string);
    void                                parse(int &, char const **&);
};
