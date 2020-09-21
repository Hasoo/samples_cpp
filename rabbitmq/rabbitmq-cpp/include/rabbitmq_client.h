#ifndef __RABBITMQ_CLIENT_IMPL_H__
#define __RABBITMQ_CLIENT_IMPL_H__

#include <amqp.h>
#include <functional>
#include <string>

class SubscribeHandler {
    virtual void onReceived(const char* message,
                            const uint32_t& message_len) = 0;
};

class RabbitmqClient {
   public:
    RabbitmqClient();
    ~RabbitmqClient();

    bool connect(const std::string& ip, const int& port);
    void close();

    bool auth(const std::string& id, const std::string& pwd);

    bool open_channel(const uint16_t& channel);
    void close_channel(const uint16_t& channel);

    bool bind_queue(const uint16_t& channel, const std::string& queue_name,
                    const std::string& exchange_name,
                    const std::string& routing_key);
    bool unbind_queue(const uint16_t& channel, const std::string& queue_name,
                      const std::string& exchange_name,
                      const std::string& routing_key);

    bool declare_exchange(const uint16_t& channel,
                          const std::string& exchange_name,
                          const std::string& exchange_type, const bool passive,
                          const bool durable, const bool auto_delete,
                          const bool internal);
    bool delete_exchange(const uint16_t& channel,
                         const std::string& exchange_name,
                         const bool if_unused);

    bool declare_queue(const uint16_t& channel, const std::string& queue_name,
                       const bool passive, const bool durable,
                       const bool exclusive, const bool auto_delete);
    bool delete_queue(const uint16_t& channel, const std::string& queue_name,
                      const bool if_unused, const bool if_empty);

    bool publish_message(const uint16_t& channel,
                         const std::string& exchange_name,
                         const std::string& routing_key, const char* message,
                         const uint32_t& message_len);

    bool subscribe_message(
        const uint16_t& channel, const std::string& queue_name,
        const bool no_local, const bool no_ack, const bool exclusive,
        std::function<void(const char*, const uint32_t&)> fp);
    bool start_consuming(const uint64_t& timeout);

    std::string get_error_desc() { return error_desc_; }
    uint32_t get_error_code() { return error_code_; }

   private:
    RabbitmqClient(const RabbitmqClient&);
    void operator=(const RabbitmqClient&);

    bool amqp_error(const amqp_rpc_reply_t& x);

    amqp_connection_state_t conn_ = NULL;
    std::string error_desc_;
    uint32_t error_code_;

    std::function<void(const char*, const uint32_t&)> fp_;
};

#endif  //__RABBITMQ_CLIENT_IMPL_H__