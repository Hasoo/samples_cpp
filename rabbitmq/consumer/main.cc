#include <Poco/Exception.h>
#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/LayeredConfiguration.h>

#include <iostream>
#include <memory>

#include "rabbitmq_client.h"

class Receiver : SubscribeHandler {
   public:
    void onReceived(const char *message, const uint32_t &message_len) {
        logger_.trace(std::string(message, message_len));
    }

   private:
    Poco::Logger &logger_ = Poco::Logger::get("Application");
};

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
                logger.error("failed to authenticate(%s,%s), desc:%s", auth_id,
                             auth_pass, client->get_error_desc());
                return EXIT_FAILURE;
            }

            if (true != client->open_channel(channel)) {
                logger.error("failed to open channel(%d), desc:%s", channel,
                             client->get_error_desc());
                return EXIT_FAILURE;
            }

            // if (true != client->bind_queue(channel, queue_name,
            // exchange_name,
            //                                routing_key)) {
            //     logger.error("failed to bind queue(%s), desc:%s", queue_name,
            //                  client->get_error_desc());
            //     return EXIT_FAILURE;
            // }

            Receiver receiver;
            if (true !=
                client->subscribe_message(
                    channel, queue_name, false, true, false,
                    std::bind(&Receiver::onReceived, receiver,
                              std::placeholders::_1, std::placeholders::_2))) {
                logger.error("failed to subscribe, desc:%s",
                             client->get_error_desc());
                return EXIT_FAILURE;
            }

            if (true != client->start_consuming(1000)) {
                logger.error("failed to consume, desc:%s");
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
