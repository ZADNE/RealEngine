#pragma once
#include <RealEngine/main/MainProgram.hpp>
#include <RealEngine/graphics/Font.hpp>

namespace RE {

	class TypingHandle {
	public:
		//Standard main program is used
		TypingHandle();
		TypingHandle(MainProgram* mainProgram);
		~TypingHandle();

		void type(bool type, bool blockPressInput = false);

		//Returns current string
		FontString visit(FontString* previousVisit = nullptr) const;

		FontString& getString() {
			return m_string;
		}

		const FontString& getString() const {
			return m_string;
		}

		//Returns whether the user is currently typing into this TypingHandle
		bool isBeingTypedInto() const;
	private:
		MainProgram* m_mainProgram = nullptr;
		FontString m_string;
		mutable FontString m_stringPrevious;
	};

}