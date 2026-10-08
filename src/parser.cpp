#include "parser.h"

ArgsParser::ArgsParser() = default;

const std::vector<FilterDescriptor>& ArgsParser::getDescriptors() const
{
    return _filterDescriptors;
}

ArgsParser::Result ArgsParser::parse(int argc, char* argv[])
{
    _inFileName = nullptr;
    _outFileName = nullptr;
    _filterDescriptors.clear();

    if(argc == 1)
        return Result::noArgs;

    bool first = true;
    FilterDescriptor filter;

    for(int i = 1; i < argc; i++)
    {
        char* token = argv[i];
        if(strcmp(token, "-i") == 0)
        {
            if((i + 1) >= argc || _inFileName != nullptr)
                return Result::badArgs;
            _inFileName = argv[++i];
        }
        else if(strcmp(token, "-o") == 0)
        {
            if((i + 1) >= argc || _outFileName != nullptr)
                return Result::badArgs;
            _outFileName = argv[++i];
        }
        else if(strcmp(token, "-f") == 0)
        {
            if((i + 1) >= argc)
                return Result::badArgs;

            if(!first)
            {
                _filterDescriptors.push_back(
                    filter); 
            }
            
            first = false;
            filter = FilterDescriptor{};
            filter.filterName = argv[++i];
        }
        else
        {
            if (first)
            {
                return Result::badArgs;
            }
            filter.params.push_back(argv[i]);
        }
    }
    if(!first)
        _filterDescriptors.push_back(filter);
    return Result::ok;
}
