add_external_module(
    NAME        epirb_decoder
    SOURCES
        epirb_decoder.cpp
    DEPENDS
        baseband
        ui
        portapack
)