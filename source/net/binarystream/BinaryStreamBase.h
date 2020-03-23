#ifndef BINARYSTREAMS_BINARYSTREAM_BASE_H
#define BINARYSTREAMS_BINARYSTREAM_BASE_H
#include "BinarySerializer.h"
#include "BinaryDeserializer.h"
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <stdint.h>
#include <type_traits>
#include <iterator>
#include "traits.h"

#define SIZEBYTES_LIMIT 4
#define UINT24_MAX ((1 << 24) - 1)

template <typename basestream>
class binarystream_base: public basestream {
public:
    typedef binarystream_base streamtype;
    using basestream::basestream;
    using basestream::good;
    using basestream::read;
    using basestream::write;
    using basestream::setstate;
    using basestream::badbit;

    binarystream_base() = default;

    bool load(const std::string & s) {
        uint32_t size = s.length();
        write(s.data(), size);
        return good();
    }

    bool save(std::string & s) {
        s = this->str();
        return good();
    }

    bool operator>>(bool &t) {
        uint8_t val;
        read((char *) &val, 1);
        t = val != 0;
        return good();
    }

    bool operator >>(uint8_t & t) {
        read((char *) &t, 1);
        return good();
    }

    bool operator >>(uint16_t & t) {
        read((char *) &t, 2);
        return good();
    }

    bool operator >>(uint32_t & t) {
        read((char *) &t, 4);
        return good();
    }

    bool operator >>(uint64_t & t) {
        read((char *) &t, 8);
        return good();
    }

    bool operator >>(int8_t & t) {
        read((char *) &t, 1);
        return good();
    }

    bool operator >>(int16_t & t) {
        read((char *) &t, 2);
        return good();
    }

    bool operator >>(int32_t & t) {
        read((char *) &t, 4);
        return good();
    }

    bool operator >>(int64_t & t) {
        read((char *) &t, 8);
        return good();
    }

    bool operator >>(float & t) {
        read((char *) &t, 4);
        return good();
    }

    bool operator >>(double & t) {
        read((char *) &t, 8);
        return good();
    }

    bool operator >>(std::string & s) {
        uint32_t size = readSize();
        s.reserve(size);

        char readBuf[size + 1] = { 0 };
        read((char *) readBuf, size);
        s.assign(readBuf, size);
        return good();
    }

    bool operator >>(std::u16string & s) {
        uint32_t length = readSize();
        if (!good()) {
            return false;
        }
        s.clear();

        char16_t readBuf[length + 1] = { 0 };
        read((char*) readBuf, length * sizeof(char16_t));
        s.assign(readBuf, length);
        return good();
    }

    bool operator >>(std::u32string & s) {
        uint32_t length = readSize();
        if (!good()) {
            return false;
        }
        s.clear();

        char32_t readBuf[length + 1] = { 0 };
        read((char*) readBuf, length * sizeof(char32_t));
        s.assign(readBuf, length);
        return good();
    }

    bool operator >>(char16_t & t) {
        read((char *) &t, sizeof(char16_t));
        return good();
    }

    bool operator >>(char32_t & t) {
        read((char *) &t, sizeof(char32_t));
        return good();
    }

    bool operator <<(const bool & t) {
        uint8_t val = t ? 1 : 0;
        write((const char *) &val, 1);
        return good();
    }

    bool operator <<(const uint8_t & t) {
        write((const char *) &t, 1);
        return good();
    }

    bool operator <<(const uint16_t & t) {
        write((const char *) &t, 2);
        return good();
    }

    bool operator <<(const uint32_t & t) {
        write((const char *) &t, 4);
        return good();
    }

    bool operator <<(const uint64_t & t) {
        write((const char *) &t, 8);
        return good();
    }

    bool operator <<(const int8_t & t) {
        write((const char *) &t, 1);
        return good();
    }

    bool operator <<(const int16_t & t) {
        write((const char *) &t, 2);
        return good();
    }

    bool operator <<(const int32_t & t) {
        write((const char *) &t, 4);
        return good();
    }

    bool operator <<(const int64_t & t) {
        write((const char *) &t, 8);
        return good();
    }

    bool operator <<(const float & t) {
        write((const char *) &t, 4);
        return good();
    }

    bool operator <<(const double & t) {
        write((const char *) &t, 8);
        return good();
    }

    bool operator <<(std::string & s) {
        uint32_t size = s.length();
        writeSize(size);

        write(s.c_str(), size);
        return good();
    }

    bool operator <<(std::u16string & s) {
        uint32_t size = s.length();
        writeSize(size);

        for (size_t i = 0; i < size && good() && *this << s[i]; i++) {
        }
        return good();
    }

    bool operator <<(std::u32string & s) {
        uint32_t size = s.length();
        writeSize(size);

        for (size_t i = 0; i < size && good() && *this << s[i]; i++) {
        }

        return good();
    }

    bool operator <<(const char16_t & t) {
        write((const char *) &t, sizeof(char16_t));
        return good();
    }

    bool operator <<(const char32_t & t) {
        write((const char *) &t, sizeof(char32_t));
        return good();
    }


    template<typename T, typename std::enable_if<is_serializable_container<T>::value, int>::type = 0>
    bool s_read(T& l, const uint32_t size_min, const uint32_t size_max) {
        l.clear();
        uint32_t size = readSize();
        if (size < size_min || size > size_max) {
            return false;
        }
        for (uint32_t i = 0; i < size; ++i) {
            typename T::value_type t;
            *this >> t;
            add(l, t);
        }
        return true;
    }

    template<typename T, typename std::enable_if<is_string<T>::value, int>::type = 0>
    bool s_read(T& l, const uint32_t size_min, const uint32_t size_max) {
        l.clear();
        uint32_t size = readSize();
        if (size < size_min || size > size_max) {
            return false;
        }
        auto dataSize = size * sizeof(typename T::value_type);
        typename T::value_type readBuf[dataSize];
        read((char *) &readBuf[0], dataSize);
        if (good()) {
            l.assign(readBuf, size);
            return true;
        } else {
            return false;
        }
    }

    template<typename T, typename std::enable_if<is_serializable_container<T>::value, int>::type = 0>
    bool s_read(T& l, const typename T::value_type & min, const typename T::value_type & max, const uint32_t size_min, const uint32_t size_max) {
        l.clear();
        uint32_t size = readSize();
        if (size < size_min || size > size_max) {
            return false;
        }
        for (uint32_t i = 0; i < size; ++i) {
            typename T::value_type t;
            *this >> t;
            if (t < min || t > max) {
                return false;
            }
            add(l, t);
        }
        return true;
    }

    template<typename T, typename std::enable_if<is_serializable_container<T>::value, int>::type = 0>
    bool s_read_min(T& l, const typename T::value_type & min, const uint32_t size_min, const uint32_t size_max) {
        l.clear();
        uint32_t size = readSize();
        if (size < size_min || size > size_max) {
            return false;
        }
        for (uint32_t i = 0; i < size; ++i) {
            typename T::value_type t;
            *this >> t;
            if (t < min) {
                return false;
            }
            add(l, t);
        }
        return true;
    }

    template<typename T, typename std::enable_if<is_serializable_container<T>::value, int>::type = 0>
    bool s_read_min(T& l, const typename T::value_type & min) {
        l.clear();
        uint32_t size = readSize();
        for (uint32_t i = 0; i < size; ++i) {
            typename T::value_type t;
            *this >> t;
            if (t < min) {
                return false;
            }
            add(l, t);
        }
        return true;
    }

    template<typename T, typename std::enable_if<is_serializable_container<T>::value, int>::type = 0>
    bool s_read_max(T& l, const typename T::value_type & max, const uint32_t size_min, const uint32_t size_max) {
        l.clear();
        uint32_t size = readSize();
        if (size < size_min || size > size_max) {
            return false;
        }
        for (uint32_t i = 0; i < size; ++i) {
            typename T::value_type t;
            *this >> t;
            if (t > max) {
                return false;
            }
            add(l, t);
        }
        return true;
    }

    template<typename T, typename std::enable_if<is_serializable_container<T>::value, int>::type = 0>
    bool s_read_max(T& l, const typename T::value_type & max) {
        l.clear();
        uint32_t size = readSize();
        for (uint32_t i = 0; i < size; ++i) {
            typename T::value_type t;
            *this >> t;
            if (t > max) {
                return false;
            }
            add(l, t);
        }
        return true;
    }

    template<typename T, typename std::enable_if<is_associative_container<T>::value, int>::type = 0>
    bool operator <<(T & s) {
        writeSize(s.size());
        for (auto & t : s) {
            if (!((*this << t.first) && (*this << t.second))) {
                return false;
            }
        }
        return good();
    }

    template<typename T, typename std::enable_if<is_associative_container<T> { }, int>::type = 0>
    bool operator >>(T & m) {
        m.clear();
        uint32_t size = readSize();

        for (uint32_t i = 0; i < size; ++i) {
            if (!good()){
                return false;
            }
            typename T::key_type k;
            typename T::mapped_type v;
            if (*this >> k && *this >> v) {
                m.insert( { k, v });
            } else {
                return false;
            }
        }
        return true;
    }

    template<typename T, typename std::enable_if<is_serializable_container<T>::value && !is_associative_container<T>{}, int>::type = 0>
    bool operator <<(T & s) {
        writeSize(s.size());
        for (auto & t : s) {
            *this << t;
            if (!good()) {
                return false;
            }
        }
        return good();
    }

    template<typename T, typename std::enable_if<is_serializable_container<T>{} && !is_associative_container<T>{} && !is_set_container<T>{}, int>::type = 0>
    bool operator >>(T & l) {
        l.clear();
        uint32_t size = readSize();

        for (uint32_t i = 0; i < size; ++i) {
            if (!good()) return false;
            typename T::value_type t;
            *this >> t;
            if (!good()) {
                return false;
            }
            l.push_back(t);
        }
        return true;
    }

    template<typename T, typename std::enable_if<is_set_container<T>::value, int>::type = 0>
    bool operator >>(T & l) {
        l.clear();
        uint32_t size = readSize();

        for (uint32_t i = 0; i < size; ++i) {
            if (!good()) return false;
            typename T::value_type t;
            *this >> t;
            if (!good()) {
                return false;
            }
            l.insert(t);
        }
        return true;
    }

    template<typename F, typename S>
    bool operator << (std::pair<F, S> & p){
        return *this << p.first && *this << p.second;
    }

    template<typename F, typename S>
    bool operator >> (std::pair<F, S> & p){
        return *this >> p.first && *this >> p.second;
    }

    template<typename C, typename std::enable_if< (std::is_class<C> {} || std::is_enum<C> {}) && !is_serializable_container<C> {} && !is_associative_container<C>{}, int>::type = 0>
     bool operator >>(C & c) {
        BinaryDeserializer<streamtype> b(*this);
        if (!serialize(b, c)) {
            setstate(badbit);
        }
        return this->good();
    }

    template<typename C, typename std::enable_if<(std::is_class<C> {} || std::is_enum<C> {}) && !is_serializable_container<C> {} && !is_associative_container<C>{}, int>::type = 0>
    bool operator <<(typename std::remove_const<C>::type & c) {
        BinarySerializer<streamtype> b(*this);
        return serialize(b, c);
    }

    template<typename C, typename std::enable_if<(std::is_class<C> {} || std::is_enum<C> {}) && !is_serializable_container<C> {} && !is_associative_container<C> {}, int>::type = 0>
    bool operator <<(C & c) {
        BinarySerializer<streamtype> b(*this);
        return serialize(b, c);
    }


public:
    uint32_t test_readSize() {
        return readSize();
    }
private:
    //writes header for string, lists etc...
    void writeSize(uint32_t size) {
        uint8_t sizeBytes = 0;
        if (size > UINT8_MAX - 4) {
            sizeBytes = 1;
        }
        if (size > UINT8_MAX) {
            sizeBytes = 2;
        }
        if (size > UINT16_MAX) {
            sizeBytes = 3;
        }
        if (size > UINT24_MAX) {
            sizeBytes = 4;
        }
        if (sizeBytes > 0) {
            uint8_t sizeByte = sizeBytes + UINT8_MAX - 4;
            write((const char *) &sizeByte, 1);
            write((const char *) &size, sizeBytes);
        } else {
            write((const char *) &size, 1);
        }
    }

    //TODO return error state
    uint32_t readSize() {
        uint32_t size = 0;
        uint8_t sizeBytes = 0;
        read((char *) &size, 1);    //TODO add good() check
        if (size > UINT8_MAX - 4) {
            sizeBytes = size - (UINT8_MAX - 4);
            read((char *) &size, sizeBytes);//TODO add good() check
        }
        return size;
    }

private:
    template<typename Container, typename Value, typename std::enable_if<is_copy_insertable_container<Container>::value, int>::type = 0>
    void add(Container & c, Value & v){
        c.push_back(v);
    }

    template<typename Container, typename Value, typename std::enable_if<is_set_container<Container>::value, int>::type = 0>
    void add(Container & c, Value & v){
        c.insert(v);
    }
};

template<typename C, typename X, typename std::enable_if<std::is_class<C> { } && !is_serializable_container<C> { }, int>::type = 0>
bool operator <<(binarystream_base<X> & bs, C && c) {
    return bs << c;
}

//Magic!
template<typename Serializer, typename C, typename std::enable_if<std::is_class<C> {} && !is_serializable_container<C> {} && !is_associative_container<C> {}, int>::type = 0>
bool serialize(Serializer & serializer, C & c) {
    return c.serialize(serializer);
}
#endif
