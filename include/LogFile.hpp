// Create a log file class for now

// Xiaobin Xiong
// June 7, 2023
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "motor_driver/MotorDriver.hpp"

class LogMotorFile
{
public:
    LogMotorFile(std::string FileName, std::string FileLocation = "/ROBOTLOG/AKmotors/")
    {
        std::string home = getenv("HOME");
        std::string path_Data = home + FileLocation + FileName + "_Data";
        std::cout << " creating log file " << std::endl;
        logDataFile.open(path_Data, std::ios::out | std::ios::binary);
        // std::string path_Name = FileLocation + FileName + "_Name.text";
        // logNameFile.open(path_Name, std::ofstream::out);
    };
    void log(double data)
    {
        float data_f = static_cast<float>(data);
        logDataFile.write((char *)&(data_f), sizeof(float));
    };
    void log(int data)
    {
        float data_f = static_cast<float>(data);
        logDataFile.write((char *)&(data_f), sizeof(float));
    };

    void log(float data)
    {
        logDataFile.write((char *)&(data), sizeof(float));
    };
    void logMotor(std::map<int, motor_driver::motorState> motor_states)
    {
        for (auto const &x : motor_states)
        {       // log data consumes minimum time
            log(x.first);
            log(x.second.position);
            log(x.second.velocity);
            log(x.second.current);
            log(x.second.temperature);
            log(x.second.timeNow);
        }
    }
    /*
    void addLogNames(std::string data_name){
        names->append(data_name);
        logNameFile << names << "\n";
    };



    void logVector(std::vector<float> logVector){
        logDataFile.write(reinterpret_cast<char *>(logVector.data()), (logVector.size()) * sizeof(float));
    };
    */
    std::fstream logDataFile;
    std::fstream logNameFile;
    std::string names[6] = {"ID", "Position", "Velocity", "Current", "Temperature", "TimeNow"};

    void doneLogging()
    {
        logDataFile.close();
        // logNameFile.close();
    };
};