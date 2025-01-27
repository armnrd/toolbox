/**
 * @file hacks.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 22/12/2024
 */

#include "hacks.hpp"

void hacks::set_display_scaling(int scaling_percentage)
{
    // Get the primary display device
    DISPLAY_DEVICE display_device = {0};
    display_device.cb = sizeof(DISPLAY_DEVICE);

    if (!EnumDisplayDevices(NULL, 0, &display_device, 0)) {
        qDebug() << "Failed to enumerate display devices.";
        return;
    }

    // Retrieve the current display settings
    DEVMODE dev_mode = {0};
    dev_mode.dmSize = sizeof(DEVMODE);

    if (!EnumDisplaySettings(display_device.DeviceName, ENUM_CURRENT_SETTINGS, &dev_mode)) {
        qDebug() << "Failed to get current display settings.";
        return;
    }

    // Adjust the scaling factor (DPI settings are often related to screen resolution scaling)
    int base_dpi = 96; // Default DPI (100%)
    int new_dpi = (scaling_percentage * base_dpi) / 100;

    dev_mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT; // Example resolution change
    dev_mode.dmPelsWidth = dev_mode.dmPelsWidth * new_dpi / base_dpi;
    dev_mode.dmPelsHeight = dev_mode.dmPelsHeight * new_dpi / base_dpi;

    // Apply the new settings
    LONG result = ChangeDisplaySettingsEx(display_device.DeviceName, &dev_mode, NULL, CDS_UPDATEREGISTRY, NULL);

    if (result != DISP_CHANGE_SUCCESSFUL) {
        qDebug() << "Failed to change display settings. Error: " << result;
    } else {
        qDebug() << "Display scaling changed successfully to " << scaling_percentage << "%";
    }
}
