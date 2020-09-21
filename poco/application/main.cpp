#include <Poco/Util/Application.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionCallback.h>
#include <Poco/Util/OptionSet.h>

#include <iostream>

class OptionExample : public Poco::Util::Application {
protected:
  void initialize(Poco::Util::Application &application) {

    this->loadConfiguration();
    Poco::Util::Application::initialize(application);
  }

  void uninitialize() { Poco::Util::Application::uninitialize(); }

  void defineOptions(Poco::Util::OptionSet &optionSet) {
    this->config().setString("optionval", "aa");
    Poco::Util::Application::defineOptions(optionSet);
    optionSet.addOption(Poco::Util::Option("optionval", "", "Some value")
                            .required(false)
                            .repeatable(false)
                            .argument("<the value>", true)
                            .callback(Poco::Util::OptionCallback<OptionExample>(
                                this, &OptionExample::handleMyOpt)));
  }

  // ./test --optionval=hasoo
  void handleMyOpt(const std::string &name, const std::string &value) {
    std::cout << "Setting option " << name << " to " << value << std::endl;
    this->config().setString(name, value);
    std::cout << "The option is now " << this->config().getString(name)
              << std::endl;
  }

  int main(const std::vector<std::string> &arguments) {

    std::cout << "We are now in main. Option is "
              << this->config().getString("optionval") << std::endl;

    Poco::Util::Application &application = Poco::Util::Application::instance();
    std::cout << application.config().getString("optionval") << std::endl;
    std::cout << application.config().getString("application.name")
              << std::endl;
    std::cout << application.config().getString("application.configDir")
              << std::endl;
    return EXIT_OK;
  }
};

POCO_APP_MAIN(OptionExample)
