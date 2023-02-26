//
// Created by calt on 18-10-22.
//

#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <strstream>
#include <iostream>
#include "networklet.h"
#include "ping.cpp"

//bool check_ip_available(std::string ip, int port) {
//
//    std::stringstream ss;
//    ss << port;
//    std::string port_str = ss.str();
//
//    try {
//        boost::asio::io_service io_service;
//        boost::asio::ip::tcp::resolver resolver(io_service);
//
//        std::cout << "query begin" << std::endl;
//        boost::asio::ip::address::from_string(ip);
//        boost::asio::ip::tcp::resolver::query query(ip, port_str);
//        std::cout << ip << std::endl;
//        std::cout << "query end" << std::endl;
//        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
//
//        boost::asio::ip::tcp::resolver::iterator end;
//        while (iterator != end)
//        {
//            boost::asio::ip::tcp::endpoint endpoint = *iterator++;
//            std::cout << endpoint << std::endl;
//        }
//    } catch (std::exception& e) {
//        return false;
//    }
//    return true;
//}

//bool check_ip_available(std::string ip, int port) {
//    try
//    {
//        boost::asio::io_service io_service;
//        pinger p(io_service, ip.c_str());
//        io_service.run();
//    }
//    catch (std::exception& e)
//    {
//        std::cerr << "Exception: " << e.what() << std::endl;
//        return false;
//    }
//    return true;
//}

bool check_ip_available(std::string ip, int port) {
    return true;
}