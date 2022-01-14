#pragma once
#include <RealEngine/MainProgram.hpp>

namespace RE {

	class TypingHandle {
	public:
		//Standard main program is used
		TypingHandle();
		TypingHandle(MainProgram* mainProgram);
		~TypingHandle();

		void type(bool type, bool blockPressInput = false);

		//Returns current string
		std::string visit(std::string* previousVisit = nullptr) const;

		std::string& getString() {
			return m_string;
		}

		const std::string& getString() const {
			return m_string;
		}

		//Returns whether the user is currently typing into this TypingHandle
		bool isBeingTypedInto() const;
	private:
		MainProgram* m_mainProgram = nullptr;
		std::string m_string;
		mutable std::string m_stringPrevious;
	};

}