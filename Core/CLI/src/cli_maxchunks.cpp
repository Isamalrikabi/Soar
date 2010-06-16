/////////////////////////////////////////////////////////////////
// max-chunks command file.
//
// Author: Jonathan Voigt, voigtjr@gmail.com
// Date  : 2004
//
/////////////////////////////////////////////////////////////////

#include <portability.h>

#include "cli_CommandLineInterface.h"

#include "cli_Commands.h"
#include "sml_Names.h"
#include "cli_CLIError.h"

#include "agent.h"

using namespace cli;
using namespace sml;

bool CommandLineInterface::ParseMaxChunks(std::vector<std::string>& argv) {

	// n defaults to 0 (print current value)
	int n = 0;

	if (argv.size() > 2) return SetError(CLIError::kTooManyArgs);

	// one argument, figure out if it is a positive integer
	if (argv.size() == 2) {
		from_string(n, argv[1]);
		if (n <= 0) return SetError(CLIError::kIntegerMustBePositive);
	}

	return DoMaxChunks(n);
}

bool CommandLineInterface::DoMaxChunks(const int n) {
	if (!n) {
		// query
		if (m_RawOutput) {
			m_Result << m_pAgentSoar->sysparams[MAX_CHUNKS_SYSPARAM];
		} else {
			std::string temp;
			AppendArgTagFast(sml_Names::kParamValue, sml_Names::kTypeInt, to_string(m_pAgentSoar->sysparams[MAX_CHUNKS_SYSPARAM], temp));
		}
		return true;
	}

	m_pAgentSoar->sysparams[MAX_CHUNKS_SYSPARAM] = n;
	return true;
}
