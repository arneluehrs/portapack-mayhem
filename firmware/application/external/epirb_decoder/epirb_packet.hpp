#pragma once
#include <cstdint>
#include <string>

struct EPIRBPacket {
    bool        valid{false};
    std::string hex_id;
    std::string country_name;
    std::string beacon_type;
    std::string protocol;
    uint32_t    mmsi{0};
    float       lat{0.0f}, lon{0.0f};
    bool        has_pos{false};
    uint32_t    burst_counter{0};

    bool        has_long_frame{false};
    float       lat_long{0.0f}, lon_long{0.0f};
    uint8_t     supplementary_data[4]{};
    std::string emergency_type{"N/A"};
};