#include <RealEngine/TypingHandle.hpp>

namespace RE {

	TypingHandle::TypingHandle() : m_mainProgram(MainProgram::std) {

	}

	TypingHandle::TypingHandle(MainProgram* mainProgram) : m_mainProgram(mainProgram) {

	}

	TypingHandle::~TypingHandle() {
		if (m_mainProgram) {
			m_mainProgram->setTypeString(nullptr);
		}
	}

	void TypingHandle::type(bool type, bool blockPressInput/* = false*/) {
		if (m_mainProgram) {
			m_mainProgram->setTypeString(type ? &m_string : nullptr, blockPressInput);
		}
	}

	FontString TypingHandle::visit(FontString* previousVisit) const {
		if (previousVisit) {
			*previousVisit = m_stringPrevious;
		}
		m_stringPrevious = m_string;
		return m_string;
	}

	bool TypingHandle::isBeingTypedInto() const {
		if (m_mainProgram && m_mainProgram->getTypeString() == &m_string) {
			return true;
		}
		return false;
	}

}