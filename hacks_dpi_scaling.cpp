/**
 * @file hacks_dpi_scaling.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 27/01/2025
 */

#include "hacks_dpi_scaling.hpp"
#include <memory>
#include <cassert>
#include <windows.h>
#include <cstringt.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

bool hacks_dpi_scaling::DPIHelper::get_paths_and_modes(std::vector<DISPLAYCONFIG_PATH_INFO>& paths, std::vector<DISPLAYCONFIG_MODE_INFO>& modes, int flags)
{
    UINT32 numPaths = 0, numModes = 0;
    auto status = GetDisplayConfigBufferSizes(flags, &numPaths, &numModes);
    if (ERROR_SUCCESS != status)
    {
        return false;
    }

    std::unique_ptr<DISPLAYCONFIG_PATH_INFO[]> paths(new DISPLAYCONFIG_PATH_INFO[numPaths]);
    std::unique_ptr<DISPLAYCONFIG_MODE_INFO[]> modes(new DISPLAYCONFIG_MODE_INFO[numModes]);
    status = QueryDisplayConfig(flags, &numPaths, paths.get(), &numModes, modes.get(), nullptr);
    if (ERROR_SUCCESS != status)
    {
        return false;
    }

    for (unsigned int i = 0; i < numPaths; i++)
    {
        paths.push_back(paths[i]);
    }

    for (unsigned int i = 0; i < numModes; i++)
    {
        modes.push_back(modes[i]);
    }

    return true;
}


hacks_dpi_scaling::DPIHelper::DPIHelper()
{
}


hacks_dpi_scaling::DPIHelper::~DPIHelper()
{
}


hacks_dpi_scaling::DPIHelper::DPIScalingInfo hacks_dpi_scaling::DPIHelper::get_dpi_scaling_info(LUID adapter_id, UINT32 source_id)
{
    DPIScalingInfo dpi_info = {};

    DPIHelper::DISPLAYCONFIG_SOURCE_DPI_SCALE_GET request_packet = {};
    request_packet.header.type = (DISPLAYCONFIG_DEVICE_INFO_TYPE)DPIHelper::DISPLAYCONFIG_DEVICE_INFO_TYPE_CUSTOM::DISPLAYCONFIG_DEVICE_INFO_GET_DPI_SCALE;
    request_packet.header.size = sizeof(request_packet);
    assert(0x20 == sizeof(request_packet));//if this fails => OS has changed something, and our reverse engineering knowledge about the API is outdated
    request_packet.header.adapterId = adapter_id;
    request_packet.header.id = source_id;

    auto res = DisplayConfigGetDeviceInfo(&request_packet.header);
    if (ERROR_SUCCESS == res)
    {//success
        if (request_packet.cur_scale_rel < request_packet.min_scale_rel)
        {
            request_packet.cur_scale_rel = request_packet.min_scale_rel;
        }
        else if (request_packet.cur_scale_rel > request_packet.max_scale_rel)
        {
            request_packet.cur_scale_rel = request_packet.max_scale_rel;
        }

        std::int32_t min_abs = abs((int)request_packet.min_scale_rel);
        if (DPIHelper::count_of(dpi_vals) >= (size_t)(min_abs + request_packet.max_scale_rel + 1))
        {//all ok
            dpi_info.current = dpi_vals[min_abs + request_packet.cur_scale_rel];
            dpi_info.recommended = dpi_vals[min_abs];
            dpi_info.maximum = dpi_vals[min_abs + request_packet.max_scale_rel];
            dpi_info.init_done = true;
        }
        else
        {
            //Error! Probably dpi_vals array is outdated
            return dpi_info;
        }
    }
    else
    {
        //DisplayConfigGetDeviceInfo() failed
        return dpi_info;
    }

    return dpi_info;
}


bool hacks_dpi_scaling::DPIHelper::set_dpi_scaling(LUID adapter_id, UINT32 source_id, UINT32 dpi_percent_to_set)
{
    DPIScalingInfo dpi_scaling_info = get_dpi_scaling_info(adapter_id, source_id);

    if (dpi_percent_to_set == dpi_scaling_info.current)
    {
        return true;
    }

    if (dpi_percent_to_set < dpi_scaling_info.mininum)
    {
        dpi_percent_to_set = dpi_scaling_info.mininum;
    }
    else if (dpi_percent_to_set > dpi_scaling_info.maximum)
    {
        dpi_percent_to_set = dpi_scaling_info.maximum;
    }

    int idx1 = -1, idx2 = -1;

    int i = 0;
    for (const auto& val : dpi_vals)
    {
        if (val == dpi_percent_to_set)
        {
            idx1 = i;
        }

        if (val == dpi_scaling_info.recommended)
        {
            idx2 = i;
        }
        i++;
    }

    if ((idx1 == -1) || (idx2 == -1))
    {
        //Error cannot find dpi value
        return false;
    }

    int dpi_relative_val = idx1 - idx2;

    DPIHelper::DISPLAYCONFIG_SOURCE_DPI_SCALE_SET set_packet = {};
    set_packet.header.adapterId = adapter_id;
    set_packet.header.id = source_id;
    set_packet.header.size = sizeof(set_packet);
    assert(0x18 == sizeof(set_packet));//if this fails => OS has changed somthing, and our reverse enginnering knowledge about the API is outdated
    set_packet.header.type = (DISPLAYCONFIG_DEVICE_INFO_TYPE) DPIHelper::DISPLAYCONFIG_DEVICE_INFO_TYPE_CUSTOM::DISPLAYCONFIG_DEVICE_INFO_SET_DPI_SCALE;
    set_packet.scale_rel = (UINT32) dpi_relative_val;

    auto res = ::DisplayConfigSetDeviceInfo(&set_packet.header);
    if (ERROR_SUCCESS == res)
    {
        return true;
    }
    else
    {
        return false;
    }
    return true;
}

/*Get default DPI scaling percentage.
The OS recommented value.
*/
int hacks_dpi_scaling::DPIHelper::get_recommended_dpi_scaling()
{
    int dpi = 0;

    auto retval = SystemParametersInfo(SPI_GETLOGICALDPIOVERRIDE, 0, (LPVOID)&dpi, 1);
    if (retval != 0)
    {
        int currDPI =  hacks_dpi_scaling::dpi_vals[dpi * -1];
        return currDPI;
    }

    return -1;
}

//to store display info along with corresponding list item
struct DisplayData {
    LUID adapter_id;
    int target_id;
    int source_id;

    DisplayData()
    {
        adapter_id = {};
        target_id = source_id = -1;
    }
};

std::vector<DisplayData> get_display_data()
{
    std::vector<DisplayData> display_data_cache;
    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;
    int flags = QDC_ONLY_ACTIVE_PATHS;
    if (false == hacks_dpi_scaling::DPIHelper::get_paths_and_modes(paths, modes, flags))
    {
        cout << "DPIHelper::get_paths_and_modes() failed\n";
    }
    display_data_cache.resize(paths.size());
    int idx = 0;
    for (const auto &path : paths)
    {
        //get display name
        auto adapter_luid = path.targetInfo.adapterId;
        auto target_id = path.targetInfo.id;
        auto source_id = path.sourceInfo.id;

        DISPLAYCONFIG_TARGET_DEVICE_NAME device_name;
        device_name.header.size = sizeof(device_name);
        device_name.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        device_name.header.adapterId = adapter_luid;
        device_name.header.id = target_id;
        if (ERROR_SUCCESS != DisplayConfigGetDeviceInfo(&device_name.header))
        {
            cout << "DisplayConfigGetDeviceInfo() failed";
        }
        else
        {
            std::wstring nameString = std::to_wstring(idx) + std::wstring(L". ") + device_name.monitorFriendlyDeviceName;
            if (DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL == device_name.outputTechnology)
            {
                nameString += L"(internal display)";
            }
            DisplayData dd = {};
            dd.adapter_id = adapter_luid;
            dd.source_id = source_id;
            dd.target_id = target_id;

            display_data_cache[idx] = dd;
        }
        idx++;
    }
    return display_data_cache;
}

bool dpi_found(int val)
{
    bool found = false;
    for (int i = 0; i < 12; i++)
    {
        if (val == hacks_dpi_scaling::dpi_vals[i])
        {
            found = true;
            break;
        }
    }
    return found;
}

int main(int argc, char *argv[])
{
    auto dpiToSet = 0;
    auto displayIndex = 1;

    enum
    {
        RESOLUTION_SET,
        RESOLUTION_GET,
        RESOLUTION_VALUE,
    } resolutionMode = RESOLUTION_SET;

    if (argc <= 1)
    {
        cout << "1. argument: Resolution in percent, use \"get\" to print the current value instead and \"value\" to print without formatting\n";
        cout << "2. argument: Monitor index, leave empty to use primary monitor\n";
        return 0;
    }

    if (argc >= 2)
    {
        if (strcmp(argv[1], "get") == 0)
        {
            resolutionMode = RESOLUTION_GET;
        }
        else if (strcmp(argv[1], "value") == 0)
        {
            resolutionMode = RESOLUTION_VALUE;
        }
        else
        {
            dpiToSet = atoi(argv[1]);
        }
    }

    if (argc >= 3)
    {
        displayIndex = atoi(argv[2]);
    }

    auto displayDataCache = get_display_data();
    if (displayIndex < 1 || displayDataCache.size() < displayIndex)
    {
        if (dpi_found(displayIndex) && 1 <= dpiToSet && dpiToSet <= displayDataCache.size())
        {
            cout << "Please provide the scale as first and the index as second argument, program will continue for legacy purposes\n";
            auto t = dpiToSet;
            dpiToSet = displayIndex;
            displayIndex = t;
        }
        else
        {
            cout << "Invalid Monitor ID: " << displayIndex;
            return 0;
        }
    }

    displayIndex -= 1; // change from 1...X to 0...(X-1)

    auto currentResolution = DpiHelper::GetDPIScalingInfo(displayDataCache[displayIndex].adapter_id, displayDataCache[displayIndex].source_id);
    if (resolutionMode == RESOLUTION_GET)
    {
        cout << "Current Resolution: " << currentResolution.current;
        return 0;
    }
    if (resolutionMode == RESOLUTION_VALUE)
    {
        cout << currentResolution.current;
        return 0;
    }
    if (!dpi_found(dpiToSet))
    {
        cout << "Invalid DPI scale value: " << dpiToSet;
        return 0;
    }
    auto success = DpiHelper::SetDPIScaling(displayDataCache[displayIndex].adapter_id, displayDataCache[displayIndex].source_id, dpiToSet);
    if (success == false)
    {
        cout << "DPIHelper::set_dpi_scaling() failed";
        return 0;
    }
    else
    {
        if (displayIndex == 0)
        {
            HKEY hKey;
            LPCWSTR sKeyPath;
            DWORD value = static_cast<DWORD>(int(dpiToSet * 0.96));
            int iResult;
            sKeyPath = L"Control Panel\\Desktop\\WindowMetrics\\";
            iResult = RegOpenKeyEx(HKEY_CURRENT_USER, sKeyPath, NULL, KEY_ALL_ACCESS, &hKey);
            iResult = RegSetValueEx(hKey, L"AppliedDPI", NULL, REG_DWORD, (const BYTE*)&value, sizeof(value));
            RegCloseKey(hKey);
            return 0;
        }
    }
}
