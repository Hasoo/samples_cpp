#include <Poco/AsyncChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/Exception.h>
#include <Poco/FileChannel.h>
#include <Poco/Format.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Logger.h>
#include <Poco/Message.h>
#include <Poco/PatternFormatter.h>
#include <Poco/Runnable.h>
#include <Poco/SplitterChannel.h>
#include <Poco/Thread.h>
#include <Poco/Util/Application.h>

#include <iostream>
#include <memory>

class MassiveLogger : public Poco::Runnable {
private:
  int _num;
  std::string _name;

  Poco::FastMutex _mutex;

  int getPlusNum() {
    Poco::ScopedLock lock(this->_mutex);
    return ++this->_num;
  }

public:
  MassiveLogger() : _num(0), _name("") {}
  MassiveLogger(const std::string &name) : _num(0), _name(name) {}

  void setName(const std::string &name) { this->_name = name; }
  virtual void run() {
    Poco::Logger &logger = Poco::Logger::get("async_logger");
    for (int i = 0; i < 100000; ++i) {
      logger.trace(
          Poco::format("[%s]massive log->%d", this->_name, this->getPlusNum()));
    }
  }
};

class MyApp : public Poco::Util::Application {
private:
  void initialize(Poco::Util::Application &self) {

    this->loadConfiguration();

    Poco::Util::Application::initialize(self);
  }

  void uninitialize() { Poco::Util::Application::uninitialize(); }

  int main(const std::vector<std::string> &arguments) {
    std::string logPath("./"), logFilename("massive.log");
    Poco::FileChannel *pFileChannel = new Poco::FileChannel;
    pFileChannel->setProperty("rotation", "100 M");
    pFileChannel->setProperty("archive", "timestamp");
    pFileChannel->setProperty("path",
                              Poco::format("%s/%s", logPath, logFilename));

    Poco::AsyncChannel *pAsyncChannel = new Poco::AsyncChannel(pFileChannel);

    Poco::Logger::create(
        "async_logger",
        new Poco::FormattingChannel(
            new Poco::PatternFormatter("%L%H:%M:%S.%i [%P][%T] %p %t"),
            pAsyncChannel),
        Poco::Message::PRIO_TRACE);

    Poco::Logger &logger = Poco::Logger::root();
    logger.setLevel(Poco::Message::PRIO_TRACE);
    logger.setChannel(new Poco::FormattingChannel(
        new Poco::PatternFormatter("%L%H:%M:%S.%i [%P] %t"),
        new Poco::ConsoleChannel));
    /*
        Poco::ThreadPool::defaultPool().start(massiveLogger, "worker1");
        Poco::ThreadPool::defaultPool().start(massiveLogger, "worker2");
        Poco::ThreadPool::defaultPool().start(massiveLogger, "worker3");
        Poco::ThreadPool::defaultPool().start(massiveLogger, "worker4");
        Poco::ThreadPool::defaultPool().start(massiveLogger, "worker5");
        Poco::ThreadPool::defaultPool().start(massiveLogger, "worker6");
        Poco::ThreadPool::defaultPool().start(massiveLogger, "worker7");
        Poco::ThreadPool::defaultPool().start(massiveLogger, "worker8");
        Poco::ThreadPool::defaultPool().start(massiveLogger, "worker9");
        Poco::ThreadPool::defaultPool().start(massiveLogger, "worker10");

        Poco::ThreadPool::defaultPool().joinAll();
    */
    auto p = std::make_shared<MassiveLogger>("hasoo");

    const uint32_t thMinCount = 2;
    const uint32_t thMaxCount = 10;
    Poco::ThreadPool *threadPool = new Poco::ThreadPool(thMinCount, thMaxCount);
    for (int n = 0; n < 1000; ++n) {
      logger.trace(Poco::format("used count:%d", threadPool->used()));

      if (thMaxCount == threadPool->used()) {
        Poco::Thread::sleep(1000);
        continue;
      }

      threadPool->start(*p, Poco::format("worker%d", n));
      //      try {
      //      } catch (Poco::NoThreadAvailableException &ex) {
      //        logger.critical(ex.what());
      //      }
    }

    return EXIT_OK;
  }
};

POCO_APP_MAIN(MyApp)
