/**
 * @file hacks_dpi_scaling.hpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 27/01/2025
 */

#ifndef TOOLBOX_HACKS_DPI_SCALING_HPP
#define TOOLBOX_HACKS_DPI_SCALING_HPP

#include <windows.h>
#include <vector>

namespace hacks_dpi_scaling
{

/*
* OS reports DPI scaling values in relative terms, and not absolute terms.
* eg. if current DPI value is 250%, and recommended value is 200%, then
* OS will give us integer 2 for DPI scaling value (starting from recommended
* DPI scaling move 2 steps to the right in this list).
* values observed (and extrapolated) from system settings app (immersive control panel).
*/
static const UINT32 dpi_vals[] = {100, 125, 150, 175, 200, 225, 250, 300, 350, 400, 450, 500 };

class DPIHelper
{

public:
    template<class T, size_t sz>
    static size_t count_of(const T (&arr)[sz])
    {
        UNREFERENCED_PARAMETER(arr);
        return sz;
    }

    /*
    * @brief : Use QueryDisplayConfig() to get paths, and modes.
    * @param[out] paths : reference to a vector which will contain paths
    * @param[out] modes : reference to a vector which will contain modes
    * @param[in] flags : determines the kind of paths to retrieve (only active paths by default)
    * return : false in case of failure, else true
    */
    static bool get_paths_and_modes(std::vector<DISPLAYCONFIG_PATH_INFO>& paths, std::vector<DISPLAYCONFIG_MODE_INFO>& modes, int flags = QDC_ONLY_ACTIVE_PATHS);

    //out own enum, similar to DISPLAYCONFIG_DEVICE_INFO_TYPE enum in wingdi.h
    enum class DISPLAYCONFIG_DEVICE_INFO_TYPE_CUSTOM : int
    {
        DISPLAYCONFIG_DEVICE_INFO_GET_DPI_SCALE = -3, //returns min, max, suggested, and currently applied DPI scaling values.
        DISPLAYCONFIG_DEVICE_INFO_SET_DPI_SCALE = -4, //set current dpi scaling value for a display
    };

    /*
    * struct DISPLAYCONFIG_SOURCE_DPI_SCALE_GET
    * @brief used to fetch min, max, suggested, and currently applied DPI scaling values.
    * All values are relative to the recommended DPI scaling value
    * Note that DPI scaling is a property of the source, and not of target.
    */
    struct DISPLAYCONFIG_SOURCE_DPI_SCALE_GET
    {
        DISPLAYCONFIG_DEVICE_INFO_HEADER            header;
        /*
        * @brief min value of DPI scaling is always 100, min_scale_rel gives no. of steps down from recommended scaling
        * eg. if min_scale_rel is -3 => 100 is 3 steps down from recommended scaling => recommended scaling is 175%
        */
        std::int32_t min_scale_rel;

        /*
        * @brief currently applied DPI scaling value wrt the recommended value. eg. if recommended value is 175%,
        * => if cur_scale_rel == 0 the current scaling is 175%, if cur_scale_rel == -1, then current scale is 150%
        */
        std::int32_t cur_scale_rel;

        /*
        * @brief maximum supported DPI scaling wrt recommended value
        */
        std::int32_t max_scale_rel;
    };

    /*
    * struct DISPLAYCONFIG_SOURCE_DPI_SCALE_SET
    * @brief set DPI scaling value of a source
    * Note that DPI scaling is a property of the source, and not of target.
    */
    struct DISPLAYCONFIG_SOURCE_DPI_SCALE_SET
    {
        DISPLAYCONFIG_DEVICE_INFO_HEADER            header;
        /*
        * @brief The value we want to set. The value should be relative to the recommended DPI scaling value of source.
        * eg. if scale_rel == 1, and recommended value is 175% => we are trying to set 200% scaling for the source
        */
        int32_t scale_rel;
    };

    /*
    * struct DPIScalingInfo
    * @brief DPI info about a source
    * mininum :     minumum DPI scaling in terms of percentage supported by source. Will always be 100%.
    * maximum :     maximum DPI scaling in terms of percentage supported by source. eg. 100%, 150%, etc.
    * current :     currently applied DPI scaling value
    * recommended : DPI scaling value reommended by OS. OS takes resolution, physical size, and expected viewing distance
    *               into account while calculating this, however exact formula is not known, hence must be retrieved from OS
    *               For a system in which user has not explicitly changed DPI, current should eqaul recommended.
    * init_done :   If true, it means that the members of the struct contain values, as fetched from OS, and not the default
    *               ones given while object creation.
    */
    struct DPIScalingInfo
    {
        UINT32 mininum = 100;
        UINT32 maximum = 100;
        UINT32 current = 100;
        UINT32 recommended = 100;
        bool init_done = false;
    };

    DPIHelper();
    ~DPIHelper();
    static DPIHelper::DPIScalingInfo get_dpi_scaling_info(LUID adapter_id, UINT32 source_id);
    static int get_recommended_dpi_scaling();
    static bool set_dpi_scaling(LUID adapter_id, UINT32 source_id, UINT32 dpi_percent_to_set);
};


}

#endif //TOOLBOX_HACKS_DPI_SCALING_HPP
