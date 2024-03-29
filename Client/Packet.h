#pragma once
#include <memory>
#include <iostream>
#include <fstream>

class Packet
{
    struct Header
    {
        unsigned char PlaneIDLength;
        unsigned char TimeLength;
        unsigned char FuelLength;
    } Head;

    char* PlaneID;
    char* TimeField;
    char* FuelField;

    char* TxBuffer;

public:
    // Default constructor
    Packet() : PlaneID(nullptr), TimeField(nullptr), FuelField(nullptr), TxBuffer(nullptr) {
        memset(&Head, 0, sizeof(Head));
    }

    // Function to get the plane ID, currentFuel and currentTime
    char* getPlaneID() const {
        return PlaneID;
    }
    const char* getCurrentFuel() const {
        return FuelField;
    }
    const char* getCurrentTime() const {
        return TimeField;
    }
    // Destructor to release memory
    ~Packet() {
        delete[] PlaneID;
        delete[] TimeField;
        delete[] FuelField;
    }

    void Display(std::ostream& os)
    {
        os << "Airplane ID:  " << PlaneID << std::endl;
        os << "Time:     " << TimeField << std::endl;
        os << "Remaining Fuel:     " << FuelField << std::endl <<std::endl;
    }

    // Constructor to initialize TxBuffer and deserialize raw data
    Packet(char* src)
    {
        // Deserialize header
        memcpy(&Head, src, sizeof(Header));
        int sourceSize = Head.PlaneIDLength;
        int timeSize = Head.TimeLength;
        int fuelSize = Head.FuelLength;

        // Allocate memory for source, time, and fuel fields
        PlaneID = new char[sourceSize + 1];
        TimeField = new char[timeSize + 1];
        FuelField = new char[fuelSize + 1];

        PlaneID[sourceSize] = '\0';
        TimeField[timeSize] = '\0';
        FuelField[fuelSize] = '\0';

        // Deserialize source, time, and fuel fields
        memcpy(PlaneID, src + sizeof(Header), sourceSize);
        memcpy(TimeField, src + sizeof(Header) + sourceSize, timeSize);
        memcpy(FuelField, src + sizeof(Header) + sourceSize + timeSize, fuelSize);
    }

    // Method to set data and update header
    void SetData(char* srcTime, char* srcFuel, char* srcId, int timeSize, int fuelSize, int idSize)
    {
        if (PlaneID || TimeField || FuelField)
        {
            delete[] PlaneID;
            delete[] TimeField;
            delete[] FuelField;
        }

        // Update header
        Head.PlaneIDLength = idSize;
        Head.TimeLength = timeSize;
        Head.FuelLength = fuelSize;

        // Allocate memory for source, time, and fuel fields
        PlaneID = new char[idSize + 1];
        TimeField = new char[timeSize + 1];
        FuelField = new char[fuelSize + 1];

        PlaneID[idSize] = '\0';
        TimeField[timeSize] = '\0';
        FuelField[fuelSize] = '\0';

        // Copy data from source buffers
        memcpy(PlaneID, srcId, idSize);
        memcpy(TimeField, srcTime, timeSize);
        memcpy(FuelField, srcFuel, fuelSize);
    }

    char* SerializeData(int& TotalSize)
    {
        if (TxBuffer)
            delete[] TxBuffer;

        TotalSize = sizeof(Header) + Head.PlaneIDLength + Head.TimeLength + Head.FuelLength;

        TxBuffer = new char[TotalSize];

        // Serialize header
        memcpy(TxBuffer, &Head, sizeof(Header));

        // Serialize source, time, and fuel fields
        memcpy(TxBuffer + sizeof(Header), PlaneID, Head.PlaneIDLength);
        memcpy(TxBuffer + sizeof(Header) + Head.PlaneIDLength, TimeField, Head.TimeLength);
        memcpy(TxBuffer + sizeof(Header) + Head.PlaneIDLength + Head.TimeLength, FuelField, Head.FuelLength);

        return TxBuffer;
    };

};