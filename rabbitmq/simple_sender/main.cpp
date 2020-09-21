#include <amqp.h>
#include <amqp_tcp_socket.h>

#include <Poco/Exception.h>
#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <Poco/Util/Application.h>

#include <iostream>

class MyApp : public Poco::Util::Application {
   private:
    void initialize(Poco::Util::Application &self) {
        loadConfiguration();

        Poco::Util::Application::initialize(self);
    }

    void uninitialize() { Poco::Util::Application::uninitialize(); }

    std::string amqp_error(amqp_rpc_reply_t x) {
        std::string desc("");

        switch (x.reply_type) {
            case AMQP_RESPONSE_NORMAL:
                break;
            case AMQP_RESPONSE_NONE:
                desc = "missing RPC reply type!";
                break;
            case AMQP_RESPONSE_LIBRARY_EXCEPTION:
                desc = amqp_error_string2(x.library_error);
                break;
            case AMQP_RESPONSE_SERVER_EXCEPTION:
                switch (x.reply.id) {
                    case AMQP_CONNECTION_CLOSE_METHOD: {
                        amqp_connection_close_t *m =
                            (amqp_connection_close_t *)x.reply.decoded;

                        desc = Poco::format(
                            "server connection error %d, message: %s",
                            m->reply_code,
                            std::string((char *)m->reply_text.bytes,
                                        (int)m->reply_text.len)
                                .c_str());
                        break;
                    }
                    case AMQP_CHANNEL_CLOSE_METHOD: {
                        amqp_channel_close_t *m =
                            (amqp_channel_close_t *)x.reply.decoded;

                        desc = Poco::format(
                            "server channel error %d, message: %s",
                            m->reply_code,
                            std::string((char *)m->reply_text.bytes,
                                        (int)m->reply_text.len)
                                .c_str());
                        break;
                    }
                    default:
                        desc = Poco::format(
                            "unknown server error, method id 0x%08X\n",
                            x.reply.id);
                        break;
                }
                break;
        }

        return desc;
    }

    Poco::Logger &_logger = Application::instance().logger();

    int main(const std::vector<std::string> &arguments) {
        // Poco::Logger &logger = Application::instance().logger();
        try {
            amqp_connection_state_t conn = amqp_new_connection();

            amqp_socket_t *socket = amqp_tcp_socket_new(conn);
            if (NULL == socket) {
                return EXIT_FAILURE;
            }

            int ret = amqp_socket_open(socket, "127.0.0.1", 5672);
            if (AMQP_STATUS_OK != ret) {
                _logger.error("failed to open tcp socket, result:%d", ret);
                return EXIT_FAILURE;
            }

            amqp_rpc_reply_t res =
                amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                           "test", "test");
            std::string desc = amqp_error(res);
            if (!desc.empty()) {
                _logger.error("failed to login, desc:%s", desc.c_str());
                return EXIT_FAILURE;
            }

            amqp_channel_open(conn, 1);
            res = amqp_get_rpc_reply(conn);
            desc = amqp_error(res);
            if (!desc.empty()) {
                _logger.error("failed to open channel, desc:%s", desc.c_str());
                return EXIT_FAILURE;
            }

            std::string exchange("amq.direct"), routeKey("router");
            std::string message(
                "{\"msgKey\":\"7d2c55e9524f4bf4bb1e3e30d428b233\",\"userKey\":"
                "\"1\",\"groupname\":\"TEST\",\"username\":\"test\","
                "\"resDate\":1548131806368,\"msgType\":\"SMS\",\"contentType\":"
                "\"SMS\",\"phone\":\"01029663620\",\"callback\":"
                "\"01022222222\",\"message\":\"testmessage\",\"code\":null,"
                "\"desc\":null,\"doneDate\":null,\"net\":null,\"routingType\":"
                "\"order\",\"senders\":[{\"name\":\"SKT_1\",\"attr\":\"SKT\","
                "\"level\":1},{\"name\":\"KT_1\",\"attr\":\"KT\",\"level\":3}]"
                "}");

            // amqp_basic_properties_t props;
            // props._flags =
            //     AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
            // props.content_type = amqp_cstring_bytes("text/plain");
            // props.delivery_mode = 2; /* persistent delivery mode */
            std::string content_type("application/json");
            std::string content_encoding("UTF-8");

            amqp_table_entry_t entries[1];
            amqp_table_t table;
            entries[0].key = amqp_cstring_bytes("__TypeId__");
            entries[0].value.kind = AMQP_FIELD_KIND_UTF8;
            entries[0].value.value.bytes =
                amqp_cstring_bytes("com.iheart.message.ircs.queue.SenderQue");
            table.num_entries = 1;
            table.entries = entries;
            amqp_basic_properties_t prop;
            prop._flags = AMQP_BASIC_CONTENT_TYPE_FLAG |
                          AMQP_BASIC_DELIVERY_MODE_FLAG |
                          AMQP_BASIC_HEADERS_FLAG;
            prop.delivery_mode = 2;
            prop.headers = table;

            prop.content_type = amqp_cstring_bytes(content_type.c_str());
            prop.content_encoding =
                amqp_cstring_bytes(content_encoding.c_str());

            for (int i = 0; i < 1; ++i) {
                ret = amqp_basic_publish(
                    conn, 1, amqp_cstring_bytes(exchange.c_str()),
                    amqp_cstring_bytes(routeKey.c_str()), 0, 0, &prop,
                    amqp_cstring_bytes(message.c_str()));
                if (0 > ret) {
                    _logger.error("failed to publish, desc:%s",
                                  amqp_error_string2(ret));
                    return EXIT_FAILURE;
                }
            }

            amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
            amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
            amqp_destroy_connection(conn);
        } catch (Poco::Exception &ex) {
            _logger.error(ex.what());
        }

        return EXIT_OK;
    }
};

POCO_APP_MAIN(MyApp)