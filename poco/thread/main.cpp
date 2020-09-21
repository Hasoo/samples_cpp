#include <Poco/AutoPtr.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/Format.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Logger.h>
#include <Poco/Message.h>
#include <Poco/PatternFormatter.h>
#include <Poco/Process.h>
#include <Poco/Runnable.h>
#include <Poco/SignalHandler.h>
#include <Poco/SplitterChannel.h>
#include <Poco/Thread.h>
#include <Poco/Util/Application.h>

#include <iostream>

using Poco::Util::Application;
using Poco::ConsoleChannel;
using Poco::SplitterChannel;
using Poco::FileChannel;
using Poco::FormattingChannel;
using Poco::Formatter;
using Poco::PatternFormatter;
using Poco::Logger;
using Poco::Message;

class HelloRunnable : public Poco::Runnable {
public:
  virtual ~HelloRunnable() { std::cout << "thread exit" << std::endl; }
  virtual void run() {
    Logger *logger = &Logger::get("TestLog");
    while (true) {
      Poco::Thread::sleep(1000);
      // logger->trace(Poco::format("hello, world!->%s", std::string("abc")));
      logger->trace(Poco::format("[%d] hello, world!->%d",
                                 static_cast<int>(Poco::Process::id()),
                                 static_cast<int>(Poco::Thread::currentTid())));
    }
  }
};

class MyApp : public Application {
private:
  Logger *_logger;

protected:
  void initialize(Application &self) {

    this->loadConfiguration();

    SplitterChannel *pSplitterChannel = new SplitterChannel();
    pSplitterChannel->addChannel(new ConsoleChannel());
    pSplitterChannel->addChannel(new FileChannel("test.log"));

    _logger = &Logger::create(
        "TestLog", new FormattingChannel(new PatternFormatter("%h-%M-%S.%i %t"),
                                         pSplitterChannel),
        Message::PRIO_TRACE);

    Application::initialize(self);
  }

  void uninitialize() { Application::uninitialize(); }

  int main(const std::vector<std::string> &arguments) {
    _logger->trace("start");
    try {
      poco_throw_on_signal;

      HelloRunnable runnable;

      Poco::Thread thread;

      thread.start(runnable);
      thread.join();
    } catch (Poco::Exception &ex) {
      std::cerr << ex.message() << std::endl;
    }

    _logger->trace("exit main");
    Poco::Thread::sleep(5000);

    return EXIT_OK;
  }
};

POCO_APP_MAIN(MyApp)
