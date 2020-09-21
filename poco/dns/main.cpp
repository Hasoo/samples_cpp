#include <Poco/ConsoleChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Logger.h>
#include <Poco/Net/DNS.h>
#include <Poco/PatternFormatter.h>
#include <iostream>

using Poco::Net::DNS;
using Poco::Net::IPAddress;
using Poco::Net::HostEntry;

int main(int argc, char *argv[]) {

  const HostEntry &entry = DNS::hostByName("www.appinf.com");

  Poco::Logger &logger = Poco::Logger::root();
  logger.setLevel(Poco::Message::PRIO_TRACE);
  logger.setChannel(new Poco::FormattingChannel(
      new Poco::PatternFormatter("%L%H:%M:%S.%i [%P] %t"),
      new Poco::ConsoleChannel));

  logger.trace(Poco::format("Canonical Name: %s", entry.name()));

  const HostEntry::AliasList &aliases = entry.aliases();

  for (auto it = aliases.begin(); it != aliases.end(); ++it)
    logger.trace(Poco::format("Alias: %s", *it));

  const HostEntry::AddressList &addrs = entry.addresses();

  for (auto it = addrs.begin(); it != addrs.end(); ++it)
    logger.trace(Poco::format("Address: %s", it->toString()));

  return 0;
}
