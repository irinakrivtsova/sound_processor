#ifndef PARSER_H
#define PARSER_H

#include <cstring>
#include <vector>

struct FilterDescriptor
{
    char* filterName = nullptr;
    std::vector<char*> params;
};

class ArgsParser
{
public:
    enum class Result
    {
        ok,
        noArgs,   // show help
        badArgs,  // incorrect arguments sequence
    };

    ArgsParser();

    Result parse(int argc, char* argv[]);
    const std::vector<FilterDescriptor>& getDescriptors() const;

public:
    const char* getInFileName() const { return _inFileName; }
    const char* getOutFileName() const { return _outFileName; }

private:
    char* _inFileName = nullptr;
    char* _outFileName = nullptr;
    std::vector<FilterDescriptor> _filterDescriptors;
};

#endif