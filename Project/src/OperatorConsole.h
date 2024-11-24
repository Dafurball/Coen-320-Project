#ifndef SRC_OPERATORCONSOLE_H_
#define SRC_OPERATORCONSOLE_H_

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <sys/dispatch.h>

class OperatorConsole {
public:
    OperatorConsole();
    virtual ~OperatorConsole();

    void startOperatorConsoleThread();
    void stopOperatorConsoleThread();

    void sendCommand(int aircraftID, const std::string& command);

private:
    std::thread input_thread;         // Thread for user input
    std::atomic<bool> running;       // Flag to control thread execution

    void inputLoop();                // Input handling loop
};

#endif /* SRC_OPERATORCONSOLE_H_ */
