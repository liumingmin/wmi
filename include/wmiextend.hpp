#pragma once

/**
  *
  * WMI extend
  * @author liumingmin (2023)
  *
 **/

#include <string>
#include <type_traits>
#include <algorithm> 
#include <cctype>
#include <locale>

#include <wmiexception.hpp>
#include <wmiresult.hpp>

namespace Wmi
{
    struct Win32_NetworkAdapter
    {
        Win32_NetworkAdapter() :
            AdapterTypeID(0),
            PhysicalAdapter(false)
        {
        }

        std::string MACAddress;
        uint32_t AdapterTypeID;
        bool PhysicalAdapter;
        std::string PNPDeviceID;

        void setProperties(const Wmi::WmiResult& result, std::size_t index)
        {
            result.extract(index, "MACAddress", (*this).MACAddress);
            result.extract(index, "AdapterTypeID", (*this).AdapterTypeID);
            result.extract(index, "PhysicalAdapter", (*this).PhysicalAdapter);
            result.extract(index, "PNPDeviceID", (*this).PNPDeviceID);
        }

        static std::string getWmiClassName()
        {
            return "Win32_NetworkAdapter";
        }
    };

    static std::string s_PciPrefix = "PCI\\";

    std::vector<std::string> GetMacAddresses(uint32_t adapterTypeID, bool onlyPCI)
    {
        std::vector<std::string> macAddresses;

        std::vector<Win32_NetworkAdapter> networkAdapters = Wmi::retrieveAllWmi<Win32_NetworkAdapter>(
            "MACAddress,AdapterTypeID,PhysicalAdapter,PNPDeviceID");
        for (auto& networkAdapter : networkAdapters)
        {
            if (!networkAdapter.PhysicalAdapter)
            {
                continue;
            }

            //avoid vmware
            bool isPci = networkAdapter.PNPDeviceID.length() >= s_PciPrefix.length() &&
                networkAdapter.PNPDeviceID.substr(0, s_PciPrefix.length()).compare(s_PciPrefix) == 0;

            if (onlyPCI && !isPci)
            {
                continue;
            }

            if (networkAdapter.MACAddress.empty())
            {
                continue;
            }

            if (networkAdapter.AdapterTypeID == adapterTypeID)
            {
                macAddresses.emplace_back(networkAdapter.MACAddress);
            }
        }
        return macAddresses;
    }

    // trim from start (in place)
    inline void ltrimstr(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    inline void rtrimstr(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    std::vector<std::string> GetDriveSerialNumbersByVolName(const std::string& volName)
    {
        std::vector<std::string> serialNumbers;

        char buffer[512] = { 0 };
        std::snprintf(buffer, sizeof(buffer), R"(Associators OF {Win32_LogicalDisk.DeviceID="%s"} WHERE AssocClass = Win32_LogicalDiskToPartition)", volName.c_str());

        Wmi::WmiResult partitions;
        Wmi::query(buffer, Wmi::CallGetWmiPath<Wmi::Win32_LogicalDisk>(0), partitions);

        for (int i = 0; i < partitions.size(); i++)
        {
            std::string deviceID;
            partitions.extract(i, "DeviceID", deviceID);

            char partBuffer[512] = { 0 };
            std::snprintf(partBuffer, sizeof(partBuffer), R"(Associators OF {Win32_DiskPartition.DeviceID="%s"} WHERE AssocClass = Win32_DiskDriveToDiskPartition)", deviceID.c_str());


            Wmi::WmiResult disks;
            Wmi::query(partBuffer, Wmi::CallGetWmiPath<Wmi::Win32_LogicalDisk>(0), disks);

            for (int j = 0; j < disks.size(); j++)
            {
                std::string diskSerialNum;

                disks.extract(j, "SerialNumber", diskSerialNum);

                ltrimstr(diskSerialNum);
                rtrimstr(diskSerialNum);

                if(!diskSerialNum.empty())
                    serialNumbers.emplace_back(std::move(diskSerialNum));
            }
        }

        return serialNumbers;
    }

    std::map<std::string, int> GetDriverSnAndMediaType()
    {
        std::map<std::string, int> snToMediaType;

        Wmi::WmiResult physicalDiskResult;
        Wmi::query("SELECT * FROM MSFT_PhysicalDisk", "microsoft\\windows\\storage", physicalDiskResult);

        for (int i = 0; i < physicalDiskResult.size(); i++)
        {
            std::string serialNumber;
            int mediaType=0;

            physicalDiskResult.extract(i, "SerialNumber", serialNumber);
            physicalDiskResult.extract(i, "MediaType", mediaType); //            0 Unspecified 3 HDD 4 SSD 5 SCM

            ltrimstr(serialNumber);
            rtrimstr(serialNumber);

            snToMediaType.emplace(serialNumber, mediaType);
        }

        return snToMediaType;
    }
}