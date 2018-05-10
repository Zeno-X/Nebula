// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.
#pragma once
#include <string>

namespace zendo
{
    
    class unique_package
    {
    private:
        char* buffer_;
        size_t read_nonius_, write_nonius_;
        
        template<typename...Args> struct writer;
        template<typename T, typename...Args> struct writer<T, Args...>
        {
            writer(unique_package* package, T&& value, Args&&...args)
            {
                new(package->buffer_ + package->write_nonius_)T(std::forward<T>(value));
                package->write_nonius_ += sizeof(T);
                writer<Args...> wtr(package, std::forward<Args>(args)...);
            }
        };

        template<typename T> struct writer<T>
        {
            writer(unique_package* package, T&& value)
            {
                new(package->buffer_ + package->write_nonius_)T(std::forward<T>(value));
                package->write_nonius_ += sizeof(T);
            }
        };
        
        template<typename...Args> struct reverse_writer;
        template<typename T, typename...Args> struct reverse_writer<T, Args...>
        {
            reverse_writer(unique_package* package, Args&&...args, T&& value)
            {
                new(package->buffer_ + package->write_nonius_)T(std::forward<T>(value));
                package->write_nonius_ += sizeof(T);
                reverse_writer<Args...> wtr(package, std::forward<Args>(args)...);
            }
        };
        
        template<typename T> struct reverse_writer<T>
        {
            reverse_writer(unique_package* package, T&& value)
            {
                new(package->buffer_ + package->write_nonius_)T(std::forward<T>(value));
                package->write_nonius_ += sizeof(T);
            }
        };
        
        template <typename...Args> struct reader;
        template<typename T, typename...Args> struct reader<T, Args...>
        {
            reader(unique_package* package, size_t& cnt, T*& value, Args*&...args)
            {
                if(package->readable())
                {
                    value = reinterpret_cast<T*>(package->buffer_ + package->read_nonius_);
                    package->read_nonius_ += sizeof(T);
                    ++cnt;
                    reader<Args...> rdr(package, cnt, args...);
                }
            }
        };
        
        template<typename T> struct reader<T>
        {
            reader(unique_package* package, size_t& cnt, T*& value)
            {
                if(package->readable())
                {
                    value = reinterpret_cast<T*>(package->buffer_ + package->read_nonius_);
                    package->read_nonius_ += sizeof(T);
                    ++cnt;
                }
            }
        };
        
        
        template <typename...Args> struct reverse_reader;
        template<typename T, typename...Args> struct reverse_reader<T, Args...>
        {
            reverse_reader(unique_package* package, size_t& cnt, Args*&...args, T*& value)
            {
                if(package->readable())
                {
                    value = reinterpret_cast<T*>(package->buffer_ + package->read_nonius_);
                    package->read_nonius_ += sizeof(T);
                    ++cnt;
                    reverse_reader<Args...> rdr(package, cnt, args...);
                }
            }
        };
        
        template<typename T> struct reverse_reader<T>
        {
            reverse_reader(unique_package* package, size_t& cnt, T*& value)
            {
                if(package->readable())
                {
                    value = reinterpret_cast<T*>(package->buffer_ + package->read_nonius_);
                    package->read_nonius_ += sizeof(T);
                    ++cnt;
                }
            }
        };
        
    public:
        unique_package(const unique_package&) = delete;
        unique_package& operator=(const unique_package&) = delete;
        unique_package(void* buffer)
        : buffer_(static_cast<char*>(buffer))
        , read_nonius_(0)
        , write_nonius_(0)
        {
        }
        
        unique_package(void* buffer, size_t size)
        : buffer_(static_cast<char*>(buffer))
        , read_nonius_(0)
        , write_nonius_(size)
        {
        }
        
        unique_package(unique_package&& rhs)
        : buffer_(rhs.buffer_)
        , read_nonius_(rhs.read_nonius_)
        , write_nonius_(rhs.write_nonius_)
        {
            rhs.buffer_ = nullptr;
            rhs.read_nonius_ = 0;
            rhs.write_nonius_ = 0;
        }
        
        ~unique_package() { }
        
        void reset(void* buffer, size_t size = 0)
        {
            buffer_ = static_cast<char*>(buffer);
            read_nonius_ = 0;
            write_nonius_ = size;
        }
        
        void* get_buffer() const { return buffer_; }
        size_t read_nonius() const { return read_nonius_; }
        size_t write_nonius() const { return write_nonius_; }
        bool readable() const { return read_nonius_ < write_nonius_; }
        template<typename T> T* read()
        {
            if(read_nonius_ < write_nonius_)
            {
                T* ret = reinterpret_cast<T*>(buffer_ + read_nonius_);
                read_nonius_ += sizeof(T);
                return ret;
            }
            return nullptr;
        }
        
        template<typename...Args> size_t read(Args*&...args)
        {
            size_t cnt = 0;
            reader<Args...> rdr(this, cnt, args...);
            return cnt;
        }
        
        template<typename...Args> size_t reverse_read(Args*&...args)
        {
            size_t cnt = 0;
            reverse_reader<Args...> rdr(this, cnt, args...);
            return cnt;
        }
        
        template<typename...Args> void write(Args&&...args)
        {
            writer<Args...> wtr(this, std::forward<Args>(args)...);
        }
        
        template<typename...Args> void reverse_write(Args&&...args)
        {
            reverse_writer<Args...> reverse_wtr(this, std::forward<Args>(args)...);
        }
        
    };
    
}
