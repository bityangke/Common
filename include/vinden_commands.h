#ifndef _VINDEN_COMMANDS_H_
#define _VINDEN_COMMANDS_H_

#include <cstdint>

#include <wind_protocol.h>

//-----------------------------------------------------------------------------
enum system_command {
    ERROR_RESPONSE =    0x00,
    
    GET_CAMERA_VER =    0x01,
    
    END_CAMERA_CTRL =   0x02,    
};

enum lens_command {
    
    LENS_VERSION =      0x41,
    LENS_READY =        0x42,
    
    SET_ZOOM_INDEX =    0x45,
    GET_ZOOM_INDEX =    0x46,
    
    GET_ZOOM_MTR_POS =  0x47,
    
    SET_FOCUS_POS =     0x48,
    GET_FOCUS_POS =     0x49,
    SET_INF_FOCUS =     0x4A,
    
    START_AUTOFOCUS =   0x4C,
    STOP_AUTOFOCUS =    0x4D,
    
    SET_ZOOM_SPEED =    0x4E,
    GET_ZOOM_SPEED =    0x4F,
    
    SET_FOCUS_SPEED =   0x50,
    GET_FOCUS_SPEED =   0x51,
    
    RUN_CONT_ZOOM =     0x52,
    RUN_CONT_FOCUS =    0x53,
    
    SET_LENS_BLUR =     0x54
};

enum sensor_command {
    SENSOR_VERSION =    0x81,
    
    FLAT_FIELD_CORR =   0x82,
    
    LOAD_NUC_TABLE =    0x83,
    GET_NUC_TABLE =     0x84,
    
    SET_AUTO_FFC_PER =  0x85,
    GET_AUOT_FFC_PER =  0x86,
    
    SET_AUTO_FFC_MODE = 0x87,
    GET_AUTO_FFC_MODE = 0x88,
    
    SET_SHUTTER =       0x89,
    
    GET_SENSOR_READY =  0x8A,
       
};

//-----------------------------------------------------------------------------
std::vector<std::string> vinden_error_code = {
    "None",
    "Wrong command ID",
    "Wrond data size",
    "Argument out of range",
    "Wrong checksum",
    "Receive buffer full",
    "Communication timeout",
    "Boot up error",
    "Error while writing",
    "Error while reading",
    "Lens serial 1 commincation issue",
    "Sensor serial 2 commincation issue",
    "Command not implemented",
    "Telemtry error",
    "Undefined camera model",
    "Autofocus running"
};

//-----------------------------------------------------------------------------
class lens{
    
    uint16_t fw_maj_rev;
    uint16_t fw_min_rev;
    uint16_t sw_maj_rev;
    uint16_t sw_min_rev;
    
    uint16_t zoom_index;
    uint16_t zoom_position;
    uint8_t zoom_speed;
    
    uint16_t focus_position;
    uint8_t focus_speed;
    
    //-----------------------------------------------------------------------------
    lens() = default;
    
    lens(uint16_t fw_maj, uint16_t fw_min, uint16_t sw_maj, uint16_t fw_min)
    {
        fw_maj_rev = fw_maj;
        fw_min_rev = fw_min;
        sw_maj_rev = sw_maj;
        sw_min_rev = sw_min;
    }        
    
    //-----------------------------------------------------------------------------
    fip_protocol get_version(void)
    {
        wind_protocol lens_packet(LENS_VERSION);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }
    
    //-----------------------------------------------------------------------------
    fip_protocol lens_ready(uint8_t &value)
    {
        wind_protocol lens_packet(LENS_READY);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol set_zoom_index(uint16_t value)
    {
        wind_protocol lens_packet(SET_ZOOM_INDEX);
        lens_packet.update_payload(value);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol get_zoom_index(uint16_t &value)
    {
        wind_protocol lens_packet(GET_ZOOM_INDEX);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        

    }

    //-----------------------------------------------------------------------------
    fip_protocol get_zoom_index(uint16_t &value)
    {
        wind_protocol lens_packet(GET_ZOOM_INDEX);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol get_zoom_mtr_position(uint16_t &value)
    {
        wind_protocol lens_packet(GET_ZOOM_MTR_POS);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }
    
        //-----------------------------------------------------------------------------
    fip_protocol set_focus_position(uint16_t value)
    {
        wind_protocol lens_packet(SET_FOCUS_POS);
        lens_packet.update_payload(value);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol get_focus_position(uint16_t &value)
    {
        wind_protocol lens_packet(GET_FOCUS_POS);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol set_infinity_focus(void)
    {
        wind_protocol lens_packet(SET_INF_FOCUS);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol start_autofocus(uint8_t value)
    {
        wind_protocol lens_packet(START_AUTOFOCUS);
        lens_packet.update_payload(value);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol stop_autofocus(void)
    {
        wind_protocol lens_packet(STOP_AUTOFOCUS);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol set_zoom_speed(uint8_t value)
    {
        wind_protocol lens_packet(GET_ZOOM_INDEX);
        lens_packet.update_payload(value);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol get_zoom_speed(uint8_t &value)
    {
        wind_protocol lens_packet(GET_ZOOM_INDEX);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol set_focus_speed(uint8_t value)
    {
        wind_protocol lens_packet(GET_ZOOM_INDEX);
        lens_packet.update_payload(value);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol get_focus_speed(uint8_t &value)
    {
        wind_protocol lens_packet(GET_ZOOM_INDEX);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol run_continuous_zoom(uint8_t value)
    {
        wind_protocol lens_packet(RUN_CONT_ZOOM);
        lens_packet.update_payload(value);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol run_continuous_focus(uint8_t value)
    {
        wind_protocol lens_packet(RUN_CONT_FOCUS);
        lens_packet.update_payload(value);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol set_lens_blur(uint8_t value)
    {
        wind_protocol lens_packet(SET_LENS_BLUR);
        lens_packet.update_payload(value);

        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());        
        
    }
    
    //-----------------------------------------------------------------------------
    inline friend std::ostream& operator<< (
        std::ostream& out,
        const vinden_lens& item
    )
    {
        out << "  Firmware Version: " << (uint32_t)item.fw_maj_rev << "." << item.fw_min_rev << std::endl;
        out << "  Software Version: " << (uint32_t)item.sw_maj_rev << "." << item.sw_min_rev << std::endl;
        return out;
    }
    
};  // end lens class


//-----------------------------------------------------------------------------
class sensor{
    
    uint16_t fw_maj_rev;
    uint16_t fw_min_rev;
    uint16_t sw_maj_rev;
    uint16_t sw_min_rev;
    
    uint16_t ffc_period;
    uint8_t ffc_mode;
    
    
    sensor() = default;
    
    sensor(uint16_t fw_maj, uint16_t fw_min, uint16_t sw_maj, uint16_t fw_min)
    {
        fw_maj_rev = fw_maj;
        fw_min_rev = fw_min;
        sw_maj_rev = sw_maj;
        sw_min_rev = sw_min;
    }        
    
    //-----------------------------------------------------------------------------
    fip_protocol get_version(void)
    {
        wind_protocol lens_packet(SENSOR_VERSION);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol perform_ffc(uint8_t value)
    {
        wind_protocol lens_packet(FLAT_FIELD_CORR);
        lens_packet.update_payload(value);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol set_auto_ffc_period(uint8_t value)
    {
        wind_protocol lens_packet(SET_AUTO_FFC_PER);
        lens_packet.update_payload(value);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol get_auto_ffc_period(uint8_t &value)
    {
        wind_protocol lens_packet(GET_AUOT_FFC_PER);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol set_auto_ffc_mode(uint8_t value)
    {
        wind_protocol lens_packet(SET_AUTO_FFC_MODE);
        lens_packet.update_payload(value);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol get_auto_ffc_mode(uint8_t &value)
    {
        wind_protocol lens_packet(LENS_VERSION);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }    
    
    //-----------------------------------------------------------------------------
    fip_protocol set_shutter(uint8_t value)
    {
        wind_protocol lens_packet(SET_SHUTTER);
        lens_packet.update_payload(value);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol get_sensor_ready(uint8_t &value)
    {
        wind_protocol lens_packet(GET_SENSOR_READY);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }

    //-----------------------------------------------------------------------------
    fip_protocol load_nuc_table(uint8_t value)
    {
        wind_protocol lens_packet(LOAD_NUC_TABLE);
        lens_packet.update_payload(value);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }    

    //-----------------------------------------------------------------------------
    fip_protocol get_nuc_table(uint8_t &value)
    {
        wind_protocol lens_packet(GET_NUC_TABLE);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }

    //-----------------------------------------------------------------------------    
    inline friend std::ostream& operator<< (
        std::ostream& out,
        const sensor& item
    )
    {
        out << "  Firmware Version: " << (uint32_t)item.fw_maj_rev << "." << item.fw_min_rev << std::endl;
        out << "  Software Version: " << (uint32_t)item.sw_maj_rev << "." << item.sw_min_rev << std::endl;
        return out;
    }
    
};  // end sensor class

//-----------------------------------------------------------------------------


class system{

    uint8_t maj_rev;
    uint8_t min_rev;
    uint16_t build_num;
    uint16_t camera_type;

    //-----------------------------------------------------------------------------
    fip_protocol get_version(void)
    {
        wind_protocol lens_packet(GET_CAMERA_VER);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }
    
    //-----------------------------------------------------------------------------
    fip_protocol end_cam_control(void)
    {
        wind_protocol lens_packet(END_CAMERA_CTRL);
        
        // build a fip packet
        return fip_packet(0x3D, lens_packet.to_array());
        
    }    

};  // end system class


#endif  // _VINDEN_COMMANDS_H_
