#include <Poco/AutoPtr.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Foundation.h>
#include <Poco/Logger.h>
#include <Poco/PatternFormatter.h>
#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/JSONConfiguration.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#include <Poco/Util/XMLConfiguration.h>

#include <iostream>
#include <memory>

class MyApp : public Poco::Util::Application {
private:
  void initialize(Poco::Util::Application &self) {
    Poco::Util::Application::initialize(self);
  }

  void uninitialize() { Poco::Util::Application::uninitialize(); }

  int main(const std::vector<std::string> &arguments) {
    Poco::Logger &logger = this->logger();
    logger.setChannel(new Poco::FormattingChannel(
        new Poco::PatternFormatter("%L%H:%M:%S.%i [%P] %t"),
        new Poco::ConsoleChannel));
    logger.setLevel("trace");

    { // get keys
      logger.trace("get keys");

      std::shared_ptr<Poco::Util::AbstractConfiguration> p =
          std::make_shared<Poco::Util::AbstractConfiguration>("app.xml");

      Poco::Util::AbstractConfiguration::Keys props;
      p->keys(props);
      for (auto it = props.begin(); it != props.end(); ++it) {
        logger.trace(*it);
      }
    }

    {
      logger.trace("XMLConfiguration----------->");

      Poco::AutoPtr<Poco::Util::XMLConfiguration> pConf(
          new Poco::Util::XMLConfiguration("app.xml"));

      logger.trace(pConf->getString("logging.channels.c1.class"));

      Poco::AutoPtr<Poco::Util::AbstractConfiguration> abConf(
          pConf->createView("logging.channels"));

      Poco::Util::AbstractConfiguration::Keys props;
      abConf->keys(props);
      for (auto it = props.begin(); it != props.end(); ++it) {
        logger.trace(*it);
      }
    }

    {
      logger.trace("PropertyFileConfiguration----------->");

      Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> pConf(
          new Poco::Util::PropertyFileConfiguration("app.properties"));

      logger.trace(pConf->getString("logging.channels.c1.class"));

      Poco::AutoPtr<Poco::Util::AbstractConfiguration> abConf(
          pConf->createView("logging.channels"));

      Poco::Util::AbstractConfiguration::Keys props;
      abConf->keys(props);
      for (auto it = props.begin(); it != props.end(); ++it) {
        logger.trace(*it);
      }
    }

    {
      logger.trace("JSONConfiguration----------->");

      Poco::AutoPtr<Poco::Util::JSONConfiguration> pConf(
          new Poco::Util::JSONConfiguration("app.json"));

      logger.trace(pConf->getString("logging.channels.c1.class"));

      Poco::AutoPtr<Poco::Util::AbstractConfiguration> abConf(
          pConf->createView("logging.channels"));

      Poco::Util::AbstractConfiguration::Keys props;
      abConf->keys(props);
      for (auto it = props.begin(); it != props.end(); ++it) {
        logger.trace(*it);
      }
    }

    return EXIT_OK;
  }
};

POCO_APP_MAIN(MyApp)
