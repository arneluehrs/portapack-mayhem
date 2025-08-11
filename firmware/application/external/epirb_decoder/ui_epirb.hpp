#pragma once
#include "ui.hpp"
#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "receiver_model.hpp"
#include "baseband_api.hpp"
#include "epirb_packet.hpp"
#include "freqman.hpp"
#include "string_format.hpp"

namespace ui::external_apps::epirb_decoder {

class EPIRBRxView : public View {
public:
    explicit EPIRBRxView(NavigationView& nav) : nav_{nav} {
        freq_options.emplace_back(406025000, "406.025 MHz");
        freq_options.emplace_back(406028000, "406.028 MHz");
        freq_options.emplace_back(406037000, "406.037 MHz");
        freq_options.emplace_back(144875000, "144.875 MHz");

        add_children({
            &labels,
            &console,
            &field_freq,
            &button_done
        });

        receiver_model.set_target_frequency(freq_options[0].frequency);
        receiver_model.set_modulation(ReceiverModel::Mode::NarrowbandFM);
        receiver_model.set_nbfm_configuration(10);
        receiver_model.enable();

        baseband::set_epirb_decoder();
        baseband::set_packet_handler([this](const EPIRBPacket& p) {
            handle_packet(p);
        });

        field_freq.on_change = [this](size_t, const freqman::Entry& e) {
            receiver_model.set_target_frequency(e.frequency);
        };
    }

    void focus() override { field_freq.focus(); }

private:
    NavigationView& nav_;

    std::vector<freqman::Entry> freq_options;

    Labels labels{
        { { 1 * 8, 1 * 16 }, "EPIRB / 144.875 MHz Decoder", Color::green() }
    };

    Console console{
        { 0, 5 * 16, 30 * 8, 16 * 16 }
    };

    FrequencyField field_freq{
        { 1 * 8, 3 * 16 },
        freq_options
    };

    Button button_done{
        { 21 * 8, 22 * 16, 9 * 8, 32 },
        "Done",
        [this] { nav_.pop(); }
    };

    void handle_packet(const EPIRBPacket& p) {
        if (!p.valid) return;

        console.writeln("--- EPIRB Burst ---");
        console.writeln("Hex ID : " + p.hex_id);
        console.writeln("Country: " + p.country_name);
        console.writeln("Type   : " + p.beacon_type);
        console.writeln("Proto  : " + p.protocol);
        if (p.mmsi) console.writeln("MMSI   : " + to_string_dec_uint(p.mmsi));
        if (p.has_pos)
            console.writeln("Pos    : " + to_string_float(p.lat, 5) + ", " +
                            to_string_float(p.lon, 5));
        if (p.has_long_frame) {
            console.writeln("LongLat: " + to_string_float(p.lat_long, 5) + ", " +
                            to_string_float(p.lon_long, 5));
            console.writeln("Supp   : " + to_hex_string(p.supplementary_data, 4));
            console.writeln("Nature : " + p.emergency_type);
        }
        console.writeln("");
    }

    static std::string to_hex_string(const uint8_t* d, size_t n) {
        std::string s;
        for (size_t i = 0; i < n; ++i) {
            s += to_string_hex(d[i], 2);
            if (i + 1 < n) s += ' ';
        }
        return s;
    }
};

}  // namespace ui::external_apps::epirb_decoder