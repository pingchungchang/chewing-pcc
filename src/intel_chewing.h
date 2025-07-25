/*
 * SPDX-FileCopyrightText: 2021~2021 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _FCITX5_INTEL_CHEWING_INTEL_CHEWING_H_
#define _FCITX5_INTEL_CHEWING_INTEL_CHEWING_H_

#include <fcitx-utils/inputbuffer.h>
#include <fcitx/addonfactory.h>
#include <fcitx/addonmanager.h>
#include <fcitx/inputcontext.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/inputpanel.h>
#include <fcitx/instance.h>
#include <chewing.h>

#include <fcitx-utils/i18n.h>
#include <fcitx-utils/misc.h>
#include <fcitx-utils/trackableobject.h>
#include <fcitx-config/configuration.h>
#include <fcitx-config/enum.h>
#include <fcitx-config/iniparser.h>
#include <fcitx-config/option.h>
#include <fcitx-config/rawconfig.h>

namespace IntelChewingConfigs {
	bool ShowEnglishInsteadOfBopomofo = true;
	int ErrorCount = 0;
}

class IntelChewingEngine;

class IntelChewingState : public fcitx::InputContextProperty {
public:
	void initChewing();
    IntelChewingState(IntelChewingEngine *engine, fcitx::InputContext *ic)
        : engine_(engine), ic_(ic) {
			initChewing();
	}

    void handleEvent(fcitx::KeyEvent &keyEvent);
    void handleKeyEvent(fcitx::KeyEvent &keyEvent);
	void handleCandidateEvent(fcitx::KeyEvent& keyEvent);
	ChewingContext* getChewing() { return chewing_ctx; }
    void updateUI();
    void reset() {
		chewing_Reset(chewing_ctx);
		chewing_set_ChiEngMode(chewing_ctx, 1);
        updateUI();
    }
	void setCandidateCursor(int index) { candidate_cursor_ = index; }
	int getCandidateCursor() { return candidate_cursor_; }
	~IntelChewingState() {
		if (chewing_ctx) {
			chewing_delete(chewing_ctx);
		}
	}
	bool iThinkItIsEnglish();

private:
    IntelChewingEngine *engine_;
    fcitx::InputContext *ic_;
	ChewingContext* chewing_ctx;
	int candidate_cursor_;
	std::string bopomofo_eng_;
	std::string prev_buffer_;
	int current_language_ = 1;
	bool to_eng_handled_ = false;
};

class IntelChewingEngine : public fcitx::InputMethodEngineV2 {
public:
    IntelChewingEngine(fcitx::Instance *instance);

    void keyEvent(const fcitx::InputMethodEntry &entry,
                  fcitx::KeyEvent &keyEvent) override;

    void reset(const fcitx::InputMethodEntry &,
               fcitx::InputContextEvent &event) override;

    auto factory() const { return &factory_; }
    auto instance() const { return instance_; }

private:
    fcitx::Instance *instance_;
    fcitx::FactoryFor<IntelChewingState> factory_;
};

class IntelChewingEngineFactory : public fcitx::AddonFactory {
    fcitx::AddonInstance *create(fcitx::AddonManager *manager) override {
        FCITX_UNUSED(manager);
        return new IntelChewingEngine(manager->instance());
    }
};

#endif // _FCITX5_INTEL_CHEWING_INTEL_CHEWING_H_
