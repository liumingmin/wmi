#pragma once

#define C_SSGMACHINE_API extern "C" __declspec(dllexport)
#define C_SSGMACHINE_CALL __stdcall


C_SSGMACHINE_API int C_SSGMACHINE_CALL SSG_GetDiskType(const char* szVolName, int* pnDiskType);
C_SSGMACHINE_API int C_SSGMACHINE_CALL SSG_GetDiskFriendlyName(const char* szVolName, char* pBuffer, int* pBufferSize);