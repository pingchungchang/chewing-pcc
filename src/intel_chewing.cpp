/*
 * SPDX-FileCopyrightText: 2021~2021 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "intel_chewing.h"
#include <fcitx-utils/utf8.h>
#include <fcitx/candidatelist.h>
#include <fcitx/inputpanel.h>
#include <fcitx/instance.h>
#include <fcitx/userinterfacemanager.h>
#include <utility>

namespace {

const int SEL_KEYS[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
const char* CHEWING_DATA_DIR = "/usr/share/libchewing";
const char* USER_HOME = getenv("HOME");
std::string user_data_dir = std::string(USER_HOME) + "/.local/share/chewing.sqlite3";
// Template to help resolve iconv parameter issue on BSD.
template <class T>
struct function_traits;

// partial specialization for function pointer
template <class R, class... Args>
struct function_traits<R (*)(Args...)> {
    using result_type = R;
    using argument_types = std::tuple<Args...>;
};

template <class T>
using second_argument_type = typename std::tuple_element<
    1, typename function_traits<T>::argument_types>::type;

static const std::array<fcitx::Key, 10> selectionKeys = {
    fcitx::Key{FcitxKey_1}, fcitx::Key{FcitxKey_2}, fcitx::Key{FcitxKey_3},
    fcitx::Key{FcitxKey_4}, fcitx::Key{FcitxKey_5}, fcitx::Key{FcitxKey_6},
    fcitx::Key{FcitxKey_7}, fcitx::Key{FcitxKey_8}, fcitx::Key{FcitxKey_9},
    fcitx::Key{FcitxKey_0},
};

class IntelChewingCandidateWord : public fcitx::CandidateWord {
public:
    IntelChewingCandidateWord(IntelChewingEngine *engine, std::string text, int index = 0)
        : engine_(engine), index_(index) {
        setText(fcitx::Text(std::move(text)));
    }

    void select(fcitx::InputContext *inputContext) const override {
        auto *state = inputContext->propertyFor(engine_->factory());
		FCITX_INFO() << "total choices: "<<chewing_cand_TotalChoice(state -> getChewing());
		int ret = chewing_cand_choose_by_index(state -> getChewing(), index_);
		FCITX_INFO() << "text = " << text().toString();
		FCITX_INFO() << "choose return val = " << ret;
		FCITX_INFO() << "CHOSE BY INDEX: "<<index_;
		state -> updateUI();
    }

private:
	int index_;
    IntelChewingEngine *engine_;
};

class IntelChewingCandidateList : public fcitx::CandidateList,
                           public fcitx::PageableCandidateList,
                           public fcitx::CursorMovableCandidateList,
						   public fcitx::CursorModifiableCandidateList{
public:
    IntelChewingCandidateList(IntelChewingEngine *engine, fcitx::InputContext *ic,
                       ChewingContext* chewing_ctx)
        : engine_(engine), ic_(ic), chewing_ctx_(chewing_ctx) {
        setPageable(this);
        setCursorMovable(this);
		setCursorModifiable(this);
        for (int i = 0; i < 10; i++) {
            const char label[2] = {static_cast<char>('0' + (i + 1) % 10), '\0'};
            labels_[i].append(label);
            labels_[i].append(". ");
        }
        generate();
    }

    const fcitx::Text &label(int idx) const override { return labels_[idx]; }

    const fcitx::CandidateWord &candidate(int idx) const override {
		assert(idx < size());
        return *candidates_[idx];
    }
    int size() const override { return size_; }
    fcitx::CandidateLayoutHint layoutHint() const override {
        return fcitx::CandidateLayoutHint::Horizontal;
    }
    bool usedNextBefore() const override { return false; }
    void prev() override {
		FCITX_INFO() << "chewing handle prev";
        if (!hasPrev()) {
            return;
        }
        auto *state = ic_->propertyFor(engine_->factory());
		chewing_handle_PageUp(state -> getChewing());
		cursor_ = 0;
		setCursorIndex(cursor_);
		generate();
    }
    void next() override {
		FCITX_INFO() << "chewing handle next";
        if (!hasNext()) {
            return;
        }
        auto *state = ic_->propertyFor(engine_->factory());
		chewing_handle_Down(state -> getChewing());
		cursor_ = 0;
		setCursorIndex(cursor_);
		generate();
    }

    bool hasPrev() const override { /*TODO*/ return true; }

    bool hasNext() const override { /*TODO*/ return true; }

    void prevCandidate() override {
		cursor_ --;
		if (cursor_ < 0) {
			prev();
			cursor_ = 0;
		}
		setCursorIndex(cursor_);
	}

    void nextCandidate() override {
		cursor_ ++;
		if (cursor_ >= size()) {
			next();
			cursor_ = 0;
		}
		setCursorIndex(cursor_);
	}

    int cursorIndex() const override { return cursor_; }

	void setCursorIndex(int index) {
		if (index >= 0 && index < size()) {
			cursor_ = index;
			auto *state = ic_->propertyFor(engine_->factory());
			state -> setCursor(index);
		}
		return;
	}

private:
    void generate() {
		size_ = 0;
        auto *state = ic_->propertyFor(engine_->factory());
		int now_page = chewing_cand_CurrentPage(state -> getChewing());
		int head = now_page * chewing_cand_ChoicePerPage(state -> getChewing());
		int tail = std::min((now_page + 1) * chewing_cand_ChoicePerPage(state -> getChewing()), chewing_cand_TotalChoice(state -> getChewing()));

		for(int i = 0, now = head;i<10 && now < tail;i++, now++) {
			std::string cand_word(chewing_cand_string_by_index_static(state -> getChewing(), now));
			//FCITX_INFO() << "total: "<<chewing_cand_TotalChoice(state -> getChewing());
			//FCITX_INFO() << "generating: "<<head<<','<<tail<<','<<now_page<<','<<now<<','<<i<<','<<cand_word;
            candidates_[i] = std::make_unique<IntelChewingCandidateWord>(engine_, cand_word, i);
			size_ = i+1;
        }
    }

	ChewingContext *chewing_ctx_;
    IntelChewingEngine *engine_;
    fcitx::InputContext *ic_;
    fcitx::Text labels_[10];
    std::unique_ptr<IntelChewingCandidateWord> candidates_[10];
	int size_ = 0;
    int cursor_ = 0;
};

} // namespace
  //
void IntelChewingState::initChewing() {
	chewing_ctx = chewing_new2(CHEWING_DATA_DIR, user_data_dir.c_str(), NULL, 0);
	if (!chewing_ctx) {
		FCITX_INFO() << "error initializing chewing_ctx";
		return;
	}
	chewing_set_selKey(chewing_ctx, SEL_KEYS, 10);
	chewing_set_maxChiSymbolLen(chewing_ctx, 10);
	chewing_set_candPerPage(chewing_ctx, 10);
	return;
}

void IntelChewingState::handleCandidateEvent(fcitx::KeyEvent &event) {
    if (auto candidateList = ic_->inputPanel().candidateList()) {
        int idx = event.key().keyListIndex(selectionKeys);
        if (idx >= 0 && idx < candidateList->size()) {
            event.accept();
            candidateList->candidate(idx).select(ic_);
            return;
        }
        if (event.key().checkKeyList(
                engine_->instance()->globalConfig().defaultPrevPage())) {
            if (auto *pageable = candidateList->toPageable();
                pageable && pageable->hasPrev()) {
                event.accept();
                pageable->prev();
                ic_->updateUserInterface(
                    fcitx::UserInterfaceComponent::InputPanel);
            }
            return event.filterAndAccept();
        }
        if (event.key().checkKeyList(
                engine_->instance()->globalConfig().defaultNextPage())) {
            if (auto *pageable = candidateList->toPageable();
                pageable && pageable->hasNext()) {
                pageable->next();
                ic_->updateUserInterface(
                    fcitx::UserInterfaceComponent::InputPanel);
            }
            return event.filterAndAccept();
        }
		if (event.key().checkKeyList(
                engine_->instance()->globalConfig().defaultPrevCandidate())
				|| event.key().check(FcitxKey_Left)) {
			FCITX_INFO() << "PREV CANDIDATE";
			if (auto *movable = candidateList->toCursorMovable(); movable) {
				movable -> prevCandidate();
                ic_->updateUserInterface(
                    fcitx::UserInterfaceComponent::InputPanel);
			}
            return event.filterAndAccept();
		}
		if (event.key().checkKeyList(
                engine_->instance()->globalConfig().defaultNextCandidate())
				|| event.key().check(FcitxKey_Right)) {
			FCITX_INFO() << "NEXT CANDIDATE";
			if (auto *movable = candidateList->toCursorMovable(); movable) {
				movable -> nextCandidate();
                ic_->updateUserInterface(
                    fcitx::UserInterfaceComponent::InputPanel);
			}
            return event.filterAndAccept();
		}
		if (event.key().check(FcitxKey_Return)) {
			candidateList -> candidate(candidateList -> cursorIndex()).select(ic_);
			return event.filterAndAccept();
		}
    }
	else {
		FCITX_INFO() << "warning no candidate list!!";
	}

}

void IntelChewingState::handleKeyEvent(fcitx::KeyEvent &event) {
	if (event.key().check(FcitxKey_space)) {
		chewing_handle_Space(chewing_ctx);
	} else if (event.key().check(FcitxKey_Escape)) {
		chewing_handle_Esc(chewing_ctx);
	} else if (event.key().check(FcitxKey_Return)) {
		chewing_handle_Enter(chewing_ctx);
	} else if (event.key().check(FcitxKey_Delete)) {
		chewing_handle_Del(chewing_ctx);
	} else if (event.key().check(FcitxKey_BackSpace)) {
		chewing_handle_Backspace(chewing_ctx);
	} else if (event.key().check(FcitxKey_Tab)) {
		chewing_handle_Tab(chewing_ctx);
	} else if (event.key().check(FcitxKey_Shift_L)) {
		chewing_handle_ShiftLeft (chewing_ctx);
	} else if (event.key().check(FcitxKey_Left)) {
		chewing_handle_Left(chewing_ctx);
	} else if (event.key().check(FcitxKey_Shift_R)) {
		chewing_handle_ShiftRight(chewing_ctx);
	} else if (event.key().check(FcitxKey_Right)) {
		chewing_handle_Right(chewing_ctx);
	} else if (event.key().check(FcitxKey_Up)) {
		chewing_handle_Up(chewing_ctx);
	} else if (event.key().check(FcitxKey_Home)) {
		chewing_handle_Home(chewing_ctx);
	} else if (event.key().check(FcitxKey_End)) {
		chewing_handle_End(chewing_ctx);
	} else if (event.key().check(FcitxKey_Page_Up)) {
		chewing_handle_PageUp(chewing_ctx);
	} else if (event.key().check(FcitxKey_Page_Down)) {
		chewing_handle_PageDown(chewing_ctx);
	} else if (event.key().check(FcitxKey_Down)) {
		chewing_handle_Down(chewing_ctx);
	} else if (event.key().check(FcitxKey_Caps_Lock)) {
		chewing_handle_Capslock(chewing_ctx);
	} else {
		chewing_handle_Default(chewing_ctx, event.key().sym());
	}
	if (chewing_keystroke_CheckIgnore(chewing_ctx)) return;
	else return event.filterAndAccept();
}

void IntelChewingState::handleEvent(fcitx::KeyEvent &event) {
	if (chewing_cand_CheckDone(chewing_ctx)) {
		handleKeyEvent(event);
	}
	else handleCandidateEvent(event);
	updateUI();
	return;
}

void IntelChewingState::updateUI() {
    auto &inputPanel = ic_->inputPanel();
    inputPanel.reset();
	if (chewing_commit_Check(chewing_ctx)) {
		std::string commit_string(chewing_commit_String_static(chewing_ctx));
		FCITX_INFO() << "commiting: "<<commit_string;
        ic_->commitString(commit_string);
	}
	if (!chewing_cand_CheckDone(chewing_ctx)) {
		// show candidate list
        inputPanel.setCandidateList(std::make_unique<IntelChewingCandidateList>(
            engine_, ic_, chewing_ctx));
		inputPanel.candidateList() -> toCursorModifiable() -> setCursorIndex(cursor_);
	}

	std::string buffer_string(chewing_buffer_String_static(chewing_ctx));
	FCITX_INFO() << "buffer string = " << buffer_string;
	int bytes_per_word = 0;
	if (!buffer_string.empty()) {
		bytes_per_word = buffer_string.size() / chewing_buffer_Len(chewing_ctx);
	}
	int buffer_cursor = chewing_cursor_Current(chewing_ctx);
	std::string shown_text = 
		buffer_string.substr(0, bytes_per_word * buffer_cursor) 
		+ std::string(chewing_bopomofo_String_static(chewing_ctx)) 
		+ buffer_string.substr(bytes_per_word * buffer_cursor, buffer_string.size() - bytes_per_word * buffer_cursor);
	FCITX_INFO() << "cursor = " << bytes_per_word * buffer_cursor << ',' << buffer_string.size();

    if (ic_->capabilityFlags().test(fcitx::CapabilityFlag::Preedit)) {
        fcitx::Text preedit(shown_text,
                            fcitx::TextFormatFlag::HighLight);
		preedit.setCursor(bytes_per_word * buffer_cursor);
        inputPanel.setClientPreedit(preedit);
    } else {
        fcitx::Text preedit(shown_text);
		preedit.setCursor(bytes_per_word * buffer_cursor);
        inputPanel.setPreedit(preedit);
    }
    ic_->updateUserInterface(fcitx::UserInterfaceComponent::InputPanel);
    ic_->updatePreedit();
}

IntelChewingEngine::IntelChewingEngine(fcitx::Instance *instance)
    : instance_(instance), factory_([this](fcitx::InputContext &ic) {
          return new IntelChewingState(this, &ic);
      }) {
    instance->inputContextManager().registerProperty("intelChewingState", &factory_);
}

void IntelChewingEngine::keyEvent(const fcitx::InputMethodEntry &entry,
                           fcitx::KeyEvent &keyEvent) {
    FCITX_UNUSED(entry);
    if (keyEvent.isRelease() || keyEvent.key().states()) {
        return;
    }
    // FCITX_INFO() << keyEvent.key() << " isRelease=" << keyEvent.isRelease();
    auto ic = keyEvent.inputContext();
    auto *state = ic->propertyFor(&factory_);
	state->handleEvent(keyEvent);
}

void IntelChewingEngine::reset(const fcitx::InputMethodEntry &,
                        fcitx::InputContextEvent &event) {
    auto *state = event.inputContext()->propertyFor(&factory_);
    state->reset();
}

FCITX_ADDON_FACTORY(IntelChewingEngineFactory);

