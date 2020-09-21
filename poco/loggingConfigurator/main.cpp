#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/Foundation.h>
#include <Poco/Logger.h>
#include <Poco/PatternFormatter.h>
#include <Poco/SplitterChannel.h>
#include <Poco/Util/Application.h>

#include <iostream>

class MyApp : public Poco::Util::Application {
private:
  void initialize(Poco::Util::Application &self) {

    try {
      // loadConfiguration("./app.xml");
      loadConfiguration("./app.json");
      // loadConfiguration("./app.properties");

      Poco::Util::Application::initialize(self);
    } catch (Poco::Exception &ex) {
      std::cerr << ex.what() << std::endl;
    }
  }

  void uninitialize() { Poco::Util::Application::uninitialize(); }

  int main(const std::vector<std::string> &arguments) {
    try {
      Poco::Logger &logger = this->logger();
      // Poco::Logger &logger = Poco::Logger::root();
      logger.trace("console log and file log");
    } catch (Poco::Exception &ex) {
      std::cerr << ex.what() << std::endl;
    }

    return EXIT_OK;
  }
};

POCO_APP_MAIN(MyApp)
