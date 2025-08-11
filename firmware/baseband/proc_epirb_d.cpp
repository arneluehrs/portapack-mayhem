#include "proc_epirb.hpp"
#include "epirb_packet.hpp"
#include "crc.hpp"
#include <cstring>

using namespace dsp;

static const char* country_name(uint16_t cc) {
    // trivial LUT – expand as needed
    switch (cc) {
        case 232: return "Netherlands";
        case 244: return "Finland";
        case 247: return "Italy";
        case 250: return "United Kingdom";
        case 366: return "USA";
        default:  return "Unknown";
    }
}

static const char* emergency_type_str(uint8_t code) {
    switch (code & 0x0F) {
        case 0:  return "Fire / explosion";
        case 1:  return "Flooding";
        case 2:  return "Collision";
        case 3:  return "Grounding";
        case 4:  return "Listing / capsizing";
        case 5:  return "Sinking";
        case 6:  return "Disabled / adrift";
        case 7:  return "Undesignated distress";
        case 8:  return "Abandoning ship";
        case 9:  return "Piracy / boarding";
        case 10: return "Man overboard";
        case 11: return "Medical assistance";
        case 12: return "Technical assistance";
        case 13: return "Reserved (13)";
        case 14: return "Reserved (14)";
        default: return "Test / no distress";
    }
}

static bool crc_ok(const uint8_t* b) {
    // 12-bit CRC – placeholder, returns true for demo
    return true;
}

static EPIRBPacket decode_beacon(const uint8_t* b, bool is_second) {
    EPIRBPacket p;
    uint64_t frame = 0;
    for (int i = 0; i < 7; ++i)
        frame = (frame << 8) | b[i];

    if (!is_second) {
        p.valid = crc_ok(b);
        if (!p.valid) return p;

        uint32_t id = (frame >> 26) & 0x1FFFFFF;
        p.hex_id = to_string_hex(id, 7);

        uint16_t cc = (frame >> 36) & 0x3FF;
        p.country_name = country_name(cc);

        uint8_t proto = (frame >> 37) & 0x03;
        switch (proto) {
            case 0: p.protocol = "Serial"; break;
            case 1: p.protocol = "MMSI"; break;
            case 2: p.protocol = "Test"; break;
            default: p.protocol = "RLS"; break;
        }

        uint8_t type = (frame >> 40) & 0x07;
        switch (type) {
            case 0: p.beacon_type = "EPIRB"; break;
            case 1: p.beacon_type = "ELT"; break;
            case 2: p.beacon_type = "PLB"; break;
            default: p.beacon_type = "?"; break;
        }

        if ((frame >> 46) & 1) {
            int32_t lat_raw = ((frame >> 47) & 0x7FFFF) * 2;
            int32_t lon_raw = ((frame >> 66) & 0x7FFFF) * 2;
            p.lat  = (lat_raw - 90000) / 600.0f;
            p.lon  = (lon_raw - 180000) / 600.0f;
            p.has_pos = true;
        }
    }
    return p;
}

class EPIRBDecoder : public dsp::Processor {
public:
    void execute(const buffer_c8_t& buffer) override {
        for (size_t i = 0; i < buffer.count; ++i) {
            int16_t s = buffer.p[i].real() * buffer.p[i].real() +
                        buffer.p[i].imag() * buffer.p[i].imag();
            feed(s);
        }
    }

private:
    enum State { WAIT_FIRST, WAIT_SECOND };
    State state_ = WAIT_FIRST;
    EPIRBPacket first_;
    uint32_t counter_ = 0;

    dsp::tone_detector<1100, 160> tone_detector{};

    void feed(int16_t s) {
        tone_detector.update(s);
        if (!tone_detector.detected()) return;

        uint8_t raw[14];
        if (!read_bits(raw)) return;

        switch (state_) {
        case WAIT_FIRST:
            first_ = decode_beacon(raw, false);
            if (!first_.valid) break;
            if (raw[13] & 0x01) {           // long-message flag
                counter_ = 0;
                state_ = WAIT_SECOND;
                return;
            } else {
                send(first_);
            }
            break;

        case WAIT_SECOND:
            if (++counter_ > 30) {          // timeout
                state_ = WAIT_FIRST;
                first_.has_long_frame = false;
                send(first_);
                return;
            }
            if ((raw[13] & 0x01) == 0) {   // second frame
                uint32_t payload = 0;
                for (size_t i = 1; i < 5; ++i)
                    payload = (payload << 8) | raw[i];

                int32_t lat_raw = (payload >> 20) & 0xFFFFF;
                int32_t lon_raw = (payload >> 0)  & 0xFFFFF;
                first_.lat_long = (lat_raw - 524288) / 600.0f;
                first_.lon_long = (lon_raw - 1048576) / 600.0f;

                std::memcpy(first_.supplementary_data, raw + 5, 4);

                uint8_t et = (raw[0] >> 3) & 0x0F;
                first_.emergency_type = emergency_type_str(et);

                first_.has_long_frame = true;
                send(first_);
                state_ = WAIT_FIRST;
            }
            break;
        }
    }

    bool read_bits(uint8_t* dst) {
        // placeholder: returns 112 bits from soft demod
        // simplified for readability
        for (int i = 0; i < 14; ++i) dst[i] = 0;   // TODO real bit slicer
        return true;
    }

    void send(const EPIRBPacket& p) {
        shared_memory.application_queue.push(p);
    }
};

void set_epirb_decoder() {
    baseband::set_processor(std::make_unique<EPIRBDecoder>());
}