#pragma once
#include <memory>
#include <iostream>
#include <fstream>

class Packet
{
    struct Header
    {
        char* Source; //Airplane ID
        unsigned char TimeLength;
        unsigned char FuelLength;
    } Head;

    char* TimeField;
    char* FuelField;

    char* TxBuffer;

public:
    // Default constructor
    Packet() : TimeField(nullptr), FuelField(nullptr), TxBuffer(nullptr) { memset(&Head, 0, sizeof(Head)); Head.Source = nullptr; }

    // Function to get the plane ID, currentFuel and currentTime
    char* getPlaneID() const {
        return Head.Source;
    }
    const char* getCurrentFuel() const {
        return FuelField;
    }
    const char* getCurrentTime() const {
        return TimeField;
    }
    // Destructor to release memory
    ~Packet() {
        delete[] TimeField;
        delete[] FuelField;
        delete[] Head.Source;
    }

    void Display(std::ostream& os)
    {
        os << std::dec;
        os << "Airplane ID:  " << Head.Source << std::endl;
        os << "Time:     " << TimeField << std::endl;
        os << "Remaining Fuel:     " << FuelField << std::endl;
    }

    // Constructor to initialize TxBuffer and deserialize raw data
    Packet(char* src)
    {
        // Deserialize header
        memcpy(&Head, src, sizeof(Header));
        int timeSize = Head.TimeLength;
        int fuelSize = Head.FuelLength;

        // Allocate memory for time and fuel fields
        TimeField = new char[timeSize + 1];
        FuelField = new char[fuelSize + 1];

        TimeField[timeSize] = '\0';
        FuelField[fuelSize] = '\0';

        // Deserialize time and fuel fields
        memcpy(TimeField, src + sizeof(Header), timeSize);
        memcpy(FuelField, src + sizeof(Header) + timeSize, fuelSize);
    }

    // Method to set data and update header
    void SetData(char* srcTime, char* srcFuel, char* srcId, int timeSize, int fuelSize, int idSize)
    {
        if (TimeField || FuelField || Head.Source)
        {
            delete[] TimeField;
            delete[] FuelField;
            delete[] Head.Source;
        }

        // Update header
        Head.TimeLength = timeSize;
        Head.FuelLength = fuelSize;

        // Allocate memory for id, time and fuel fields
        Head.Source = new char[idSize + 1];
        TimeField = new char[timeSize + 1];
        FuelField = new char[fuelSize + 1];

        Head.Source[idSize] = '\0';
        TimeField[timeSize] = '\0';
        FuelField[fuelSize] = '\0';

        // Copy data from source buffers
        memcpy(Head.Source, srcId, idSize);
        memcpy(TimeField, srcTime, timeSize);
        memcpy(FuelField, srcFuel, fuelSize);
    }

    char* SerializeData(int& TotalSize)
    {
        if (TxBuffer)
            delete[] TxBuffer;

        TotalSize = sizeof(Header) + Head.TimeLength + Head.FuelLength;

        TxBuffer = new char[TotalSize];

        memcpy(TxBuffer, &Head, sizeof(Header));

        // Serialize time field
        memcpy(TxBuffer + sizeof(Header), TimeField, Head.TimeLength);

        // Serialize fuel field
        memcpy(TxBuffer + sizeof(Header) + Head.TimeLength, FuelField, Head.FuelLength);

        return TxBuffer;
    };

};