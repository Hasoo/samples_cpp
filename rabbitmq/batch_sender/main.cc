#include <Poco/Exception.h>
#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <Poco/Stopwatch.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/LayeredConfiguration.h>

#include <iostream>
#include <memory>
#include <sstream>

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

            Poco::Util::LayeredConfiguration &config =
                Application::instance().config();

            std::string server_ip = config.getString("rabbitmq.ip");
            int server_port = config.getInt("rabbitmq.port");
            std::string auth_id = config.getString("rabbitmq.id");
            std::string auth_pass = config.getString("rabbitmq.password");
            int channel = config.getInt("rabbitmq.channel");
            std::string exchange_name = config.getString("rabbitmq.exchange");
            std::string routing_key = config.getString("rabbitmq.routing_key");
            std::string queue_name = config.getString("rabbitmq.queue");

            if (true != client->connect(server_ip, server_port)) {
                logger.error("failed to connect(%s:%d), desc:%s", server_ip,
                             server_port, client->get_error_desc());
                return EXIT_FAILURE;
            }

            if (true != client->auth(auth_id, auth_pass)) {
                logger.error("failed to authenticate, desc:%s",
                             client->get_error_desc());
                return EXIT_FAILURE;
            }

            if (true != client->open_channel(channel)) {
                logger.error("failed to open channel(%d), desc:%s", channel,
                             client->get_error_desc());
                return EXIT_FAILURE;
            }

            // for (int loop = 0; loop < 5; ++loop) {
            Poco::Stopwatch stop_watch;
            stop_watch.start();
            const std::string name("Hasoo Kim's ");
            for (int i = 0; i < 5000000; ++i) {
                std::ostringstream os;
                os << name << i;
                if (true != client->publish_message(
                                channel, exchange_name, routing_key,
                                os.str().c_str(), os.str().size())) {
                    logger.error("failed to publish, desc:%s",
                                 client->get_error_desc());
                    break;
                }
            }
            stop_watch.stop();
            logger.trace("%ld", stop_watch.elapsed());
            // }

            client->close_channel(channel);
            client->close();
        } catch (Poco::Exception &ex) {
            logger.error(ex.what());
        }

        return EXIT_OK;
    }
};

POCO_APP_MAIN(MyApp)
