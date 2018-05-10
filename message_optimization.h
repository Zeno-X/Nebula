// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <type_traits>

namespace zendo
{
    
    namespace details
    {
        
#pragma pack(push, 1)
        template<typename Header, typename Body, bool HeaderEmpty, bool BodyEmpty> class message_optimization;
        
        template<typename Header, typename Body>
        class message_optimization<Header, Body, false, false>
        {
        public:
            using header_type = Header;
            using body_type = Body;
        private:
            header_type header_;
            body_type body_;
        public:
            static constexpr bool header_empty() { return false; }
            static constexpr bool body_empty() { return false; }
            header_type* header() noexcept { return &header_; }
            header_type const* header() const noexcept { return &header_; }
            body_type* body() noexcept { return &body_; }
            body_type const* body() const noexcept { return &body_; }
        };
        
        template<typename Header, typename Body>
        class message_optimization<Header, Body, false, true>
        {
        public:
            using header_type = Header;
            using body_type = Body;
        private:
            header_type header_;
        public:
            static constexpr bool header_empty() { return false; }
            static constexpr bool body_empty() { return true; }
            header_type* header() noexcept { return &header_; }
            header_type const* header() const noexcept { return &header_; }
            body_type* body() noexcept { return nullptr; };
            body_type const* body() const noexcept { return nullptr; };
        };
        
        template<typename Header, typename Body>
        class message_optimization<Header, Body, true, false>
        {
        public:
            using header_type = Header;
            using body_type = Body;
        private:
            body_type body_;
        public:
            static constexpr bool header_empty() { return true; }
            static constexpr bool body_empty() { return false; }
            header_type* header() noexcept { return nullptr; };
            header_type const* header() const noexcept { return nullptr; }
            body_type* body() noexcept { return &body_; }
            body_type const* body() const noexcept { return &body_; }
        };
        
        template<typename Header, typename Body>
        class message_optimization<Header, Body, true, true>
        {
        public:
            using header_type = Header;
            using body_type = Body;
            static constexpr bool header_empty() { return true; }
            static constexpr bool body_empty() { return true; }
            header_type* header() noexcept { return nullptr; };
            header_type const* header() const noexcept { return nullptr; }
            body_type* body() noexcept { return nullptr; }
            body_type const* body() const noexcept { return nullptr; }
        };
#pragma pack(pop)
        
    }
    
    template<typename Header, typename Body>
    using message_optimization = details::message_optimization<Header, Body, std::is_empty<Header>::value, std::is_empty<Body>::value>;
    
}

