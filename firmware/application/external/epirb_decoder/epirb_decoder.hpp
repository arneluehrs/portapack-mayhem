#pragma once

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "external_app.hpp"

namespace ui::external_apps::epirb_decoder {

class EPIRBDecoderApp : public View {
public:
    explicit EPIRBDecoderApp(NavigationView& nav);
    ~EPIRBDecoderApp() override;

    void focus() override;

private:
    class EPIRBRxView* view{nullptr};
};

extern "C" {
void external_app__epirb_decoder__run(NavigationView& nav);
}

}  // namespace ui::external_apps::epirb_decoder