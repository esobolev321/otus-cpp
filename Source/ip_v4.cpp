#include <ip_v4.h>

#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace ip_filter {   

IPAddressV4::IPAddressV4(const std::string& str) {
    std::istringstream iss(str);
    std::string token;
    int i = 0;

    while (i < 4 && std::getline(iss, token, '.')) {
        const auto octet = std::stoi(token);
        if (octet < 0 || octet > 255) {
            throw std::invalid_argument("Invalid IP address");
        }
        octets[i++] = static_cast<uint8_t>(octet);
    }

    if (i != 4 || std::getline(iss, token, '.')) {
        throw std::invalid_argument("Invalid IP address");
    }
}

std::string IPAddressV4::toString() const {
    std::ostringstream oss;
    oss << static_cast<int>(octets[0]) << '.'
        << static_cast<int>(octets[1]) << '.'
        << static_cast<int>(octets[2]) << '.'
        << static_cast<int>(octets[3]);
    return oss.str();
}

bool IPAddressV4::operator<(const IPAddressV4& other) const {
    for (size_t i = 0; i < 4; ++i) {
        if (octets[i] < other.octets[i]) {
            return true;
        }
        else if (octets[i] > other.octets[i]) {
            return false;
        }
    }
    return false;
}

bool IPAddressV4::isInSubnet(const IPAddressV4& network, const IPAddressV4& mask) const {
    for (size_t i = 0; i < 4; ++i) {
        if ((octets[i] & mask.octets[i]) != (network.octets[i] & mask.octets[i])) {
            return false;
        }
    }
    return true;
}

std::ostream& operator<<(std::ostream& os, const IPAddressV4& ip) {
    os << ip.toString();
    return os;
}

std::istream& operator>>(std::istream& is, IPAddressV4& ip) {
    std::string str;
    is >> str;
    if (is) {
        try {
            ip = IPAddressV4(str);
        }
        catch (...) {
            is.setstate(std::ios::failbit);
        }
    }
    return is;
}

} // namespace ip_filter