#include "machineinfo.h"
#include <wmi.hpp>
#include <wmiclasses.hpp>
#include <wmiextend.hpp>


int SSG_GetDiskType(const char* szVolName, int* pnDiskType)
{
    try
    {
        auto sns = Wmi::GetDriveSerialNumbersByVolName(szVolName);
        if(sns.empty())
        {
            return 2;
        }

        auto snToMediaType = Wmi::GetDriverSnAndMediaType();
        auto iter = snToMediaType.find(sns[0]);
        if (iter == snToMediaType.end())
        {
            return 3;
        }

        *pnDiskType = iter->second;
    }catch(const std::exception&)
    {
        return 4;
    }
    return 0;
}

int SSG_GetDiskFriendlyName(const char* szVolName, char* pBuffer, int* pBufferSize)
{
    try
    {
        auto sns = Wmi::GetDriveSerialNumbersByVolName(szVolName);
        if (sns.empty())
        {
            return 2;
        }

        auto snToFriendlyName = Wmi::GetDriverSnAndFriendlyName();
        auto iter = snToFriendlyName.find(sns[0]);
        if (iter == snToFriendlyName.end())
        {
            return 3;
        }

        std::string friendlyName = iter->second;
        int copySize = friendlyName.size();
        if (copySize == 0) {
            *pBufferSize = 0;
            return 0;
        }

        int buffSize = int(*pBufferSize);
        if (copySize > buffSize){
            copySize = buffSize;
        }

        memcpy(pBuffer, friendlyName.c_str(), copySize);
        *pBufferSize = copySize;
        return 0;
    }
    catch (const std::exception&)
    {
        return 4;
    }
    return 0;
}