#pragma once

#include <Common/Noncopyable.h>
#include <cassert>
#include <ostream>

#define ESC "\x1b"
namespace Log
{
    enum class Level
    {
        Error,
        Warning
    };

    template<Level level>
    class ColorfulStream
    {
        std::ostream& m_out;
    public:
        ColorfulStream(std::ostream &out):m_out(out)
        {
            switch(level)
            {
                case Level::Error:
                    m_out<< ESC "[1;31m";
                    break;
                case Level::Warning:
                    m_out<< ESC "[1;33m";
                    break;
                default:
                    assert(false);
            }
        }
        ~ColorfulStream()
        {
            m_out<< ESC "[0m";
        }

        template <typename T>
        ColorfulStream& operator<<(T value)
        {
            m_out << value;
            return *this;
        }
    };

    using Error = ColorfulStream<Level::Error>;
    using Warning = ColorfulStream<Level::Warning>;
}
#undef ESC