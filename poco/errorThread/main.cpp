#include <Poco/ErrorHandler.h>
#include <Poco/Foundation.h>
#include <Poco/Logger.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>
#include <Poco/Util/Application.h>

#include <iostream>

class Offender : public Poco::Runnable {
  void run() { throw Poco::ApplicationException("got you"); }
};

class MyErrorHandler : public Poco::ErrorHandler {
public:
  void exception(const Poco::Exception &ex) {
    std::cerr << ex.displayText() << std::endl;
  }
  void exception(const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
  }

  void exception() { std::cerr << "unknown exception" << std::endl; }
};

class MyApp : public Poco::Util::Application {
protected:
  void initialize(Poco::Util::Application &self) {
    loadConfiguration();
    Poco::Util::Application::initialize(self);
  }

  void uninitialize() { Poco::Util::Application::uninitialize(); }

  int main(const std::vector<std::string> &arguments) {
    MyErrorHandler mh;
    Poco::ErrorHandler *peh = Poco::ErrorHandler::set(&mh);

    Offender offender;

    Poco::Thread thread;
    thread.start(offender);
    thread.join();

    Poco::ErrorHandler::set(peh);

    return EXIT_OK;
  }
};

POCO_APP_MAIN(MyApp)
