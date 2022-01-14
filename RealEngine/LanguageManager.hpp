#pragma once
/*
Example of expression list

#define LM_EXPRESSION_LIST(m)   \
	m(MMENU, LOAD_A_SAVE)  \
	m(MMENU, MAP_EDITOR)  \
	m(MMENU, SETTINGS)  \
	m(MMENU, EXIT)  \
	m(MMENU, CREATE_NEW_MAP)  \
	m(MMENU, LOAD_A_MAP)  \
	m(MMENU, ENTER_NAME_OF_MAP)  \
	m(MMENU, CHOOSE_WIDTH_OF_MAP)  \
	m(MMENU, CHOOSE_HEIGHT_OF_MAP)  \
	m(MMENU, RETURN_TO_MAIN_MENU)
		/|\				/|\
	section name	option name

*/

#ifdef LM_EXPRESSION_LIST
#include <string>
#include <array>
#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>

#include <RealEngine/utility.hpp>
#include <RealEngine/Error.hpp>

//Deliberately outside of RE namespace
namespace {
	SMARTENUM_DEFINE_ENUM(EL, LM_EXPRESSION_LIST)
		SMARTENUM_DEFINE_NAMES(EL, LM_EXPRESSION_LIST)
		SMARTENUM_DEFINE_GET_VALUE_FROM_STRING(EL, LM_EXPRESSION_LIST)
}

#define LM(enum) (RE::LanguageManager::std())[EL::e##enum##]

namespace RE {

	struct LanguageInfo {
		std::string internationalName; //'international' name of the language(~english), this should be used for loadLanguage()
		std::string selfName; //name of the language in the language itself
		float translated; //in interval [0.0; 1.0], indicates how many expressions of the language are translated (1.0 means ale are translated, 0.0 means that not a single expression is translated)
	};

	class LanguageManager {
	public:
		LanguageManager() {
			try {
				//Try to load last language
				std::ifstream i(m_languagesFolder + "/" + m_metadataFile);
				nlohmann::json j;
				i >> j;
				setLanguage(j["current"]["language"].get<std::string>());
			}
			catch (...) {
				//Could not load last language, using expression names instead
				std::copy(&ELArray[0], &ELArray[EL::eEL_Count], m_expNames.data());
			}
		}

		//Expressions have default names
		LanguageManager(std::array<std::string, (size_t)EL::eEL_Count> defaultExprs) : m_expNames(defaultExprs) {

		}

		//Loads a language
		LanguageManager(const std::string& internationalName) {
			setLanguage(internationalName);
		}

		const std::string& operator[](EL exp) {
			return m_expNames[(size_t)exp];
		}

		const std::string& operator[](const std::string& expName) {
			return m_expNames[(size_t)getELFromString(expName)];
		}

		//Comprehensive function to get all available languages
		//1st string -> 'international' name of the language (~ english), this should be used for loadLanguage()
		//2nd string -> name of the language in the language itself
		//float -> in interval [0.0f; 1.0f], indicates how much of the language is translated (1.0f stands for fully translated, 0.0f for not a single expression translated)
		void getAvailableLanguages(std::vector<LanguageInfo>& outputVector) const {
			outputVector.clear();

			for (auto& dir_entry : std::filesystem::directory_iterator{ m_languagesFolder }) {
				if (!dir_entry.is_regular_file()) continue;//Avoid non-files
				auto& path = dir_entry.path();
				if (path.extension() != ".json") continue;//JSON only
				if (path.filename() == m_metadataFile) continue;//Skip metadata file

				LanguageInfo language;

				//Set international name of the language
				language.internationalName = path.stem().string();

				//Load the language
				nlohmann::json j;
				try {
					std::ifstream i(m_languagesFolder + "/" + path.filename().string());
					i >> j;
				}
				catch (...) {
					RE::error("Found language " + path.filename().string() + " but it could not be loaded.");
					continue;
				}

				//Get the self-name of the language
				try {
					language.selfName = j["LANGUAGE"]["selfname"].get<std::string>();
				}
				catch (...) {
					//Language doesn't declare its own name, international name will be used instead
					language.selfName = language.internationalName;
				}

				//Measuring how many expressions of the language are translated
				std::string sectionName;
				std::array<std::string, (unsigned int)EL::eEL_Count> tempExpressions{};
				for (auto outer = j.begin(); outer != j.end(); outer++) {
					for (auto inner = outer.value().begin(); inner != outer.value().end(); inner++) {
						tempExpressions[(size_t)(stringToEnum(EL, 'e' + outer.key() + '_' + inner.key()))] = inner.value().get<std::string>();
					}
				}
				float counter = 0.0f;
				for (int i = 0; i < (int)EL::eEL_Count; i++) {
					if (tempExpressions[i] != "" && i != (int)EL::eEL_Error) {
						++counter;
					}
				}
				language.translated = counter / ((float)EL::eEL_Count - 1.0f);

				outputVector.push_back(language);
			}
		}

		//Returns if the load was successful
		bool setLanguage(const std::string& internationalName) {
			nlohmann::json j;
			try {
				std::ifstream i(m_languagesFolder + "/" + internationalName + ".json");
				i >> j;
			}
			catch (...) {//Something went wrong
				return false;
			}

			//Assigning loaded config to expressions
			for (auto outer = j.begin(); outer != j.end(); outer++) {
				for (auto inner = outer.value().begin(); inner != outer.value().end(); inner++) {
					m_expNames[(size_t)(stringToEnum(EL, 'e' + outer.key() + '_' + inner.key()))] = inner.value().get<std::string>();
				}
			}
			//Set correct error expression
			try {
				m_expNames[(size_t)EL::eEL_Error] = j["LANGUAGE"]["error_exp"].get<std::string>();
			}
			catch (...) {
				m_expNames[(size_t)EL::eEL_Error] = "<ERROR_EXP>";
			}
			//Measuring completeness
			int counter = 0;
			for (int i = 0; i < (int)EL::eEL_Count; i++) {
				if (m_expNames[i] == "" && i != (int)EL::eEL_Error) {
					m_expNames[i] = enumToString(EL, i);
					++counter;
				}
			}
			if (counter > 1) {
				std::printf("%i expressions (%.0f %%) were missing in currently loaded language: %s.\nDefault (~english) expressions have been used for the missing ones.\n", counter, (float)counter / (((float)(EL::eEL_Count - 1)) * 100.0f), internationalName.c_str());
			}

			//Overwrite metadata
			try {
				nlohmann::json j;
				std::ifstream i(m_languagesFolder + "/" + m_metadataFile);
				i >> j;
				i.close();
				if (j["current"]["language"] != internationalName) {//If actually changed language
					j["current"]["language"] = internationalName;
					std::ofstream o(m_languagesFolder + "/" + m_metadataFile, std::ofstream::trunc);
					o << j.dump(2);
					o.close();
				}
			}
			catch (...) {//Something went wrong
				//Create default metadata file
				nlohmann::json j{
					{"current", {
						{"language", internationalName}
					}}
				};
				std::ofstream o(m_languagesFolder + "/" + m_metadataFile, std::ofstream::trunc);
				o << j.dump(2);
				o.close();
			}

			return true;
		};

		static LanguageManager& std() {
			static LanguageManager std{};
			return std;
		};
	private:
		std::array<std::string, (size_t)EL::eEL_Count> m_expNames;
		std::string m_languagesFolder = "localization";
		std::string m_metadataFile = "meta.json";
	};

}
#endif // LM_EXPRESSION_LIST