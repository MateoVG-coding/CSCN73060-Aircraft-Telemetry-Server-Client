#pragma once
#include <memory>
#include <iostream>
#include <fstream>

const int EmptyPktSize = 6; //Number of data bytes in a packet with no data field

class Packet
{
    struct Header
    {
        unsigned char Source : 4; //Airplane ID
        unsigned char TimeLength;
        unsigned char FuelLength;
    } Head;

    char* TimeField; 
    char* FuelField;

    char* TxBuffer;

public:
    // Default constructor
    Packet() : TimeField(nullptr), FuelField(nullptr), TxBuffer(nullptr) { memset(&Head, 0, sizeof(Head)); Head.Source = 2; }

    // Function to get the plane ID, TimeField, FuelField and elapsedTime
    unsigned char getPlaneID() const {
        return Head.Source;
    }
    const char* getTimeField() const {
        return TimeField;
    }
    const char* getFuelField() const {
        return FuelField;
    }
    // Destructor to release memory
    ~Packet() {
        delete[] TimeField;
        delete[] FuelField;
    }

    void Display(std::ostream& os)
    {
        os << std::dec;
        os << "Source:  " << (int)Head.Source << std::endl;
        os << "Length Time:  " << (int)Head.TimeLength << std::endl;
        os << "Time:     " << TimeField << std::endl;
        os << "Length Fuel:  " << (int)Head.FuelLength << std::endl;
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
    void SetData(char* srcTime, char* srcFuel, int timeSize, int fuelSize) 
    {
        if (TimeField || FuelField)
        {
            delete[] TimeField;
            delete[] FuelField;
        }

        // Update header
        Head.TimeLength = timeSize;
        Head.FuelLength = fuelSize;

        // Allocate memory for time and fuel fields
        TimeField = new char[timeSize + 1]; 
        FuelField = new char[fuelSize + 1]; 

        TimeField[timeSize] = '\0';
        FuelField[fuelSize] = '\0';

        // Copy data from source buffers
        memcpy(TimeField, srcTime, timeSize);
        memcpy(FuelField, srcFuel, fuelSize);
    }

    char* SerializeData(int& TotalSize)
    {
        if (TxBuffer)
            delete[] TxBuffer;

        TotalSize = EmptyPktSize + Head.TimeLength + Head.FuelLength;

        TxBuffer = new char[TotalSize];

        memcpy(TxBuffer, &Head, sizeof(Header));

        // Serialize time field
        memcpy(TxBuffer + sizeof(Header), TimeField, Head.TimeLength);

        // Serialize fuel field
        memcpy(TxBuffer + sizeof(Header) + Head.TimeLength, FuelField, Head.FuelLength);

        return TxBuffer;
    };

};
