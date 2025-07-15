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

class IntelChewingEngine;

class IntelChewingState : public fcitx::InputContextProperty {
	public:
		ChewingContext* initChewing();
		IntelChewingState(IntelChewingEngine *engine, fcitx::InputContext *ic)
			: engine_(engine), ic_(ic) {
				chewing_ctx_.push_back(std::unique_ptr<ChewingContext, decltype(&chewing_delete)>(initChewing(), &chewing_delete));
				preedit_ctx_ = chewing_ctx_.begin();
			}

		void handleEvent(fcitx::KeyEvent &keyEvent);
		void handleKeyEvent(fcitx::KeyEvent &keyEvent);
		void handleCandidateEvent(fcitx::KeyEvent& keyEvent);
		ChewingContext* getChewing() { return preedit_ctx_ -> get(); }
		void updateUI();
		void reset() {
			chewing_Reset(preedit_ctx_ -> get());
			updateUI();
		}
		void setCandidateCursor(int index) { candidate_cursor_ = index; }
		int getCandidateCursor() { return candidate_cursor_; }
	private:

		void moveChewingCursor(ChewingContext* ctx, int rounds, int dir) {
			while(rounds --) {
				switch(dir) {
					case -1:
						chewing_handle_Left(ctx);  break;
					case 1:
						chewing_handle_Right(ctx); break;
				}
			}
			return;
		}

		IntelChewingEngine *engine_;
		fcitx::InputContext *ic_;
		std::vector<std::unique_ptr<ChewingContext, decltype(&chewing_delete)>> chewing_ctx_;
		std::vector<std::unique_ptr<ChewingContext, decltype(&chewing_delete)>>::iterator preedit_ctx_;
		int candidate_cursor_;
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
