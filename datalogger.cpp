#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace std::chrono;

#define MAX_ITERATIONS 10000

/*
 * Simulate finalthingy's sensor data logging to profile i/o operations
 */

float tempC = 25.0;
float pressure = 1013.25;
float altitude = 0.0;
float velocity = 0.0;
float maxVelocity = 0.0;
float maxAltitude = 0.0;
float battery = 4.2;

int launchTime = 500;
int startTime = 1000;
int times[2] = {0, 0};

uint64_t millis()
{
    static auto start = steady_clock::now();
    auto now = steady_clock::now();
    return duration_cast<milliseconds>(now - start).count();
}

void updateSensors(int i)
{
    tempC = 20 + sin(i * 0.01) * 10;
    pressure = 1013.25 + sin(i * 0.01) * 5;
    altitude = max(0.0f, altitude + velocity * 0.1f);
    velocity = sin(i * 0.01) * 50 + 10;
    maxVelocity = max(maxVelocity, velocity);
    maxAltitude = max(maxAltitude, altitude);
    battery = 4.2 - (i / (float)MAX_ITERATIONS) * 1.0;
}

float readBatt()
{
    return battery;
}

string buildDataString(int i)
{
    times[1] = i;
    int t2 = (startTime == launchTime) ? 0 : (times[1] + startTime - launchTime);
    updateSensors(i);

    ostringstream ss;
    ss << times[1] << " "
       << t2 << " "
       << fixed << setprecision(2)
       << tempC << " "
       << pressure << " "
       << altitude << " "
       << velocity << " "
       << maxVelocity << " "
       << maxAltitude << " "
       << readBatt();
    return ss.str();
}

void resetSimulation()
{
    tempC = 25.0;
    pressure = 1013.25;
    altitude = 0.0;
    velocity = 0.0;
    maxVelocity = 0.0;
    maxAltitude = 0.0;
    battery = 4.2;
    times[0] = 0;
    times[1] = 0;
}

// Original: open/close per write
void log_original()
{
    auto start = millis();
    for (int i = 0; i < MAX_ITERATIONS; i++)
    {
        string data = buildDataString(i);
        ofstream file("logs/log_original.txt", ios::app);
        if (file.is_open())
        {
            file << data << "\n";
            file.close();
        }
    }
    auto end = millis();
    cout << "Original: " << (end - start) << " ms\n";
}

// Append: open -> append each line -> close
void log_append()
{
    auto start = millis();
    ofstream file("logs/log_append.txt", ios::out | ios::app);
    if (!file.is_open())
    {
        cerr << "Error opening append file\n";
        return;
    }

    for (int i = 0; i < MAX_ITERATIONS; i++)
    {
        string data = buildDataString(i);
        file << data << "\n";
    }

    file.close();
    auto end = millis();
    cout << "Appended: " << (end - start) << " ms\n";
}

// Chunk: open -> write in chunks -> close
void log_chunk()
{
    auto start = millis();

    ofstream file("logs/log_chunk.txt", ios::out | ios::binary);
    if (!file.is_open())
    {
        cerr << "Error opening optimized file\n";
        return;
    }

    const int BUFFER_SIZE = 8192;
    char buffer[BUFFER_SIZE];
    int bufIndex = 0;

    for (int i = 0; i < MAX_ITERATIONS; i++)
    {
        std::string line = buildDataString(i) + "\n";
        int len = line.length();

        if (bufIndex + len >= BUFFER_SIZE)
        {
            file.write(buffer, bufIndex);
            bufIndex = 0;
        }

        memcpy(buffer + bufIndex, line.c_str(), len);
        bufIndex += len;
    }

    if (bufIndex > 0)
    {
        file.write(buffer, bufIndex);
    }

    file.close();
    auto end = millis();
    cout << "Chunked: " << (end - start) << " ms\n";
}

int main()
{
    cout << "Simulating logging with " << MAX_ITERATIONS << " iterations...\n";

    log_original();
    resetSimulation();
    log_append();
    resetSimulation();
    log_chunk();

    return 0;
}