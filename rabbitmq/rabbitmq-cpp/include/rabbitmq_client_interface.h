#ifndef __RABBITMQ_CLIENT_INTERFACE_H__
#define __RABBITMQ_CLIENT_INTERFACE_H__

#include <iostream>
#include <string>

class RabbitmqClientInterface {
   public:
    virtual bool connect(const std::string& ip, const int& port) = 0;
    virtual void close() = 0;

    virtual bool auth(const std::string& id, const std::string& pwd) = 0;

    virtual bool open_channel(const uint16_t& channel) = 0;
    virtual void close_channel(const uint16_t& channel) = 0;

    virtual bool bind_queue(const uint16_t& channel,
                            const std::string& queue_name,
                            const std::string& exchange_name,
                            const std::string& routing_key) = 0;
    virtual bool unbind_queue(const uint16_t& channel,
                              const std::string& queue_name,
                              const std::string& exchange_name,
                              const std::string& routing_key) = 0;

    virtual bool declare_exchange(const uint16_t& channel,
                                  const std::string& exchange_name,
                                  const std::string& exchange_type,
                                  const bool passive, const bool durable,
                                  const bool auto_delete,
                                  const bool internal) = 0;
    virtual bool delete_exchange(const uint16_t& channel,
                                 const std::string& exchange_name,
                                 const bool if_unused) = 0;

    virtual std::string get_error_desc() = 0;
    virtual uint32_t get_error_code() = 0;
};

#endif  //__RABBITMQ_CLIENT_INTERFACE_H__