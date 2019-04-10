#pragma once

#include <cassert>
#include <ostream>

#define ESC "\x1b"
namespace Log
{
    enum class Level
    {
        Critical,
        Error,
        Warning,
        Info
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
                case Level::Critical:
                    m_out<< ESC "[1;35m";
                    break;
                case Level::Error:
                    m_out<< ESC "[1;31m";
                    break;
                case Level::Warning:
                    m_out<< ESC "[1;33m";
                    break;
                case Level::Info:
                    m_out<< ESC "[1;37m";
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

    using Critical = ColorfulStream<Level::Critical>;
    using Error   = ColorfulStream<Level::Error>;
    using Warning = ColorfulStream<Level::Warning>;
    using Info = ColorfulStream<Level::Info>;
}
#undef ESC