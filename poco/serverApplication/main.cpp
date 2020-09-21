#include <Poco/ConsoleChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Foundation.h>
#include <Poco/PatternFormatter.h>
#include <Poco/Util/ServerApplication.h>

#include <iostream>

using Poco::Util::Application;
using Poco::Util::ServerApplication;

class MyApp : public ServerApplication {
protected:
  void initialize(Application &application) {
    Poco::Logger &logger = this->logger();
    logger.setChannel(new Poco::FormattingChannel(
        new Poco::PatternFormatter("%L%H:%M:%S.%i [%P][%I] %p %t"),
        new Poco::ConsoleChannel));
    logger.setLevel("trace");

    loadConfiguration();
    ServerApplication::initialize(application);
  }

  void uninitialize() { ServerApplication::uninitialize(); }

  int main(const std::vector<std::string> &arguments) {
    Poco::Logger &logger = this->logger();

    if (config().getBool("application.runAsDaemon", false)) {
      logger.trace("Hello My Daemon App!!");
    } else {
      logger.trace("Hello My App!!");
      logger.trace(
          Poco::format("config->%s", config().getString("config.prop4")));
    }
    return EXIT_OK;
  }
};

POCO_SERVER_MAIN(MyApp)
