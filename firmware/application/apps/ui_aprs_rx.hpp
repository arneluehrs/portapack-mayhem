/*
 * Copyright (C) 2014 Jared Boone, ShareBrained Technology, Inc.
 * Copyright (C) 2017 Furrtek
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __UI_APRS_RX_H__
#define __UI_APRS_RX_H__

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "ui_receiver.hpp"
#include "ui_freq_field.hpp"
#include "ui_record_view.hpp"
#include "ui_geomap.hpp"
#include "app_settings.hpp"
#include "radio_state.hpp"
#include "recent_entries.hpp"
#include "ui_tabview.hpp"

#include "log_file.hpp"
#include "utility.hpp"
#include "file_path.hpp"

class APRSLogger {
   public:
    Optional<File::Error> append(const std::filesystem::path& filename) {
        return log_file.append(filename);
    }

    void log_raw_data(const std::string& data);

   private:
    LogFile log_file{};
};

namespace ui {

struct APRSRecentEntry {
    using Key = uint64_t;

    static constexpr Key invalid_key = 0xffffffffffffffff;

    uint16_t hits{0};
    uint32_t age{0};

    uint64_t source{0};
    std::string source_formatted{"        "};
    std::string time_string{""};
    std::string info_string{""};

    aprs::aprs_pos pos{0, 0, 0, 0};
    bool has_position = false;
    APRSRecentEntry(uint64_t src) {
        source = src;
    }

    Key key() const {
        return source;
    }

    void set_source_formatted(std::string& new_source) {
        source_formatted = new_source;
    }

    void inc_hit() {
        hits++;
    }

    void set_info_string(std::string& new_info_string) {
        info_string = new_info_string;
    }

    void set_time_string(std::string& new_time_string) {
        time_string = new_time_string;
    }

    void set_pos(aprs::aprs_pos pos_in) {
        pos = pos_in;
    }

    void set_has_position(bool has_pos) {
        has_position = has_pos;
    }

    void reset_age() {
        age = 0;
    }

    void inc_age() {
        age++;
    }
};

class APRSDetailsView : public View {
   public:
    APRSDetailsView(NavigationView&);
    ~APRSDetailsView();

    APRSDetailsView(const APRSDetailsView&) = delete;
    APRSDetailsView(APRSDetailsView&&) = delete;
    APRSDetailsView& operator=(const APRSDetailsView&) = delete;
    APRSDetailsView& operator=(APRSDetailsView&&) = delete;

    void focus() override;

    void update();
    void set_entry(const APRSRecentEntry& entry);

    const APRSRecentEntry& entry() const { return entry_copy; };

    std::string title() const override { return "Details"; };
    std::function<void(void)> on_close{};

   private:
    APRSRecentEntry entry_copy{0};
    GeoMapView* geomap_view{nullptr};
    bool send_updates{false};

    Console console{
        {0, 0 * 16, screen_width, 224}};

    Button button_done{
        {160, 14 * 16, 8 * 8, 3 * 16},
        "Close"};

    Button button_see_map{
        {80, 14 * 16, 8 * 8, 3 * 16},
        "Map"};
};

using APRSRecentEntries = RecentEntries<APRSRecentEntry>;

class APRSTableView : public View {
   public:
    APRSTableView(NavigationView& nav, Rect parent_rec);
    ~APRSTableView();

    void on_show() override;
    void on_hide() override;
    void focus() override;
    void on_pkt(const APRSPacketMessage* message);

    std::string title() const override { return "Stations"; };

   private:
    NavigationView& nav_;
    const RecentEntriesColumns columns{{{"Source", 9},
                                        {"Loc", 6},
                                        {"Hits", 4},
                                        {"Time", 8}}};
    APRSRecentEntries recent{};
    RecentEntriesView<RecentEntries<APRSRecentEntry>> recent_entries_view{columns, recent};
    APRSDetailsView details_view{nav_};
    uint32_t detailed_entry_key{0};
    bool send_updates{false};

    void on_show_list();
    void on_show_detail(const APRSRecentEntry& entry);
};

class APRSRxView : public View {
   public:
    APRSRxView(NavigationView& nav, Rect parent_rect);
    ~APRSRxView();

    void on_show() override;
    void focus() override;

    std::string title() const override { return "APRS RX"; };
    void on_packet(const APRSPacketMessage* message);
    void on_freqchg(int64_t freq);

   private:
    void on_data(uint32_t value, bool is_data);
    bool reset_console = false;
    uint8_t options_region_id = 1;          // default to North America
    rf::Frequency aprs_rx_freq{144390000};  // default to North America frequency

    NavigationView& nav_;
    RxRadioState radio_state_{
        144390000 /* frequency */,
        1750000 /* bandwidth */,
        3072000 /* sampling rate */
    };
    app_settings::SettingsManager settings_{
        "rx_aprs",
        app_settings::Mode::RX,
        {{"options_region_id"sv, &options_region_id},
         {"aprs_rx_freq"sv, &aprs_rx_freq}}};

    uint8_t console_color{0};
    std::string str_log{""};

    RFAmpField field_rf_amp{
        {13 * 8, 0 * 16}};
    LNAGainField field_lna{
        {15 * 8, 0 * 16}};
    VGAGainField field_vga{
        {18 * 8, 0 * 16}};
    RSSI rssi{
        {21 * 8, 0, 6 * 8, 4}};
    Channel channel{
        {21 * 8, 5, 6 * 8, 4}};

    AudioVolumeField field_volume{
        {screen_width - 2 * 8, 0 * 16}};

    OptionsField options_region{
        {0 * 8, 0 * 8},
        3,
        {{"MAN", 0},
         {"NA ", 1},
         {"NZ ", 2},
         {"JAP", 3},
         {"PHI", 4},
         {"EUR", 5},
         {"THA", 6},
         {"AUS", 7},
         {"BR ", 8},
         {"ISS", 9}}};

    FrequencyField field_frequency{
        {3 * 8, 0 * 16}};

    // DEBUG
    RecordView record_view{
        {0 * 8, 1 * 16, screen_width, 1 * 16},
        u"AFS_????.WAV",
        aprs_dir,
        RecordView::FileType::WAV,
        4096,
        4};

    Console console{
        {0, 2 * 16, screen_width, screen_height - 80}};

    std::unique_ptr<APRSLogger> logger{};
};

class APRSRXView : public View {
   public:
    APRSRXView(NavigationView& nav);
    ~APRSRXView();

    void focus() override;

    std::string title() const override { return "APRS RX"; };

   private:
    NavigationView& nav_;
    Rect view_rect = {0, 3 * 8, screen_width, screen_height - 40};

    APRSRxView view_stream{nav_, view_rect};
    APRSTableView view_table{nav_, view_rect};

    TabView tab_view{
        {"Stream", Theme::getInstance()->fg_cyan->foreground, &view_stream},
        {"List", Theme::getInstance()->fg_yellow->foreground, &view_table}};

    MessageHandlerRegistration message_handler_packet{
        Message::ID::APRSPacket,
        [this](Message* const p) {
            const auto message = static_cast<const APRSPacketMessage*>(p);
            this->view_stream.on_packet(message);
            this->view_table.on_pkt(message);
        }};

    MessageHandlerRegistration message_handler_freqchg{
        Message::ID::FreqChangeCommand,
        [this](Message* const p) {
            const auto message = static_cast<const FreqChangeCommandMessage*>(p);
            this->view_stream.on_freqchg(message->freq);
        }};
};

} /* namespace ui */

#endif /*__UI_APRS_RX_H__*/
