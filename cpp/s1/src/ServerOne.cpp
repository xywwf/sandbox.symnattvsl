#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>

#include "Config.hpp"
#include "ServerOne.hpp"

ServerOne::ServerOne()
        : network()
        , server(network)
        , clients()
        , stopped(false)
{
}
bool ServerOne::f1()
{
    int port = Config::getPort(Config::S1_);

    server.setBlocking(false);
    server.startAccept(cppsocket::ANY_ADDRESS, port);

    server.setAcceptCallback([&](
            cppsocket::Socket&,
            cppsocket::Socket& client) {
        std::cout
                << "Client Connected:"
                << std::endl;

        client.setReadCallback([&](
                cppsocket::Socket& socket,
                const std::vector<uint8_t>& data) {
            std::cout
                    << "Read Client: "
                    << cppsocket::ipToString(socket.getRemoteIPAddress())
                    << std::endl
                    << data.data()
                    << std::endl;
        });

        client.startRead();
        std::ostringstream oss;
        oss
                << ".s1"
                << std::endl
                << "Local IP Address:"
                << cppsocket::ipToString(client.getLocalIPAddress())
                << std::endl
                << "Local Port:"
                << client.getLocalPort()
                << std::endl
                << "Remote IP Address:"
                << cppsocket::ipToString(client.getRemoteIPAddress())
                << std::endl
                << "Remote Port:"
                << client.getRemotePort()
                << std::endl;
        const std::string& str = oss.str();
        std::vector<uint8_t> vector(str.begin(), str.end());
        client.send(vector);
        client.setCloseCallback([&](
                cppsocket::Socket& socket) {
            std::cout
                    << "Client Disconnected: "
                    << cppsocket::ipToString(socket.getRemoteIPAddress())
                    << std::endl;

            for (auto
                 iterator = clients.begin();
                 iterator != clients.end();)
            {
                if (&(*iterator) == &socket)
                {
                    iterator = clients.erase(iterator);
                    break;
                }
                else
                {
                    ++iterator;
                }
            }

        });
        clients.push_back(std::move(client));
    });

    const std::chrono::microseconds sleepTime(10000);
    while (!stopped) {
        network.update();

        std::this_thread::sleep_for(sleepTime);
    }

    return false;
}
bool ServerOne::f2()
{
    return false;
}
bool ServerOne::f3()
{
    return false;
}
void ServerOne::stop() {
    stopped = true;
}
