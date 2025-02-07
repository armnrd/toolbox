/**
 * @file hacks.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 22/12/2024
 */

#include "hacks.hpp"
#include "hacks_dpi_scaling.hpp"

void hacks::set_display_scaling(int scaling_percentage, int display_index)
{
    auto display_data_cache = hacks_dpi_scaling::get_display_data();

    auto current_resolution = hacks_dpi_scaling::get_dpi_scaling_info(display_data_cache[display_index].adapter_id, display_data_cache[display_index].source_id);

    auto success = hacks_dpi_scaling::set_dpi_scaling(display_data_cache[display_index].adapter_id, display_data_cache[display_index].source_id, scaling_percentage);

    if (success == false)
    {
        qDebug() << "set_dpi_scaling() failed";
        return;
    }
    else
    {
        if (display_index == 0)
        {
            HKEY hKey;
            LPCWSTR sKeyPath;
            DWORD value = static_cast<DWORD>(int(scaling_percentage * 0.96));
            int iResult;
            sKeyPath = L"Control Panel\\Desktop\\WindowMetrics\\";
            iResult = RegOpenKeyEx(HKEY_CURRENT_USER, sKeyPath, NULL, KEY_ALL_ACCESS, &hKey);
            iResult = RegSetValueEx(hKey, L"AppliedDPI", NULL, REG_DWORD, (const BYTE*)&value, sizeof(value));
            RegCloseKey(hKey);
        }
    }
}
