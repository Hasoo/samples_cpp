#include <Poco/Exception.h>
#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <Poco/Util/Application.h>

#include <iostream>
#include <memory>

#include "rabbitmq_client.h"

class MyApp : public Poco::Util::Application {
   private:
    void initialize(Poco::Util::Application &self) {
        loadConfiguration();

        Poco::Util::Application::initialize(self);
    }

    void uninitialize() { Poco::Util::Application::uninitialize(); }

    int main(const std::vector<std::string> &arguments) {
        Poco::Logger &logger = Application::instance().logger();
        try {
            auto client = std::make_unique<RabbitmqClient>();

            if (true != client->connect("127.0.0.1", 5672)) {
                logger.error("failed to connect, desc:%s", client->get_error_desc());
                return EXIT_FAILURE;
            }

            if (true != client->auth("test", "test")) {
                logger.error("failed to authenticate, desc:%s", client->get_error_desc());
                return EXIT_FAILURE;
            }
            const uint16_t channel = 1;
            if (true != client->open_channel(channel)) {
                logger.error("failed to open channel(%d), desc:", channel, client->get_error_desc());
                return EXIT_FAILURE;
            }

            if (true !=
                client->bind_queue(channel, "HasooQue", "HasooEx", "Hasoo#")) {
                logger.error(client->get_error_desc());
                return EXIT_FAILURE;
            }

            client->close_channel(channel);
            client->close();
        } catch (Poco::Exception &ex) {
            logger.error(ex.what());
        }

        return EXIT_OK;
    }
};

POCO_APP_MAIN(MyApp)
