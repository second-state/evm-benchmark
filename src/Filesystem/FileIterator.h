//#pragma once

#include<filesystem>
#include<functional>
#include<string>
#include<iostream>

namespace fs = std::filesystem;

class FileIterator
{
    using func = std::function<void(fs::path)>;

public:
    void visit(const std::string &base, func callback)
    {
        for(auto path: fs::recursive_directory_iterator(base))
        {
            if( path.is_regular_file() )
            {
                callback(path.path());
            }
        }
    }
};
