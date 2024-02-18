#include "machineinfo.h"
#include <wmi.hpp>
#include <wmiclasses.hpp>
#include <wmiextend.hpp>


int SSG_GetDiskType(const char* szVolName, int* pnDiskType)
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

    return 0;
}