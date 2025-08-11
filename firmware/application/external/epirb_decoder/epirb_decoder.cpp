#include "epirb_decoder.hpp"
#include "ui_epirb.hpp"
#include "baseband_api.hpp"
#include "receiver_model.hpp"

namespace ui::external_apps::epirb_decoder {

void external_app__epirb_decoder__run(NavigationView& nav) {
    nav.push<EPIRBDecoderApp>();
}

EPIRBDecoderApp::EPIRBDecoderApp(NavigationView& nav) {
    view = new EPIRBRxView{nav};
    add_child(*view);
}

EPIRBDecoderApp::~EPIRBDecoderApp() {
    receiver_model.disable();
    baseband::shutdown();
}

void EPIRBDecoderApp::focus() {
    view->focus();
}

}  // namespace ui::external_apps::epirb_decoder