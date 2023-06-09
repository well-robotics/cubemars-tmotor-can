#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

#include "motor_driver/MotorDriver.hpp"
#include "LogFile.hpp"

using namespace std;

using namespace std::chrono;

// Get time stamp in microseconds.
uint64_t get_time_in_microseconds()
{
  uint64_t us =
      std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::high_resolution_clock::now().time_since_epoch())
          .count();
  return us;
}

// Print Motor State
void printMotorStates(std::map<int, motor_driver::motorState> motor_states)
{
  for (auto const &x : motor_states)
  {
    std::cout << "Motor ID: " << x.first
              << " Position: " << x.second.position
              << " Velocity: " << x.second.velocity
              << " Current: " << x.second.current
              << " Temperature: " << x.second.temperature
              << " ErrorCode: " << x.second.error_code
              << std::endl;
  }
}

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    cout
        << "Example Usage: ./MotorDriverTest can0 <motor_id_1> <motor_id_2> ..."
        << endl;
    return 0;
  }
  vector<int> motor_ids;

  LogMotorFile motorLog("testing");
 
  char *can_interface(argv[1]);

  assert(strncmp(can_interface, "can", 3) == 0 &&
         "First argument must be a can interface starting with 'can'. Example: can0 or can1");

  for (int i = 2; i < argc; i++)
  {
    motor_ids.push_back(atoi(argv[i]));
  }
  for (auto ids : motor_ids)
  {
    cout << "IDS: " << ids << endl;
  }

  motor_driver::MotorDriver motor_controller(
      motor_ids, can_interface, motor_driver::MotorType::AK60_6_V2p2);

  cout << "Enabling Motor..." << endl;
  auto start_state = motor_controller.enableMotor(motor_ids);
  printMotorStates(start_state);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  cout << "Setting Zero Position..." << endl;
  auto stateZero = motor_controller.setZeroPosition(motor_ids);
  printMotorStates(stateZero);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  int i;
  cout << "Enter any number to move motors..." << endl;
  cin >> i;

  //  Iterate over motor ids to create command map for 90 deg rotation and send
  //  command
  motor_driver::motorCommand moveCommandStruct = {0, 1.57, 0, 1, 0};
  std::map<int, motor_driver::motorCommand> commandMap;
  for (int i = 0; i < motor_ids.size(); i++)
  {
    commandMap[motor_ids[i]] = moveCommandStruct;
  }

  cout << "Moving Motor..." << endl;
  // auto startT = get_time_in_microseconds();
  auto commandState = motor_controller.sendRadCommand(commandMap);

  // execute the command of certain time
  // std::this_thread::sleep_for(std::chrono::milliseconds(10000));
  auto start = std::chrono::steady_clock::now();
  while (true)
  {
    commandState = motor_controller.sendRadCommand(commandMap);
    motorLog.logMotor(commandState);
    if (std::chrono::steady_clock::now() - start > std::chrono::seconds(5))
      break;
  }

  motor_driver::motorCommand stopCommandStruct = {0, 0, 0, 0, 0};
  std::map<int, motor_driver::motorCommand> stopCommandMap;
  for (int i = 0; i < motor_ids.size(); i++)
  {
    stopCommandMap[motor_ids[i]] = stopCommandStruct;
  }
  commandState = motor_controller.sendRadCommand(stopCommandMap);

  // auto endT = get_time_in_microseconds();
  // auto dt = (endT - startT);
  // std::cout << "Time Taken for Command: " << double(dt / 1e6) << std::endl;

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  cout << "Disabling Motor..." << endl;
  auto end_state = motor_controller.disableMotor(motor_ids);
  printMotorStates(end_state);

  motorLog.doneLogging(); 
  return 0;
}
