#include <Poco/Delegate.h>
#include <Poco/Exception.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/Foundation.h>
#include <Poco/Logger.h>
#include <Poco/NObserver.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Thread.h>
#include <Poco/Util/Application.h>

#include <iostream>

class MyApp : public Poco::Util::Application {
   private:
    void initialize(Poco::Util::Application &self) {
        loadConfiguration();

        Poco::Util::Application::initialize(self);
    }

    void uninitialize() { Poco::Util::Application::uninitialize(); }

    int main(const std::vector<std::string> &arguments) {
        try {
            Poco::Logger &logger = Application::instance().logger();
            // Poco::Logger &logger = this->logger();
            // Poco::Logger &logger = Poco::Logger::root();
            unsigned short port = (unsigned short)config().getInt("reactor.port", 9977);

            Poco::Net::ServerSocket serverSock(port);
            Poco::Net::SocketReactor reactor;
        } catch (Poco::Exception &ex) {
            std::cerr << ex.what() << std::endl;
        }

        return EXIT_OK;
    }
};

POCO_APP_MAIN(MyApp)