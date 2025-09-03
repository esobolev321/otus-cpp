#include <gtest/gtest.h>
#include <ip_v4.h>
#include <sstream>
#include <set>

class IPAddressV4Test : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};


TEST_F(IPAddressV4Test, DefaultConstructor) {
    ip_filter::IPAddressV4 ip;
    EXPECT_NO_THROW(ip.toString());
}

TEST_F(IPAddressV4Test, ValidIPAddresses) {
    EXPECT_NO_THROW(ip_filter::IPAddressV4("0.0.0.0"));
    EXPECT_NO_THROW(ip_filter::IPAddressV4("255.255.255.255"));
    EXPECT_NO_THROW(ip_filter::IPAddressV4("192.168.1.1"));
    EXPECT_NO_THROW(ip_filter::IPAddressV4("10.0.0.1"));
    EXPECT_NO_THROW(ip_filter::IPAddressV4("172.16.254.1"));
}

TEST_F(IPAddressV4Test, InvalidIPAddresses) {
    EXPECT_THROW(ip_filter::IPAddressV4(""), std::invalid_argument);
    EXPECT_THROW(ip_filter::IPAddressV4("256.1.1.1"), std::invalid_argument);
    EXPECT_THROW(ip_filter::IPAddressV4("1.1.1"), std::invalid_argument);
    EXPECT_THROW(ip_filter::IPAddressV4("1.1.1.1.1"), std::invalid_argument);
    EXPECT_THROW(ip_filter::IPAddressV4("a.b.c.d"), std::invalid_argument);
    EXPECT_THROW(ip_filter::IPAddressV4("192.168.1.-1"), std::invalid_argument);
    EXPECT_THROW(ip_filter::IPAddressV4("192.168..1"), std::invalid_argument);
    EXPECT_THROW(ip_filter::IPAddressV4("192 168 1 1"), std::invalid_argument);
}

TEST_F(IPAddressV4Test, ToStringMethod) {
    ip_filter::IPAddressV4 ip1("192.168.1.1");
    EXPECT_EQ(ip1.toString(), "192.168.1.1");
    
    ip_filter::IPAddressV4 ip2("0.0.0.0");
    EXPECT_EQ(ip2.toString(), "0.0.0.0");
    
    ip_filter::IPAddressV4 ip3("255.255.255.255");
    EXPECT_EQ(ip3.toString(), "255.255.255.255");
}

TEST_F(IPAddressV4Test, ComparisonOperator) {
    ip_filter::IPAddressV4 ip1("192.168.1.1");
    ip_filter::IPAddressV4 ip2("192.168.1.2");
    ip_filter::IPAddressV4 ip3("192.168.2.1");
    ip_filter::IPAddressV4 ip4("193.168.1.1");
    ip_filter::IPAddressV4 ip5("192.168.1.1");
    
    EXPECT_TRUE(ip1 < ip2);
    EXPECT_TRUE(ip1 < ip3);
    EXPECT_TRUE(ip1 < ip4);
    EXPECT_FALSE(ip2 < ip1);
    EXPECT_FALSE(ip1 < ip5);
    EXPECT_FALSE(ip5 < ip1);
    
    ip_filter::IPAddressV4 small("10.0.0.0");
    ip_filter::IPAddressV4 large("192.0.0.0");
    EXPECT_TRUE(small < large);
}

TEST_F(IPAddressV4Test, OutputOperator) {
    ip_filter::IPAddressV4 ip("192.168.1.1");
    std::ostringstream oss;
    oss << ip;
    EXPECT_EQ(oss.str(), "192.168.1.1");
}

TEST_F(IPAddressV4Test, InputOperator) {
    std::istringstream iss("192.168.1.1");
    ip_filter::IPAddressV4 ip;
    iss >> ip;
    EXPECT_EQ(ip.toString(), "192.168.1.1");
    EXPECT_TRUE(iss.good() || iss.eof());
}

TEST_F(IPAddressV4Test, InputOperatorInvalid) {
    std::istringstream iss("invalid.ip.address");
    ip_filter::IPAddressV4 ip;
    iss >> ip;
    EXPECT_TRUE(iss.fail());
}

TEST_F(IPAddressV4Test, InputOperatorMultiple) {
    std::istringstream iss("192.168.1.1 10.0.0.1 172.16.0.1");
    ip_filter::IPAddressV4 ip1, ip2, ip3;
    
    iss >> ip1 >> ip2 >> ip3;
    
    EXPECT_EQ(ip1.toString(), "192.168.1.1");
    EXPECT_EQ(ip2.toString(), "10.0.0.1");
    EXPECT_EQ(ip3.toString(), "172.16.0.1");
}

TEST_F(IPAddressV4Test, SetUsage) {
    std::set<ip_filter::IPAddressV4> ipSet;
    
    ipSet.insert(ip_filter::IPAddressV4("192.168.1.1"));
    ipSet.insert(ip_filter::IPAddressV4("10.0.0.1"));
    ipSet.insert(ip_filter::IPAddressV4("192.168.1.1"));
    ipSet.insert(ip_filter::IPAddressV4("172.16.0.1"));
    
    EXPECT_EQ(ipSet.size(), 3);
    
    auto it = ipSet.begin();
    EXPECT_EQ(it->toString(), "10.0.0.1");
    ++it;
    EXPECT_EQ(it->toString(), "172.16.0.1");
    ++it;
    EXPECT_EQ(it->toString(), "192.168.1.1");
}

TEST_F(IPAddressV4Test, BoundaryValues) {
    ip_filter::IPAddressV4 min_ip("0.0.0.0");
    EXPECT_EQ(min_ip.toString(), "0.0.0.0");
    
    ip_filter::IPAddressV4 max_ip("255.255.255.255");
    EXPECT_EQ(max_ip.toString(), "255.255.255.255");
    
    EXPECT_TRUE(min_ip < max_ip);
}

TEST_F(IPAddressV4Test, StreamChaining) {
    std::ostringstream oss;
    ip_filter::IPAddressV4 ip1("192.168.1.1");
    ip_filter::IPAddressV4 ip2("10.0.0.1");
    
    oss << ip1 << " " << ip2;
    EXPECT_EQ(oss.str(), "192.168.1.1 10.0.0.1");
}

// Тесты для isInSubnet
TEST_F(IPAddressV4Test, IsInSubnet_ClassC) {
    // Тест для подсети класса C (255.255.255.0)
    ip_filter::IPAddressV4 network("192.168.1.0");
    ip_filter::IPAddressV4 mask("255.255.255.0");
    
    // IP-адреса в подсети
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.1").isInSubnet(network, mask));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.100").isInSubnet(network, mask));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.254").isInSubnet(network, mask));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.0").isInSubnet(network, mask)); // сам адрес сети
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.255").isInSubnet(network, mask)); // broadcast
    
    // IP-адреса НЕ в подсети
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.2.1").isInSubnet(network, mask));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.167.1.1").isInSubnet(network, mask));
    EXPECT_FALSE(ip_filter::IPAddressV4("10.0.1.1").isInSubnet(network, mask));
}

TEST_F(IPAddressV4Test, IsInSubnet_ClassB) {
    // Тест для подсети класса B (255.255.0.0)
    ip_filter::IPAddressV4 network("172.16.0.0");
    ip_filter::IPAddressV4 mask("255.255.0.0");
    
    // IP-адреса в подсети
    EXPECT_TRUE(ip_filter::IPAddressV4("172.16.1.1").isInSubnet(network, mask));
    EXPECT_TRUE(ip_filter::IPAddressV4("172.16.254.254").isInSubnet(network, mask));
    EXPECT_TRUE(ip_filter::IPAddressV4("172.16.0.0").isInSubnet(network, mask));
    EXPECT_TRUE(ip_filter::IPAddressV4("172.16.255.255").isInSubnet(network, mask));
    
    // IP-адреса НЕ в подсети
    EXPECT_FALSE(ip_filter::IPAddressV4("172.17.1.1").isInSubnet(network, mask));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.1.1").isInSubnet(network, mask));
    EXPECT_FALSE(ip_filter::IPAddressV4("10.0.0.1").isInSubnet(network, mask));
}

TEST_F(IPAddressV4Test, IsInSubnet_ClassA) {
    // Тест для подсети класса A (255.0.0.0)
    ip_filter::IPAddressV4 network("10.0.0.0");
    ip_filter::IPAddressV4 mask("255.0.0.0");
    
    // IP-адреса в подсети
    EXPECT_TRUE(ip_filter::IPAddressV4("10.1.1.1").isInSubnet(network, mask));
    EXPECT_TRUE(ip_filter::IPAddressV4("10.255.255.255").isInSubnet(network, mask));
    EXPECT_TRUE(ip_filter::IPAddressV4("10.0.0.0").isInSubnet(network, mask));
    EXPECT_TRUE(ip_filter::IPAddressV4("10.168.1.100").isInSubnet(network, mask));
    
    // IP-адреса НЕ в подсети
    EXPECT_FALSE(ip_filter::IPAddressV4("11.0.0.1").isInSubnet(network, mask));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.1.1").isInSubnet(network, mask));
    EXPECT_FALSE(ip_filter::IPAddressV4("172.16.1.1").isInSubnet(network, mask));
}

TEST_F(IPAddressV4Test, IsInSubnet_CustomMasks) {
    // Тест с нестандартными масками
    ip_filter::IPAddressV4 network("192.168.0.0");
    
    // Маска /30 (255.255.255.252) - только 4 адреса
    ip_filter::IPAddressV4 mask30("255.255.255.252");
    ip_filter::IPAddressV4 network30("192.168.1.0");
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.0").isInSubnet(network30, mask30));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.1").isInSubnet(network30, mask30));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.2").isInSubnet(network30, mask30));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.3").isInSubnet(network30, mask30));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.1.4").isInSubnet(network30, mask30));
    
    // Маска /25 (255.255.255.128) - половина подсети класса C
    ip_filter::IPAddressV4 mask25("255.255.255.128");
    ip_filter::IPAddressV4 network25("192.168.1.0");
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.0").isInSubnet(network25, mask25));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.127").isInSubnet(network25, mask25));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.1.128").isInSubnet(network25, mask25));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.1.255").isInSubnet(network25, mask25));
}

TEST_F(IPAddressV4Test, IsInSubnet_EdgeCases) {
    // Тест граничных случаев
    
    // Маска 0.0.0.0 - все адреса в "подсети"
    ip_filter::IPAddressV4 mask_all("0.0.0.0");
    ip_filter::IPAddressV4 any_network("0.0.0.0");
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.1").isInSubnet(any_network, mask_all));
    EXPECT_TRUE(ip_filter::IPAddressV4("10.0.0.1").isInSubnet(any_network, mask_all));
    EXPECT_TRUE(ip_filter::IPAddressV4("255.255.255.255").isInSubnet(any_network, mask_all));
    
    // Маска 255.255.255.255 - только точное совпадение
    ip_filter::IPAddressV4 mask_exact("255.255.255.255");
    ip_filter::IPAddressV4 specific_ip("192.168.1.100");
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.100").isInSubnet(specific_ip, mask_exact));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.1.101").isInSubnet(specific_ip, mask_exact));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.1.99").isInSubnet(specific_ip, mask_exact));
}

TEST_F(IPAddressV4Test, IsInSubnet_RealWorldExamples) {
    // Примеры из реального мира
    
    // Частные подсети RFC 1918
    
    // 192.168.0.0/16
    ip_filter::IPAddressV4 private_192("192.168.0.0");
    ip_filter::IPAddressV4 mask_16("255.255.0.0");
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.1").isInSubnet(private_192, mask_16));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.255.254").isInSubnet(private_192, mask_16));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.167.1.1").isInSubnet(private_192, mask_16));
    
    // 172.16.0.0/12
    ip_filter::IPAddressV4 private_172("172.16.0.0");
    ip_filter::IPAddressV4 mask_12("255.240.0.0");
    
    EXPECT_TRUE(ip_filter::IPAddressV4("172.16.1.1").isInSubnet(private_172, mask_12));
    EXPECT_TRUE(ip_filter::IPAddressV4("172.31.255.255").isInSubnet(private_172, mask_12));
    EXPECT_FALSE(ip_filter::IPAddressV4("172.32.1.1").isInSubnet(private_172, mask_12));
    EXPECT_FALSE(ip_filter::IPAddressV4("172.15.1.1").isInSubnet(private_172, mask_12));
}

TEST_F(IPAddressV4Test, IsInSubnet_NonContiguousMasks) {
    // Тесты для неконтигуальных масок - проверка конкретных октетов
    
    // Маска 0.0.255.0 - проверяет только третий октет
    ip_filter::IPAddressV4 mask_third_octet("0.0.255.0");
    ip_filter::IPAddressV4 pattern_third("0.0.100.0"); // ищем третий октет = 100
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.100.1").isInSubnet(pattern_third, mask_third_octet));
    EXPECT_TRUE(ip_filter::IPAddressV4("10.0.100.255").isInSubnet(pattern_third, mask_third_octet));
    EXPECT_TRUE(ip_filter::IPAddressV4("172.16.100.50").isInSubnet(pattern_third, mask_third_octet));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.101.1").isInSubnet(pattern_third, mask_third_octet));
    EXPECT_FALSE(ip_filter::IPAddressV4("10.0.99.255").isInSubnet(pattern_third, mask_third_octet));
    
    // Маска 255.0.0.255 - проверяет первый и четвертый октеты
    ip_filter::IPAddressV4 mask_first_fourth("255.0.0.255");
    ip_filter::IPAddressV4 pattern_first_fourth("192.0.0.100"); // ищем 192.*.*.100
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.100").isInSubnet(pattern_first_fourth, mask_first_fourth));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.0.255.100").isInSubnet(pattern_first_fourth, mask_first_fourth));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.200.50.100").isInSubnet(pattern_first_fourth, mask_first_fourth));
    EXPECT_FALSE(ip_filter::IPAddressV4("193.168.1.100").isInSubnet(pattern_first_fourth, mask_first_fourth));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.1.101").isInSubnet(pattern_first_fourth, mask_first_fourth));
    
    // Маска 0.255.0.255 - проверяет второй и четвертый октеты
    ip_filter::IPAddressV4 mask_second_fourth("0.255.0.255");
    ip_filter::IPAddressV4 pattern_second_fourth("0.168.0.1"); // ищем *.168.*.1
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.1").isInSubnet(pattern_second_fourth, mask_second_fourth));
    EXPECT_TRUE(ip_filter::IPAddressV4("10.168.255.1").isInSubnet(pattern_second_fourth, mask_second_fourth));
    EXPECT_TRUE(ip_filter::IPAddressV4("172.168.0.1").isInSubnet(pattern_second_fourth, mask_second_fourth));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.169.1.1").isInSubnet(pattern_second_fourth, mask_second_fourth));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.1.2").isInSubnet(pattern_second_fourth, mask_second_fourth));
}

TEST_F(IPAddressV4Test, IsInSubnet_SingleOctetMatching) {
    // Тесты для проверки отдельных октетов
    
    // Проверка только первого октета
    ip_filter::IPAddressV4 mask_first("255.0.0.0");
    ip_filter::IPAddressV4 pattern_192("192.0.0.0");
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.1").isInSubnet(pattern_192, mask_first));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.0.0.0").isInSubnet(pattern_192, mask_first));
    EXPECT_TRUE(ip_filter::IPAddressV4("192.255.255.255").isInSubnet(pattern_192, mask_first));
    EXPECT_FALSE(ip_filter::IPAddressV4("193.168.1.1").isInSubnet(pattern_192, mask_first));
    
    // Проверка только второго октета
    ip_filter::IPAddressV4 mask_second("0.255.0.0");
    ip_filter::IPAddressV4 pattern_168("0.168.0.0");
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.1").isInSubnet(pattern_168, mask_second));
    EXPECT_TRUE(ip_filter::IPAddressV4("10.168.0.0").isInSubnet(pattern_168, mask_second));
    EXPECT_TRUE(ip_filter::IPAddressV4("172.168.255.255").isInSubnet(pattern_168, mask_second));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.169.1.1").isInSubnet(pattern_168, mask_second));
    
    // Проверка только четвертого октета
    ip_filter::IPAddressV4 mask_fourth("0.0.0.255");
    ip_filter::IPAddressV4 pattern_1("0.0.0.1");
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.1.1").isInSubnet(pattern_1, mask_fourth));
    EXPECT_TRUE(ip_filter::IPAddressV4("10.0.0.1").isInSubnet(pattern_1, mask_fourth));
    EXPECT_TRUE(ip_filter::IPAddressV4("172.16.254.1").isInSubnet(pattern_1, mask_fourth));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.1.2").isInSubnet(pattern_1, mask_fourth));
}

TEST_F(IPAddressV4Test, IsInSubnet_PatternMatching) {
    // Практические примеры использования масок для поиска паттернов
    
    // Найти все IP, где третий октет = 1 (маска 0.0.255.0)
    ip_filter::IPAddressV4 mask_pattern("0.0.255.0");
    ip_filter::IPAddressV4 pattern_subnet_1("0.0.1.0");
    
    std::vector<ip_filter::IPAddressV4> test_ips = {
        ip_filter::IPAddressV4("192.168.1.100"),   // match
        ip_filter::IPAddressV4("10.0.1.200"),      // match  
        ip_filter::IPAddressV4("172.16.1.50"),     // match
        ip_filter::IPAddressV4("192.168.2.100"),   // no match
        ip_filter::IPAddressV4("10.0.0.200")       // no match
    };
    
    EXPECT_TRUE(test_ips[0].isInSubnet(pattern_subnet_1, mask_pattern));
    EXPECT_TRUE(test_ips[1].isInSubnet(pattern_subnet_1, mask_pattern));
    EXPECT_TRUE(test_ips[2].isInSubnet(pattern_subnet_1, mask_pattern));
    EXPECT_FALSE(test_ips[3].isInSubnet(pattern_subnet_1, mask_pattern));
    EXPECT_FALSE(test_ips[4].isInSubnet(pattern_subnet_1, mask_pattern));
    
    // Найти все IP, которые заканчиваются на .1 (маска 0.0.0.255)
    ip_filter::IPAddressV4 mask_last_octet("0.0.0.255");
    ip_filter::IPAddressV4 pattern_ends_1("0.0.0.1");
    
    EXPECT_TRUE(ip_filter::IPAddressV4("192.168.100.1").isInSubnet(pattern_ends_1, mask_last_octet));
    EXPECT_TRUE(ip_filter::IPAddressV4("10.0.0.1").isInSubnet(pattern_ends_1, mask_last_octet));
    EXPECT_FALSE(ip_filter::IPAddressV4("192.168.1.100").isInSubnet(pattern_ends_1, mask_last_octet));
}
