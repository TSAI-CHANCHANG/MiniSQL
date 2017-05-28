#ifndef RUNTIMEEXCEPTION_H
#define RUNTIMEEXCEPTION_H

#include<exception>

class RunTimeException:std::exception{
public:
    RunTimeException(std::string s):text(s){}

    std::string what(){
        return text;
    }
private:
    std::string text;
};

#endif // RUNTIMEEXCEPTION_H
