#ifndef __PROC_EPIRB_D_HPP__
#define __PROC_EPIRB_D_HPP__

#include "dsp_demod.hpp"
#include "dsp_types.hpp"
#include "dsp_iir_config.hpp"
#include "dsp_math.hpp"
#include "dsp_tone_detect.hpp"
#include "shared_memory.hpp"
#include "epirb_packet.hpp"

namespace dsp {

/*  Very small tone detector tuned to the 1 100 Hz alert tone
    (160 ms minimum).  Used only to locate the burst start.      */
using tone_detector_1100 = dsp::tone_detector<1100, 160>;

/*  Main baseband processor for the external EPIRB-decoder app.  */
class EPIRBDecoder : public Processor {
public:
    EPIRBDecoder()  = default;
    ~EPIRBDecoder() override = default;

    /* Called by baseband thread for every audio buffer. */
    void execute(const buffer_c8_t& buffer) override;

private:
    /* Bit-by-bit demod helpers */
    bool read_bits(uint8_t* dst, size_t bit_count);

    /* COSPAS-SARSAT decoder */
    EPIRBPacket decode_beacon(const uint8_t* b, bool second_frame);

    /* State machine for long/short message handling */
    enum class State { WAIT_FIRST, WAIT_SECOND };
    State state_ = State::WAIT_FIRST;

    /* Temporary storage between frames */
    EPIRBPacket first_;
    uint32_t    frame_counter_ = 0;

    /* 1 100 Hz tone detector */
    tone_detector_1100 tone_detector_{};
};

}  /* namespace dsp */

#endif /* __PROC_EPIRB_D_HPP__ */