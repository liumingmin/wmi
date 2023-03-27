#pragma once

/**
  *
  * WMI extend
  * @author liumingmin (2023)
  *
 **/

#include <string>
#include <type_traits>

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

                serialNumbers.emplace_back(std::move(diskSerialNum));
            }
        }

        return serialNumbers;
    }
}