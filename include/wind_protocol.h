#ifndef _WIND_PROTOCOL_H_
#define _WIND_PROTOCOL_H_


#include <cstdint>


//-----------------------------------------------------------------------------
class wind_protocol
{

public:
    uint8_t id;                     // command id for the packet
    uint8_t size;                   // number of bytes in the payload
    
    std::vector<uint8_t> payload;
    
    wind_protocol() = default;
    
    wind_protocol(uint8_t id_) : id(id_)
    {
        size = 0;
        payload.clear();
        calc_checksum();
    }
    
    wind_protocol(uint8_t id_, std::vector<uint8_t> data) : id(id_)
    {
        payload = data;
        size = payload.size();
        calc_checksum();
    }
    
    //-----------------------------------------------------------------------------
    inline void update_payload(uint16_t value)
    {
        payload.push_back((uint8_t)(value & 0x00FF));
        payload.push_back((uint8_t)((value >> 8) & 0x00FF));
        
        size = payload.size();
        calc_checksum();        
    }
    
    inline void update_payload(uint32_t value)
    {
        payload.push_back((uint8_t)(value & 0x00FF));
        payload.push_back((uint8_t)((value >> 8) & 0x00FF));
        payload.push_back((uint8_t)((value >> 16) & 0x00FF));
        payload.push_back((uint8_t)((value >> 24) & 0x00FF));
        
        size = payload.size();
        calc_checksum();
    }    
    
    //-----------------------------------------------------------------------------
    std::vector<uint8_t> to_array(void)
    {
        std::vector<uint8_t> d = {header, id, size};

        d.insert(d.end(), payload.begin(), payload.end());

        d.push_back(header);
        
        return d;
    }

private:

    uint8_t header = 0x70;
    uint8_t checksum;
    
    //-----------------------------------------------------------------------------
    uint8_t calc_checksum(void);
    {
        uint8_t checksum = header;
        
        checksum += id;
        checksum += size;
        
        for(uint32_t idx = 0; idx < payload.size(); ++idx)
        {
            checksum += payload[idx];
        }
        
        return ~checksum;
    }
    
    //-----------------------------------------------------------------------------
    bool valid_checksum(void)
    {
        return (checksum == calc_checksum());
    }

};  // end of wind_protocol class

#endif  // _WIND_PROTOCOL_H_
