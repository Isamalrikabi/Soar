/////////////////////////////////////////////////////////////////
// CommandLineInterface class
//
// Author: Jonathan Voigt
// Date  : Sept 2004
//
/////////////////////////////////////////////////////////////////
#include "cli_CommandLineInterface.h"

#ifdef WIN32
#include <direct.h>
#endif // WIN32

#include "cli_GetOpt.h"

// BADBAD: I think we should be using an error class instead to work with error objects.
#include "../../gSKI/src/gSKI_Error.h"

// SML includes
#include "sml_Connection.h"
#include "sml_TagResult.h"
#include "sml_TagArg.h"

using namespace cli;
using namespace sml;

//  ____                                          _ _     _            ___       _             __
// / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| | |   (_)_ __   ___|_ _|_ __ | |_ ___ _ __ / _| __ _  ___ ___
//| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` | |   | | '_ \ / _ \| || '_ \| __/ _ \ '__| |_ / _` |/ __/ _ \
//| |__| (_) | | | | | | | | | | | (_| | | | | (_| | |___| | | | |  __/| || | | | ||  __/ |  |  _| (_| | (_|  __/
// \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|_____|_|_| |_|\___|___|_| |_|\__\___|_|  |_|  \__,_|\___\___|
//
EXPORT CommandLineInterface::CommandLineInterface() {

	// Create getopt object
	m_pGetOpt = new GetOpt;

	// Map command names to processing function pointers
	BuildCommandMap();

	// Store current working directory as 'home' dir
	if (!GetCurrentWorkingDirectory(m_HomeDirectory)) {
		// TODO: figure out what to do here!
		// ignore for now!
	}

	// Give print handlers a reference to us
	m_ResultPrintHandler.SetCLI(this);
	m_LogPrintHandler.SetCLI(this);

	// Initialize other members
	m_QuitCalled = false;
	m_pKernel = 0;
	m_SourceError = false;
	m_SourceDepth = 0;
	m_SourceDirDepth = 0;
	m_pLogFile = 0;
}

// /\/|____                                          _ _     _            ___       _             __
//|/\// ___|___  _ __ ___  _ __ ___   __ _ _ __   __| | |   (_)_ __   ___|_ _|_ __ | |_ ___ _ __ / _| __ _  ___ ___
//   | |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` | |   | | '_ \ / _ \| || '_ \| __/ _ \ '__| |_ / _` |/ __/ _ \
//   | |__| (_) | | | | | | | | | | | (_| | | | | (_| | |___| | | | |  __/| || | | | ||  __/ |  |  _| (_| | (_|  __/
//    \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|_____|_|_| |_|\___|___|_| |_|\__\___|_|  |_|  \__,_|\___\___|
//
EXPORT CommandLineInterface::~CommandLineInterface() {
	if (m_pGetOpt) {
		delete m_pGetOpt;
	}
	if (m_pLogFile) {
		delete m_pLogFile;
	}
}

// ____        _ _     _  ____                                          _ __  __
//| __ ) _   _(_) | __| |/ ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |  \/  | __ _ _ __
//|  _ \| | | | | |/ _` | |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` | |\/| |/ _` | '_ \
//| |_) | |_| | | | (_| | |__| (_) | | | | | | | | | | | (_| | | | | (_| | |  | | (_| | |_) |
//|____/ \__,_|_|_|\__,_|\____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|_|  |_|\__,_| .__/
//                                                                                    |_|
void CommandLineInterface::BuildCommandMap() {

	m_CommandMap[Constants::kCLICD]					= ParseCD;
	m_CommandMap[Constants::kCLIEcho]				= ParseEcho;
	m_CommandMap[Constants::kCLIExcise]				= ParseExcise;
	m_CommandMap[Constants::kCLIHelp]				= ParseHelp;
	m_CommandMap[Constants::kCLIHelpEx]				= ParseHelpEx;
	m_CommandMap[Constants::kCLIInitSoar]			= ParseInitSoar;
	m_CommandMap[Constants::kCLILearn]				= ParseLearn;
	m_CommandMap[Constants::kCLILog]				= ParseLog;
	m_CommandMap[Constants::kCLILS]					= ParseLS;
	m_CommandMap[Constants::kCLIMultiAttributes]	= ParseMultiAttributes;
	m_CommandMap[Constants::kCLIPopD]				= ParsePopD;
	m_CommandMap[Constants::kCLIPrint]				= ParsePrint;
	m_CommandMap[Constants::kCLIPushD]				= ParsePushD;
	m_CommandMap[Constants::kCLIPWD]				= ParsePWD;
	m_CommandMap[Constants::kCLIQuit]				= ParseQuit;
	m_CommandMap[Constants::kCLIRun]				= ParseRun;
	m_CommandMap[Constants::kCLISource]				= ParseSource;
	m_CommandMap[Constants::kCLISP]					= ParseSP;
	m_CommandMap[Constants::kCLIStats]				= ParseStats;
	m_CommandMap[Constants::kCLIStopSoar]			= ParseStopSoar;
	m_CommandMap[Constants::kCLITime]				= ParseTime;
	m_CommandMap[Constants::kCLIWarnings]			= ParseWarnings;
	m_CommandMap[Constants::kCLIWatch]				= ParseWatch;
}

// ____         ____                                          _
//|  _ \  ___  / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
//| | | |/ _ \| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
//| |_| | (_) | |__| (_) | | | | | | | | | | | (_| | | | | (_| |
//|____/ \___/ \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
//
EXPORT bool CommandLineInterface::DoCommand(Connection* pConnection, gSKI::IAgent* pAgent, const char* pCommandLine, ElementXML* pResponse, bool rawOutput, gSKI::Error* pError) {

	// Clear the result
	m_Result.clear();
	m_ErrorMessage.clear();
	m_ResponseTags.clear();

	// Save the pointers
	m_pError = pError;

	// Save the raw output flag
	m_RawOutput = rawOutput;

	// Process the command, ignoring its result (irrelevant at this level)
	bool ret = DoCommandInternal(pAgent, pCommandLine);

	// Reset source error flag
	m_SourceError = false;

	if (ret) {
		// The command succeeded, so return the result if raw output
		if (m_RawOutput) {
			pConnection->AddSimpleResultToSMLResponse(pResponse, m_Result.c_str());
		} else {
			// If there are tags in the response list, add them and return
			if (m_ResponseTags.size()) {
				TagResult* pTag = new TagResult();

				ElementXMLListIter iter = m_ResponseTags.begin();
				while (iter != m_ResponseTags.end()) {
					pTag->AddChild(*iter);
					m_ResponseTags.erase(iter);
					iter = m_ResponseTags.begin();
				}

				pResponse->AddChild(pTag);

			} else {
				// Otherwise, return result as simple result if there is one
				if (m_Result.size()) {
					pConnection->AddSimpleResultToSMLResponse(pResponse, m_Result.c_str());
				} else {
					// Or, simply return true
					pConnection->AddSimpleResultToSMLResponse(pResponse, sml_Names::kTrue);
				}
			}
		}
	} else {
		// The command failed, add the error message
		pConnection->AddErrorToSMLResponse(pResponse, m_ErrorMessage.c_str());
	}

	// Always returns true to indicate that we've generated any needed error message already
	return true ;
}

// ____         ____                                          _
//|  _ \  ___  / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
//| | | |/ _ \| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
//| |_| | (_) | |__| (_) | | | | | | | | | | | (_| | | | | (_| |
//|____/ \___/ \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
//
EXPORT bool CommandLineInterface::DoCommand(gSKI::IAgent* pAgent, const char* pCommandLine, char const* pResponse, gSKI::Error* pError) {
	// This function is for processing a command without the SML layer
	// Clear the result
	m_Result.clear();
	m_ErrorMessage.clear();

	// Save the pointers
	m_pError = pError;

	// Process the command, ignoring its result (irrelevant at this level)
	bool ret = DoCommandInternal(pAgent, pCommandLine);

	// Reset source error flag
	m_SourceError = false;

	pResponse = m_Result.c_str();

	return ret;
}

// ____         ____                                          _ ___       _                        _
//|  _ \  ___  / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |_ _|_ __ | |_ ___ _ __ _ __   __ _| |
//| | | |/ _ \| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` || || '_ \| __/ _ \ '__| '_ \ / _` | |
//| |_| | (_) | |__| (_) | | | | | | | | | | | (_| | | | | (_| || || | | | ||  __/ |  | | | | (_| | |
//|____/ \___/ \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|___|_| |_|\__\___|_|  |_| |_|\__,_|_|
//
bool CommandLineInterface::DoCommandInternal(gSKI::IAgent* pAgent, const std::string& commandLine) {

	vector<string> argv;

	// Parse command:
	if (Tokenize(commandLine, argv) == -1) {
		return false;	// Parsing failed
	}

	return DoCommandInternal(pAgent, argv);
}

// ____         ____                                          _ ___       _                        _
//|  _ \  ___  / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |_ _|_ __ | |_ ___ _ __ _ __   __ _| |
//| | | |/ _ \| |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` || || '_ \| __/ _ \ '__| '_ \ / _` | |
//| |_| | (_) | |__| (_) | | | | | | | | | | | (_| | | | | (_| || || | | | ||  __/ |  | | | | (_| | |
//|____/ \___/ \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|___|_| |_|\__\___|_|  |_| |_|\__,_|_|
//
bool CommandLineInterface::DoCommandInternal(gSKI::IAgent* pAgent, vector<string>& argv) {
	if (!argv.size()) {
		// Nothing on command line!
		return true;
	}

	// Translate aliases
	m_Aliases.Translate(argv);

	// Is the command implemented?
	if (m_CommandMap.find(argv[0]) == m_CommandMap.end()) {
		HandleError("Command '" + argv[0] + "' not found or implemented.");
		return false;
	}

	// Check for help flags
	if (CheckForHelp(argv)) {
		// Help flags found, add help to line, return true
		string output;
		if (!m_Constants.IsUsageFileAvailable()) {
			AppendToResult(Constants::kCLINoUsageFile);
		} else if (m_Constants.GetUsageFor(argv[0], output)) {
			AppendToResult(output);
		} else {
			AppendToResult(Constants::kCLINoUsageInfo);
		}
		return true;
	}

	// Process command
	CommandFunction pFunction = m_CommandMap[argv[0]];

	// Just in case...
	if (!pFunction) {
		// Very odd, should be set in BuildCommandMap
		HandleError("Command found but function pointer is null.");
		return false;
	}
	
	// Make the call
	return (this->*pFunction)(pAgent, argv);
}

// _____     _              _
//|_   _|__ | | _____ _ __ (_)_______
//  | |/ _ \| |/ / _ \ '_ \| |_  / _ \
//  | | (_) |   <  __/ | | | |/ /  __/
//  |_|\___/|_|\_\___|_| |_|_/___\___|
//
int CommandLineInterface::Tokenize(string cmdline, vector<string>& argumentVector) {
	int argc = 0;
	string::iterator iter;
	string arg;
	bool quotes = false;
	int brackets = 0;

	for (;;) {

		// Is there anything to work with?
		if(cmdline.empty()) {
			break;
		}

		// Remove leading whitespace
		iter = cmdline.begin();
		while (isspace(*iter)) {
			cmdline.erase(iter);

			if (!cmdline.length()) {
				//Nothing but space left
				break;
			}
			
			// Next character
			iter = cmdline.begin();
		}

		// Was it actually trailing whitespace?
		if (!cmdline.length()) {
			// Nothing left to do
			break;
		}

		// We have an argument
		++argc;
		arg.clear();
		// Use space as a delimiter unless inside quotes or brackets (nestable)
		while (!isspace(*iter) || quotes || brackets) {
			// Eat quotes but leave brackets
			if (*iter == '"') {
				// Flip the quotes flag
				quotes = !quotes;

				// TODO: Eat quotes?  I used to but this screws up the sp command

			} else {
				if (*iter == '{') {
					++brackets;
				} else if (*iter == '}') {
					--brackets;
					if (brackets < 0) {
						HandleError("Closing bracket found without opening counterpart.");
						return -1;
					}
				}
			}

			// Add to argument (if we eat quotes, this has to be moved into the else above
			arg += (*iter);

			// Delete the character and move on on
			cmdline.erase(iter);
			iter = cmdline.begin();

			// Are we at the end of the string?
			if (iter == cmdline.end()) {

				// Did they close their quotes or brackets?
				if (quotes || brackets) {
					HandleError("No closing quotes/brackets found.");
					return -1;
				}
				break;
			}
		}

		// Store the arg
		argumentVector.push_back(arg);
	}

	// Return the number of args found
	return argc;
}

//  ____ _               _    _____          _   _      _
// / ___| |__   ___  ___| | _|  ___|__  _ __| | | | ___| |_ __
//| |   | '_ \ / _ \/ __| |/ / |_ / _ \| '__| |_| |/ _ \ | '_ \
//| |___| | | |  __/ (__|   <|  _| (_) | |  |  _  |  __/ | |_) |
// \____|_| |_|\___|\___|_|\_\_|  \___/|_|  |_| |_|\___|_| .__/
//                                                       |_|
bool CommandLineInterface::CheckForHelp(std::vector<std::string>& argv) {

	// Standard help check if there is more than one argument
	if (argv.size() > 1) {
		// Is one of the two help strings present?
		if (argv[1] == "-h" || argv[1] == "--help") {
			return true;
		}
	}
	return false;
}

//    _                               _ _____     ____                 _ _
//   / \   _ __  _ __   ___ _ __   __| |_   _|__ |  _ \ ___  ___ _   _| | |_
//  / _ \ | '_ \| '_ \ / _ \ '_ \ / _` | | |/ _ \| |_) / _ \/ __| | | | | __|
// / ___ \| |_) | |_) |  __/ | | | (_| | | | (_) |  _ <  __/\__ \ |_| | | |_
///_/   \_\ .__/| .__/ \___|_| |_|\__,_| |_|\___/|_| \_\___||___/\__,_|_|\__|
//        |_|   |_|
inline void CommandLineInterface::AppendToResult(const char* pMessage) {
	// Simply add to result
	m_Result += pMessage;
}
inline void CommandLineInterface::AppendToResult(const std::string& message) {
	// Simply add to result
	m_Result += message;
}
// TODO: Inlines wont work on these two for some reason...
void CommandLineInterface::AppendToResult(const bool boolean) {
	// Simply add to result
	m_Result += boolean;
}
void CommandLineInterface::AppendToResult(const char character) {
	// Simply add to result
	m_Result += character;
}

// ____       _   _  __                    _
/// ___|  ___| |_| |/ /___ _ __ _ __   ___| |
//\___ \ / _ \ __| ' // _ \ '__| '_ \ / _ \ |
// ___) |  __/ |_| . \  __/ |  | | | |  __/ |
//|____/ \___|\__|_|\_\___|_|  |_| |_|\___|_|
//
EXPORT void CommandLineInterface::SetKernel(gSKI::IKernel* pKernel) {
	m_pKernel = pKernel;
}

//  ____      _    ____                          _ __        __         _    _             ____  _               _
// / ___| ___| |_ / ___|   _ _ __ _ __ ___ _ __ | |\ \      / /__  _ __| | _(_)_ __   __ _|  _ \(_)_ __ ___  ___| |_ ___  _ __ _   _
//| |  _ / _ \ __| |  | | | | '__| '__/ _ \ '_ \| __\ \ /\ / / _ \| '__| |/ / | '_ \ / _` | | | | | '__/ _ \/ __| __/ _ \| '__| | | |
//| |_| |  __/ |_| |__| |_| | |  | | |  __/ | | | |_ \ V  V / (_) | |  |   <| | | | | (_| | |_| | | | |  __/ (__| || (_) | |  | |_| |
// \____|\___|\__|\____\__,_|_|  |_|  \___|_| |_|\__| \_/\_/ \___/|_|  |_|\_\_|_| |_|\__, |____/|_|_|  \___|\___|\__\___/|_|   \__, |
//                                                                                   |___/                                     |___/
bool CommandLineInterface::GetCurrentWorkingDirectory(string& directory) {
	// Pull an arbitrary buffer size of 1024 out of a hat and use it
	char buf[1024];
	char* ret = getcwd(buf, 1024);

	// If getcwd returns 0, that is bad
	if (!ret) {
		HandleError("Couldn't get working directory.");
		return false;
	}

	// Store directory in output parameter and return success
	directory = buf;
	return true;
}

// ___     ___       _
//|_ _|___|_ _|_ __ | |_ ___  __ _  ___ _ __
// | |/ __|| || '_ \| __/ _ \/ _` |/ _ \ '__|
// | |\__ \| || | | | ||  __/ (_| |  __/ |
//|___|___/___|_| |_|\__\___|\__, |\___|_|
//                           |___/
bool CommandLineInterface::IsInteger(const string& s) {
	string::const_iterator iter = s.begin();
	
	// Allow negatives
	if (s.length() > 1) {
		if (*iter == '-') {
			++iter;
		}
	}

	while (iter != s.end()) {
		if (!isdigit(*iter)) {
			return false;
		}
		++iter;
	}
	return true;
}

// _   _                 _ _      ____              _             _____
//| | | | __ _ _ __   __| | | ___/ ___| _   _ _ __ | |_ __ ___  _| ____|_ __ _ __ ___  _ __
//| |_| |/ _` | '_ \ / _` | |/ _ \___ \| | | | '_ \| __/ _` \ \/ /  _| | '__| '__/ _ \| '__|
//|  _  | (_| | | | | (_| | |  __/___) | |_| | | | | || (_| |>  <| |___| |  | | | (_) | |
//|_| |_|\__,_|_| |_|\__,_|_|\___|____/ \__, |_| |_|\__\__,_/_/\_\_____|_|  |_|  \___/|_|
//                                      |___/
bool CommandLineInterface::HandleSyntaxError(const char* command, const char* details) {
	string msg;
	msg += Constants::kCLISyntaxError;
	msg += " (";
	msg += command;
	msg += ")\n";
	if (details) {
		msg += details;
		msg += '\n';
	}
	msg += "Type 'help ";
	msg += command;
	msg += "' or '";
	msg += command;
	msg += " --help' for syntax and usage.";
	HandleError(msg);
	return false;
}

// ____                  _             _                    _
//|  _ \ ___  __ _ _   _(_)_ __ ___   / \   __ _  ___ _ __ | |_
//| |_) / _ \/ _` | | | | | '__/ _ \ / _ \ / _` |/ _ \ '_ \| __|
//|  _ <  __/ (_| | |_| | | | |  __// ___ \ (_| |  __/ | | | |_
//|_| \_\___|\__, |\__,_|_|_|  \___/_/   \_\__, |\___|_| |_|\__|
//              |_|                        |___/
bool CommandLineInterface::RequireAgent(gSKI::IAgent* pAgent) {
	if (!pAgent) {
		HandleError("An agent pointer is required for this command.");
		return false;
	}
	return true;
}

bool CommandLineInterface::RequireKernel() {
	if (!m_pKernel) {
		HandleError("A kernel pointer is required for this command.");
		return false;
	}
	return true;
}

// _   _                 _ _       ____      _    ___        _   _____
//| | | | __ _ _ __   __| | | ___ / ___| ___| |_ / _ \ _ __ | |_| ____|_ __ _ __ ___  _ __
//| |_| |/ _` | '_ \ / _` | |/ _ \ |  _ / _ \ __| | | | '_ \| __|  _| | '__| '__/ _ \| '__|
//|  _  | (_| | | | | (_| | |  __/ |_| |  __/ |_| |_| | |_) | |_| |___| |  | | | (_) | |
//|_| |_|\__,_|_| |_|\__,_|_|\___|\____|\___|\__|\___/| .__/ \__|_____|_|  |_|  \___/|_|
//                                                    |_|
bool CommandLineInterface::HandleGetOptError(char option) {
	string msg;
	msg += "Internal error: m_pGetOpt->GetOpt_Long returned '";
	msg += option;
	msg += "'!";
	HandleError(msg);
	return false;
}

// _   _                 _ _      _____
//| | | | __ _ _ __   __| | | ___| ____|_ __ _ __ ___  _ __
//| |_| |/ _` | '_ \ / _` | |/ _ \  _| | '__| '__/ _ \| '__|
//|  _  | (_| | | | | (_| | |  __/ |___| |  | | | (_) | |
//|_| |_|\__,_|_| |_|\__,_|_|\___|_____|_|  |_|  \___/|_|
//
bool CommandLineInterface::HandleError(std::string errorMessage, gSKI::Error* pError) {
	m_ErrorMessage += errorMessage;

	if (pError && isError(*pError)) {
		m_ErrorMessage += "\ngSKI error was: " ;
		m_ErrorMessage += pError->Text ;
		m_ErrorMessage += " details: " ;
		m_ErrorMessage += pError->ExtendedMsg ;
	}
	AppendToResult(m_ErrorMessage);

	// Always return false
	return false;
}

//    _                               _    _             _____
//   / \   _ __  _ __   ___ _ __   __| |  / \   _ __ __ |_   _|_ _  __ _
//  / _ \ | '_ \| '_ \ / _ \ '_ \ / _` | / _ \ | '__/ _` || |/ _` |/ _` |
// / ___ \| |_) | |_) |  __/ | | | (_| |/ ___ \| | | (_| || | (_| | (_| |
///_/   \_\ .__/| .__/ \___|_| |_|\__,_/_/   \_\_|  \__, ||_|\__,_|\__, |
//        |_|   |_|                                 |___/          |___/
void CommandLineInterface::AppendArgTag(const char* pParam, const char* pType, const char* pValue) {
	TagArg* pTag = new TagArg();
	pTag->SetParam(pParam);
	pTag->SetType(pType);
	pTag->SetValue(pValue);
	m_ResponseTags.push_back(pTag);
}

//    _                               _    _             _____           _____         _
//   / \   _ __  _ __   ___ _ __   __| |  / \   _ __ __ |_   _|_ _  __ _|  ___|_ _ ___| |_
//  / _ \ | '_ \| '_ \ / _ \ '_ \ / _` | / _ \ | '__/ _` || |/ _` |/ _` | |_ / _` / __| __|
// / ___ \| |_) | |_) |  __/ | | | (_| |/ ___ \| | | (_| || | (_| | (_| |  _| (_| \__ \ |_
///_/   \_\ .__/| .__/ \___|_| |_|\__,_/_/   \_\_|  \__, ||_|\__,_|\__, |_|  \__,_|___/\__|
//        |_|   |_|                                 |___/          |___/
void CommandLineInterface::AppendArgTagFast(const char* pParam, const char* pType, const char* pValue) {
	TagArg* pTag = new TagArg();
	pTag->SetParamFast(pParam);
	pTag->SetTypeFast(pType);
	pTag->SetValue(pValue);
	m_ResponseTags.push_back(pTag);
}

// ____                                _    _             _____           _____         _
//|  _ \ _ __ ___ _ __   ___ _ __   __| |  / \   _ __ __ |_   _|_ _  __ _|  ___|_ _ ___| |_
//| |_) | '__/ _ \ '_ \ / _ \ '_ \ / _` | / _ \ | '__/ _` || |/ _` |/ _` | |_ / _` / __| __|
//|  __/| | |  __/ |_) |  __/ | | | (_| |/ ___ \| | | (_| || | (_| | (_| |  _| (_| \__ \ |_
//|_|   |_|  \___| .__/ \___|_| |_|\__,_/_/   \_\_|  \__, ||_|\__,_|\__, |_|  \__,_|___/\__|
//               |_|                                 |___/          |___/
void CommandLineInterface::PrependArgTagFast(const char* pParam, const char* pType, const char* pValue) {
	TagArg* pTag = new TagArg();
	pTag->SetParamFast(pParam);
	pTag->SetTypeFast(pType);
	pTag->SetValue(pValue);
	m_ResponseTags.push_front(pTag);
}

