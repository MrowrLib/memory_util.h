#pragma once

#include <string_format.h>

#include <cstdint>
#include <string>
#include <vector>

namespace MemoryUtil {

    //! Represents a collection of bytes (uint8_t)
    class Bytes {
        std::vector<uint8_t> _bytes;

    public:
        Bytes() = default;
        Bytes(std::vector<uint8_t> bytes) : _bytes(bytes) {}
        Bytes(const Bytes& other) : _bytes(other._bytes) {}

        std::vector<uint8_t> GetBytes() { return _bytes; }
        size_t               GetByteCount() { return _bytes.size(); }
        size_t               size() { return GetByteCount(); }

        Bytes& AddByte(uint8_t byte) {
            _bytes.push_back(byte);
            return *this;
        }
        Bytes& AddBytes(std::vector<uint8_t> bytes) {
            for (auto byte : bytes) _bytes.push_back(byte);
            return *this;
        }

        Bytes& Add8(uint8_t byte) {
            AddByte(byte);
            return *this;
        }
        Bytes& Add16(uint16_t word) {
            _bytes.push_back((word >> 0) & 0xFF);
            _bytes.push_back((word >> 8) & 0xFF);
            return *this;
        }
        Bytes& Add32(uint32_t dword) {
            _bytes.push_back((dword >> 0) & 0xFF);
            _bytes.push_back((dword >> 8) & 0xFF);
            _bytes.push_back((dword >> 16) & 0xFF);
            _bytes.push_back((dword >> 24) & 0xFF);
            return *this;
        }
        Bytes& Add64(uint64_t qword) {
            _bytes.push_back((qword >> 0) & 0xFF);
            _bytes.push_back((qword >> 8) & 0xFF);
            _bytes.push_back((qword >> 16) & 0xFF);
            _bytes.push_back((qword >> 24) & 0xFF);
            _bytes.push_back((qword >> 32) & 0xFF);
            _bytes.push_back((qword >> 40) & 0xFF);
            _bytes.push_back((qword >> 48) & 0xFF);
            _bytes.push_back((qword >> 56) & 0xFF);
            return *this;
        }

        Bytes& AddWord(uint16_t word) {
            Add16(word);
            return *this;
        }
        Bytes& AddDword(uint32_t dword) {
            Add32(dword);
            return *this;
        }
        Bytes& AddQword(uint64_t qword) {
            Add64(qword);
            return *this;
        }

        Bytes& AddAddress(uintptr_t address) {
            if (sizeof(uintptr_t) == 4) Add32(address);
            else if (sizeof(uintptr_t) == 8) Add64(address);
            else
                throw std::runtime_error(
                    string_format("Bytes: Unsupported address size {}", sizeof(uintptr_t))
                );
            return *this;
        }

        Bytes& Add(uint8_t byte) { return AddByte(byte); }
        Bytes& Add(std::vector<uint8_t> bytes) { return AddBytes(bytes); }
        Bytes& Add(uint16_t word) { return AddWord(word); }
        Bytes& Add(uint32_t dword) { return AddDword(dword); }
        Bytes& Add(uint64_t qword) { return AddQword(qword); }

        std::string ToHexString() const {
            std::string str;
            for (size_t i = 0; i < _bytes.size(); i++)
                if (i == _bytes.size() - 1) str += string_format("{:x}", _bytes[i]);
                else str += string_format("{:x} ", _bytes[i]);
            return str;
        }

        std::string ToEncodedString() const {
            std::string str;
            for (auto byte : _bytes) str += string_format("\\x{:x}", byte);
            return str;
        }

        static Bytes FromHexString(const std::string& hexString) {
            std::vector<uint8_t> bytes;
            std::string          hex;
            for (auto c : hexString) {
                if (c == ' ') {
                    if (hex.empty()) continue;
                    bytes.push_back(static_cast<uint8_t>(std::stoi(hex, nullptr, 16)));
                    hex.clear();
                } else {
                    hex += c;
                }
            }
            if (!hex.empty()) bytes.push_back(static_cast<uint8_t>(std::stoi(hex, nullptr, 16)));
            return Bytes{bytes};
        }

        // TODO support escaped strings (e.g. \x00)
        static Bytes FromString(const std::string& str) { return FromHexString(str); }

        operator std::vector<uint8_t>() const { return _bytes; }
    };

    std::string ToEncodedString(const Bytes& bytes) { return bytes.ToEncodedString(); }
    std::string ToHexString(const Bytes& bytes) { return bytes.ToHexString(); }

    // TODO support escaped strings (e.g. \x00)
    Bytes GetBytes(const std::string& hexString) { return Bytes::FromHexString(hexString); }
}
