#pragma once

#include <array>
#include <string>
#include <iostream>

namespace ip_filter {
    
class IPAddressV4 {
public:
    IPAddressV4() = default;

    IPAddressV4(const std::string& str);

    std::string toString() const;

    bool operator<(const IPAddressV4& other) const;
    
    bool operator>(const IPAddressV4& other) const;

    bool isInSubnet(const IPAddressV4& network, const IPAddressV4& mask) const;

private:
    std::array<uint8_t, 4> octets;
};

std::ostream& operator<<(std::ostream& os, const IPAddressV4& ip);

std::istream& operator>>(std::istream& is, IPAddressV4& ip);

} // namespace ip_filter
