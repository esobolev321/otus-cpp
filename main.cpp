#include <ip_v4.h>

#include <iostream>
#include <set>
#include <string>
#include <sstream>

int main(int, char **)
{
	std::multiset<ip_filter::IPAddressV4, std::greater<ip_filter::IPAddressV4>> ipSet;

	auto parseIPFromTSV = [](const std::string& line) -> std::string {
		auto tabPos = line.find('\t');
		return tabPos != std::string::npos ? line.substr(0, tabPos) : line;
	};

	std::string line;
	while (std::getline(std::cin, line)) {
		const auto ipStr = parseIPFromTSV(line);
		
		if (!ipStr.empty()) {
			try {
				ipSet.emplace(ipStr);
			} catch (...) {
				std::cerr << "Invalid IP: " << ipStr << std::endl;
			}
		}
	}
	
	for (const auto& addr : ipSet) {
		std::cout << addr << std::endl;
	}

	ip_filter::IPAddressV4 network1("1.0.0.0");
	ip_filter::IPAddressV4 mask1("255.0.0.0");

	for (const auto& addr : ipSet) {
		if(addr.isInSubnet(network1, mask1)) {
			std::cout << addr << std::endl;
		}
	}

	ip_filter::IPAddressV4 network2("46.70.0.0");
	ip_filter::IPAddressV4 mask2("255.255.0.0");

	for (const auto& addr : ipSet) {
		if (addr.isInSubnet(network2, mask2)) {
			std::cout << addr << std::endl;
		}
	}

	ip_filter::IPAddressV4 network3("46.0.0.0");
	ip_filter::IPAddressV4 mask3("255.0.0.0");

	ip_filter::IPAddressV4 network4("0.46.0.0");
	ip_filter::IPAddressV4 mask4("0.255.0.0");

	ip_filter::IPAddressV4 network5("0.0.46.0");
	ip_filter::IPAddressV4 mask5("0.0.255.0");

	ip_filter::IPAddressV4 network6("0.0.0.46");
	ip_filter::IPAddressV4 mask6("0.0.0.255");

	for (const auto& addr : ipSet) {
		if(addr.isInSubnet(network3, mask3)) {
			std::cout << addr << std::endl;
		}
		else if (addr.isInSubnet(network4, mask4)) {
			std::cout << addr << std::endl;
		}
		else if (addr.isInSubnet(network5, mask5)) {
			std::cout << addr << std::endl;
		}
		else if (addr.isInSubnet(network6, mask6)) {
			std::cout << addr << std::endl;
		}
	}


	return 0;
}
