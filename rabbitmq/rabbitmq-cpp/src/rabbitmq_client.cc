#include <iostream>
#include <memory>

#include <Poco/Format.h>
#include <amqp_tcp_socket.h>

#include "rabbitmq_client.h"

RabbitmqClient::RabbitmqClient() {}

RabbitmqClient::~RabbitmqClient() { close(); }

bool RabbitmqClient::connect(const std::string& ip, const int& port) {
    conn_ = amqp_new_connection();
    amqp_socket_t* socket = amqp_tcp_socket_new(conn_);
    if (NULL == socket) {
        error_desc_ = "failed to create new socket";
        return false;
    }

    int ret = amqp_socket_open(socket, ip.c_str(), port);
    if (AMQP_STATUS_OK != ret) {
        error_desc_ = Poco::format("failed to open tcp socket, result:%d", ret);
        return false;
    }

    return true;
}

bool RabbitmqClient::auth(const std::string& id, const std::string& pwd) {
    return amqp_error(amqp_login(conn_, "/", 0, 131072, 0,
                                 AMQP_SASL_METHOD_PLAIN, id.c_str(),
                                 pwd.c_str()));
}

bool RabbitmqClient::open_channel(const uint16_t& channel) {
    amqp_channel_open(conn_, channel);
    return amqp_error(amqp_get_rpc_reply(conn_));
}

bool RabbitmqClient::bind_queue(const uint16_t& channel,
                                const std::string& queue_name,
                                const std::string& exchange_name,
                                const std::string& routing_key) {
    amqp_queue_bind(conn_, channel, amqp_cstring_bytes(queue_name.c_str()),
                    amqp_cstring_bytes(exchange_name.c_str()),
                    amqp_cstring_bytes(routing_key.c_str()), amqp_empty_table);
    return amqp_error(amqp_get_rpc_reply(conn_));
}

bool RabbitmqClient::unbind_queue(const uint16_t& channel,
                                  const std::string& queue_name,
                                  const std::string& exchange_name,
                                  const std::string& routing_key) {
    amqp_queue_unbind(conn_, channel, amqp_cstring_bytes(queue_name.c_str()),
                      amqp_cstring_bytes(exchange_name.c_str()),
                      amqp_cstring_bytes(routing_key.c_str()),
                      amqp_empty_table);
    return amqp_error(amqp_get_rpc_reply(conn_));
}

bool RabbitmqClient::declare_exchange(const uint16_t& channel,
                                      const std::string& exchange_name,
                                      const std::string& exchange_type,
                                      const bool passive, const bool durable,
                                      const bool auto_delete,
                                      const bool internal) {
    amqp_exchange_declare(conn_, channel,
                          amqp_cstring_bytes(exchange_name.c_str()),
                          amqp_cstring_bytes(exchange_type.c_str()), passive,
                          durable, auto_delete, internal, amqp_empty_table);
    return amqp_error(amqp_get_rpc_reply(conn_));
}
bool RabbitmqClient::delete_exchange(const uint16_t& channel,
                                     const std::string& exchange_name,
                                     const bool if_unused) {
    amqp_exchange_delete(conn_, channel,
                         amqp_cstring_bytes(exchange_name.c_str()), if_unused);
    return amqp_error(amqp_get_rpc_reply(conn_));
}

bool RabbitmqClient::declare_queue(const uint16_t& channel,
                                   const std::string& queue_name,
                                   const bool passive, const bool durable,
                                   const bool exclusive,
                                   const bool auto_delete) {
    amqp_queue_declare(conn_, channel, amqp_cstring_bytes(queue_name.c_str()),
                       passive, durable, exclusive, auto_delete,
                       amqp_empty_table);
    return amqp_error(amqp_get_rpc_reply(conn_));
}
bool RabbitmqClient::delete_queue(const uint16_t& channel,
                                  const std::string& queue_name,
                                  const bool if_unused, const bool if_empty) {
    amqp_queue_delete(conn_, channel, amqp_cstring_bytes(queue_name.c_str()),
                      if_unused, if_empty);
    return amqp_error(amqp_get_rpc_reply(conn_));
}

bool RabbitmqClient::publish_message(const uint16_t& channel,
                                     const std::string& exchange_name,
                                     const std::string& routing_key,
                                     const char* message,
                                     const uint32_t& message_len) {
    std::unique_ptr<char[]> ptr(new char[message_len]);
    memcpy(ptr.get(), message, message_len);
    amqp_bytes_t message_bytes;
    message_bytes.len = message_len;
    message_bytes.bytes = reinterpret_cast<void*>(ptr.get());
    int ret = amqp_basic_publish(
        conn_, channel, amqp_cstring_bytes(exchange_name.c_str()),
        amqp_cstring_bytes(routing_key.c_str()), 0, 0, NULL, message_bytes);
    if (ret != AMQP_STATUS_OK) {
        error_desc_ = amqp_error_string2(ret);
        return false;
    }
    return true;
}

bool RabbitmqClient::subscribe_message(
    const uint16_t& channel, const std::string& queue_name, const bool no_local,
    const bool no_ack, const bool exclusive,
    std::function<void(const char*, const uint32_t&)> fp) {
    amqp_basic_consume(conn_, channel, amqp_cstring_bytes(queue_name.c_str()),
                       amqp_empty_bytes, no_local, no_ack, exclusive,
                       amqp_empty_table);
    fp_ = fp;
    return amqp_error(amqp_get_rpc_reply(conn_));
}
bool RabbitmqClient::start_consuming(const uint64_t& timeout) {
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    amqp_frame_t frame;

    for (;;) {
        amqp_envelope_t envelope;

        amqp_maybe_release_buffers(conn_);
        amqp_rpc_reply_t ret = amqp_consume_message(conn_, &envelope, &tv, 0);
        if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
            if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type &&
                AMQP_STATUS_UNEXPECTED_STATE == ret.library_error) {
                int ret_frame = amqp_simple_wait_frame(conn_, &frame);
                if (AMQP_STATUS_OK != ret_frame) {
                    error_code_ = ret_frame;
                    return false;
                }

                if (AMQP_FRAME_METHOD == frame.frame_type) {
                    switch (frame.payload.method.id) {
                        case AMQP_BASIC_ACK_METHOD:
                            /* if we've turned publisher confirms on, and we've
                             * published a message here is a message being
                             * confirmed.
                             */
                            break;
                        case AMQP_BASIC_RETURN_METHOD:
                            /* if a published message couldn't be routed and the
                             * mandatory flag was set this is what would be
                             * returned. The message then needs to be read.
                             */
                            {
                                amqp_message_t message;
                                ret = amqp_read_message(conn_, frame.channel,
                                                        &message, 0);
                                if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
                                    return false;
                                }
                                amqp_destroy_message(&message);
                            }

                            break;

                        case AMQP_CHANNEL_CLOSE_METHOD:
                            /* a channel.close method happens when a channel
                             * exception occurs, this can happen by publishing
                             * to an exchange that doesn't exist for example.
                             *
                             * In this case you would need to open another
                             * channel redeclare any queues that were declared
                             * auto-delete, and restart any consumers that were
                             * attached to the previous channel.
                             */
                            return false;

                        case AMQP_CONNECTION_CLOSE_METHOD:
                            /* a connection.close method happens when a
                             * connection exception occurs, this can happen by
                             * trying to use a channel that isn't open for
                             * example.
                             *
                             * In this case the whole connection must be
                             * restarted.
                             */
                            return false;
                        default:
                            error_desc_ = Poco::format(
                                "An unexpected method was received %u",
                                frame.payload.method.id);
                            return false;
                    }
                }
            }
        } else {
            fp_(reinterpret_cast<const char*>(envelope.message.body.bytes),
                envelope.message.body.len);

            amqp_destroy_envelope(&envelope);
        }
    }
    return true;
}

void RabbitmqClient::close_channel(const uint16_t& channel) {
    if (NULL != conn_) {
        amqp_channel_close(conn_, channel, AMQP_REPLY_SUCCESS);
    }
}
void RabbitmqClient::close() {
    if (NULL != conn_) {
        amqp_connection_close(conn_, AMQP_REPLY_SUCCESS);
        amqp_destroy_connection(conn_);
        conn_ = NULL;
    }
}

bool RabbitmqClient::amqp_error(const amqp_rpc_reply_t& x) {
    bool ret = false;

    switch (x.reply_type) {
        case AMQP_RESPONSE_NORMAL:
            error_desc_.empty();
            error_code_ = 1000;
            ret = true;
            break;
        case AMQP_RESPONSE_NONE:
            error_desc_ = "missing RPC reply type!";
            break;
        case AMQP_RESPONSE_LIBRARY_EXCEPTION:
            error_desc_ = amqp_error_string2(x.library_error);
            break;
        case AMQP_RESPONSE_SERVER_EXCEPTION:
            switch (x.reply.id) {
                case AMQP_CONNECTION_CLOSE_METHOD: {
                    amqp_connection_close_t* m =
                        (amqp_connection_close_t*)x.reply.decoded;
                    error_desc_ = Poco::format(
                        "server connection error, code:%d desc:%s "
                        "lib_desc:%s",
                        m->reply_code,
                        std::string((char*)m->reply_text.bytes,
                                    (int)m->reply_text.len),
                        std::string(amqp_error_string2(x.library_error)));
                    break;
                }
                case AMQP_CHANNEL_CLOSE_METHOD: {
                    amqp_channel_close_t* m =
                        (amqp_channel_close_t*)x.reply.decoded;
                    error_desc_ = Poco::format(
                        "server channel error, code:%hu desc:%s", m->reply_code,
                        std::string((char*)m->reply_text.bytes,
                                    (int)m->reply_text.len));
                    break;
                }
                default:
                    error_desc_ = Poco::format(
                        "unknown server error, method id 0x%08X\n", x.reply.id);
                    break;
            }
            break;
    }

    return ret;
}
