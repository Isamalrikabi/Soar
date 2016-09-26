#ifndef CLI_CLI_H
#define CLI_CLI_H

#include "sml_Events.h"
#include <bitset>
#include <stdint.h>
#include <vector>

typedef uint64_t epmem_time_id;

namespace cli
{
    class Cli
    {
        public:
            virtual ~Cli() {}

            virtual bool SetError(const std::string& error) = 0;
            virtual bool AppendError(const std::string& error) = 0;

            /**
             * @brief add-wme command
             * @param id Id string for the new wme
             * @param attribute Attribute string for the new wme
             * @param value Value string for the new wme
             * @param acceptable True to give wme acceptable preference
             */
            virtual bool DoAddWME(const std::string& id, std::string attribute, const std::string& value, bool acceptable) = 0;

            /**
             * @brief alias command
             * @param argv Alias command arguments with the command name removed.
             *        Ex: { "stop", "stop-soar" }
             *        Null to list.
             */
            virtual bool DoAlias(std::vector< std::string >* argv = 0) = 0;

            virtual bool DoAllocate(const std::string& pool, int blocks) = 0;

            enum eCaptureInputMode
            {
                CAPTURE_INPUT_OPEN,
                CAPTURE_INPUT_QUERY,
                CAPTURE_INPUT_CLOSE,
            };

            /**
             * @brief capture-input command
             */
            virtual bool DoCaptureInput(eCaptureInputMode mode, bool autoflush = false, std::string* pathname = 0) = 0;


           /**
             * @brief chunk command
             * @param options The various options set on the command line
             */
            virtual bool DoChunk(const char pOp = 0, const std::string* pAttr = 0, const std::string* pVal = 0) = 0;

            /**
             * @brief cd command
             * @param pDirectory Pointer to the directory to pass in to. Pass null to return
             *        to the initial (home) directory.
             */
            virtual bool DoCD(const std::string* pDirectory = 0) = 0;

            enum eLogMode
            {
                LOG_QUERY,
                LOG_NEW,
                LOG_NEWAPPEND,
                LOG_CLOSE,
                LOG_ADD,
            };

            /**
             * @brief clog command
             * @param mode The mode for the log command
             * @param pFilename The log filename, pass 0 (null) if not applicable to mode
             * @param pToAdd The string to add to the log, pass 0 (null) if not applicable to mode
             * @param silent Supress query messages (log file open/closed).
             */
            virtual bool DoCLog(const eLogMode mode = LOG_QUERY, const std::string* pFilename = 0, const std::string* pToAdd = 0, bool silent = false) = 0;

            virtual bool DoCommandToFile(const eLogMode mode, const std::string& filename, std::vector< std::string >& argv) = 0;

            /**
             * @brief debug command
             *
             */
            virtual bool DoDebug(std::vector< std::string >* argv = 0) = 0;

            /**
             * @brief dirs command
             */
            virtual bool DoDirs() = 0;

            /**
             * @brief echo command
             * @param argv The args to echo
             * @param echoNewline True means a newline character will be appended to string
             */
            virtual bool DoEcho(const std::vector<std::string>& argv, bool echoNewline) = 0;

            /**
             * @brief edit-production command
             * @param productionName Production to edit
             */
            virtual bool DoEpMem(const char pOp = 0, const std::string* pAttr = 0, const std::string* pVal = 0, const epmem_time_id memory_id = 0) = 0;

            enum eExciseOptions
            {
                EXCISE_ALL,
                EXCISE_CHUNKS,
                EXCISE_DEFAULT,
                EXCISE_RL,
                EXCISE_TASK,
                EXCISE_TEMPLATE,
                EXCISE_USER,
                EXCISE_NEVER_FIRED,
                EXCISE_NUM_OPTIONS, // must be last
            };
            typedef std::bitset<EXCISE_NUM_OPTIONS> ExciseBitset;

            /**
             * @brief excise command
             * @param options The various options set on the command line
             * @param pProduction A production to excise, optional
             */
            virtual bool DoExcise(const ExciseBitset& options, const std::string* pProduction = 0) = 0;

            enum eExplainOptions
            {
                EXPLAIN_ALL,
                EXPLAIN_CONSTRAINTS,
                EXPLAIN_FORMATION,
                EXPLAIN_EXPLANATION_TRACE,
                EXPLAIN_GLOBAL_STATS,
                EXPLAIN_IDENTITY_SETS,
                EXPLAIN_LIST_ALL,
                EXPLAIN_ONLY_SPECIFIC,
                EXPLAIN_JUSTIFICATIONS,
                EXPLAIN_RECORD,
                EXPLAIN_STATS,
                EXPLAIN_WME_TRACE,
                EXPLAIN_NUM_OPTIONS, // must be last
            };
            typedef std::bitset<EXPLAIN_NUM_OPTIONS> ExplainBitset;
            /**
             * @brief explain command
             * @param pProduction Pointer to involved production. Pass 0 (null) for
             *        query
             * @param condition A number representing the condition number to explain,
             *        0 for production name, -1 for full,
             *        this argument ignored if pProduction is 0 (null)
             */
            virtual bool DoExplain(ExplainBitset options, const std::string* pObject, const std::string* pObject2) = 0;

            /* These enums moved here because we re-used for fc options*/
            enum ePrintOptions
            {
                PRINT_ALL,
                PRINT_CHUNKS,
                PRINT_DEPTH,
                PRINT_DEFAULTS,
                PRINT_FULL,
                PRINT_FILENAME,
                PRINT_INTERNAL,
                PRINT_TREE,
                PRINT_JUSTIFICATIONS,
                PRINT_NAME,
                PRINT_OPERATORS,
                PRINT_RL,
                PRINT_STACK,
                PRINT_STATES,
                PRINT_TEMPLATE,
                PRINT_USER,
                PRINT_VARPRINT,
                PRINT_EXACT,
                PRINT_NUM_OPTIONS, // must be last
            };
            typedef std::bitset<PRINT_NUM_OPTIONS> PrintBitset;
            /**
             * @brief firing-counts command
             * @param numberToList The number of top-firing productions to list.
             *        Use 0 to list those that haven't fired. -1 lists all
             * @param pProduction The specific production to list, pass 0 (null) to list
             *        multiple productions
             */
            virtual bool DoFiringCounts(PrintBitset options, const int numberToList = -1, const std::string* pProduction = 0) = 0;

            /**
             * @brief gds-print command
             */
            virtual bool DoGDSPrint() = 0;

            /**
             * @brief gp command
             * @param productionString The general soar production to generate more productions to load to memory
             */
            virtual bool DoGP(const std::string& productionString) = 0;

            /**
             * @brief help command
             */
            virtual bool DoHelp(const std::vector<std::string>& argv) = 0;

            /**
             * @brief indifferent-selection command
             * @param pOp The operation to perform, pass 0 if unnecssary
             * @param p1 First parameter, pass 0 (null) if unnecessary
             * @param p2 Second parameter, pass 0 (null) if unnecessary
             * @param p3 Third parameter, pass 0 (null) if unnecessary
             */
            virtual bool DoIndifferentSelection(const char pOp = 0, const std::string* p1 = 0, const std::string* p2 = 0, const std::string* p3 = 0) = 0;

            enum eLearnOptions
            {
                LEARN_ALL_LEVELS,
                LEARN_BOTTOM_UP,
                LEARN_DISABLE,
                LEARN_ENABLE,
                LEARN_EXCEPT,
                LEARN_LIST,
                LEARN_ONLY,
                LEARN_ENABLE_THROUGH_LOCAL_NEGATIONS,
                LEARN_DISABLE_THROUGH_LOCAL_NEGATIONS,
                LEARN_ENABLE_THROUGH_EVALUATION_RULES,
                LEARN_DISABLE_THROUGH_EVALUATION_RULES,
                LEARN_NUM_OPTIONS, // must be last
            };
            typedef std::bitset<LEARN_NUM_OPTIONS> LearnBitset;

           /**
             * @brief learn command
             * @param options The various options set on the command line
             */
            virtual bool DoLearn(const LearnBitset& options) = 0;

            /**
             * @brief load-library command
             * @param libraryCommand The name of the library to load
             * WITHOUT the .so/.dll/etc plus its arguments.
             */
            virtual bool DoLoadLibrary(const std::string& libraryCommand) = 0;

            /**
             * @brief ls command
             */
            virtual bool DoLS() = 0;

            enum eMatchesMode
            {
                MATCHES_PRODUCTION,
                MATCHES_ASSERTIONS,
                MATCHES_RETRACTIONS,
                MATCHES_ASSERTIONS_RETRACTIONS,
            };

            enum eWMEDetail
            {
                WME_DETAIL_NONE,
                WME_DETAIL_TIMETAG,
                WME_DETAIL_FULL,
            };

            /**
             * @brief matches command
             * @param mode The mode for the command
             * @param detail The WME detail
             * @param pProduction The production, pass 0 (null) if not applicable to mode
             */
            virtual bool DoMatches(const eMatchesMode mode, const eWMEDetail detail = WME_DETAIL_NONE, const std::string* pProduction = 0) = 0;

            enum eMemoriesOptions
            {
                MEMORIES_CHUNKS,
                MEMORIES_DEFAULT,
                MEMORIES_JUSTIFICATIONS,
                MEMORIES_TEMPLATES,
                MEMORIES_USER,
                MEMORIES_NUM_OPTIONS, // must be last
            };
            typedef std::bitset<MEMORIES_NUM_OPTIONS> MemoriesBitset;

            /**
             * @brief memories command
             * @param options Options for the memories flag
             * @param n number of productions to print sorted by most memory use, use 0 for all
             * @param pProduction specific production to print, ignored if any
             *        options are set, pass 0 (null) if not applicable
             */
            virtual bool DoMemories(const MemoriesBitset options, int n = 0, const std::string* pProduction = 0) = 0;

            /**
             * @brief multi-attributes command
             * @param pAttribute The attribute, pass 0 (null) for query
             * @param n The count, pass 0 (null) for query if pAttribute is also null,
             *        otherwise this will default to 10
             */
            virtual bool DoMultiAttributes(const std::string* pAttribute = 0, int n = 0) = 0;

            /**
             * @brief numeric-indifferent mode command
             * @param query true to query
             * @param mode The new mode, ignored on query
             */

            virtual bool DoNumericIndifferentMode(bool query, bool usesAvgNIM) = 0;

            /**
             * @brief output command
             * @param options The various options set on the command line
             */
            virtual bool DoOutput(const char pOp = 0, const std::string* pAttr = 0, const std::string* pVal = 0) = 0;

            /**
             * @brief pbreak command
             * @param pProduction The production
             */
            virtual bool DoPbreak(const char& mode, const std::string& production) = 0;

            /**
             * @brief popd command
             */
            virtual bool DoPopD() = 0;

            /**
             * @brief predict command
             */
            virtual bool DoPredict() = 0;

            enum ePreferencesDetail
            {
                PREFERENCES_ONLY,
                PREFERENCES_NAMES,
                PREFERENCES_TIMETAGS,
                PREFERENCES_WMES,
            };

            /**
             * @brief preferences command
             * @param detail The preferences detail level
             * @param pId An existing soar identifier or 0 (null)
             * @param pAttribute An existing soar attribute of the specified identifier or 0 (null)
             */
            virtual bool DoPreferences(const ePreferencesDetail detail, const bool object, const std::string* pId = 0, const std::string* pAttribute = 0) = 0;

            /**
             * @brief print command
             * @param options The options to the print command
             * @param depth WME depth
             * @param pArg The identifier/timetag/pattern/production name to print,
             *        or 0 (null) if not applicable
             */
            virtual bool DoPrint(PrintBitset options, int depth, const std::string* pArg = 0) = 0;

            enum eProductionFindOptions
            {
                PRODUCTION_FIND_INCLUDE_LHS,
                PRODUCTION_FIND_INCLUDE_RHS,
                PRODUCTION_FIND_ONLY_CHUNKS,
                PRODUCTION_FIND_NO_CHUNKS,
                PRODUCTION_FIND_SHOWBINDINGS,
                PRODUCTION_FIND_NUM_OPTIONS, // must be last
            };
            typedef std::bitset<PRODUCTION_FIND_NUM_OPTIONS> ProductionFindBitset;

            /**
             * @brief production-find command
             * @param options The options to the command
             * @param pattern Any pattern that can appear in productions.
             */
            virtual bool DoProductionFind(const ProductionFindBitset& options, const std::string& pattern) = 0;

            /**
             * @brief pushd command
             * @param directory The directory to change to
             */
            virtual bool DoPushD(const std::string& directory) = 0;

            /**
             * @brief pwatch command
             * @param query Pass true to query, all other args ignored
             * @param pProduction The production to watch or stop watching, pass 0 (null)
             *        to disable watching of all productions (setting ignored)
             * @param setting True to watch the pProduction, false to stop watching it
             */
            virtual bool DoPWatch(bool query = true, const std::string* pProduction = 0, bool setting = false) = 0;

            /**
             * @brief pwd command
             */
            virtual bool DoPWD() = 0;

            /**
             * @brief rand command
             */
            virtual bool DoRand(bool integer, std::string* bound) = 0;

            /**
             * @brief remove-wme command
             * @param timetag The timetag of the wme to remove
             */
            virtual bool DoRemoveWME(uint64_t timetag) = 0;

            enum eReplayInputMode
            {
                REPLAY_INPUT_OPEN,
                REPLAY_INPUT_QUERY,
                REPLAY_INPUT_CLOSE,
            };

            /**
             * @brief replay-input command
             */
            virtual bool DoReplayInput(eReplayInputMode mode, std::string* pathname) = 0;

            /**
             * @brief rete-net command
             * @param save true to save, false to load
             * @param filename the rete-net file
             */
            virtual bool DoReteNet(bool save, std::string filename) = 0;

            /**
             * @brief rl command
             * @param pOp the rl switch to implement, pass 0 (null) for full parameter configuration
             * @param pAttr the attribute to get/set/stats, pass 0 (null) only if no pOp (all config) or stats (full stats)
             * @param pVal the value to set, pass 0 (null) only if no pOp (all config), get, or stats
             */
            virtual bool DoRL(const char pOp = 0, const std::string* pAttr = 0, const std::string* pVal = 0) = 0;

            virtual bool DoSVS(const std::vector<std::string>& args) = 0;
            enum eRunOptions
            {
                RUN_DECISION,
                RUN_ELABORATION,
                RUN_FOREVER,
                RUN_INTERLEAVE,
                RUN_OUTPUT,
                RUN_PHASE,
                RUN_SELF,
                RUN_UPDATE,
                RUN_NO_UPDATE,
                RUN_GOAL,
                RUN_NUM_OPTIONS, // must be last
            };
            typedef std::bitset<RUN_NUM_OPTIONS> RunBitset;

            enum eRunInterleaveMode
            {
                RUN_INTERLEAVE_DEFAULT,
                RUN_INTERLEAVE_ELABORATION,
                RUN_INTERLEAVE_PHASE,
                RUN_INTERLEAVE_DECISION,
                RUN_INTERLEAVE_OUTPUT,
            };

            /**
             * @brief run command
             * @param options Options for the run command
             * @param count The count, units or applicability depends on options
             * @param interleave Support for round robin execution across agents
             *         at a finer grain than the run-size parameter.
             */
            virtual bool DoRun(const RunBitset& options, int count = 0, eRunInterleaveMode interleave = RUN_INTERLEAVE_DEFAULT) = 0;

            /**
             * @brief select command
             * @param pAgent The pointer to the gSKI agent interface
             * @param setting The new setting, pass 0 (null) for query
             */
            virtual bool DoSelect(const std::string* pOp = 0) = 0;

            /**
             * @brief smem command
             * @param pOp the smem switch to implement, pass 0 (null) for full parameter configuration
             * @param pAttr the attribute to get/set/stats, pass 0 (null) only if no pOp (all config) or stats (full stats)
             * @param pVal the value to set, pass 0 (null) only if no pOp (all config), get, or stats
             */
            virtual bool DoSMem(const char pOp = 0, const std::string* pArg1 = 0, const std::string* pArg2 = 0, const std::string* pArg3 = 0) = 0;

            /**
             * @brief soar command
             * @param options The various options set on the command line
             */
            virtual bool DoSoar(const char pOp = 0, const std::string* pAttr = 0, const std::string* pVal = 0) = 0;

            enum eSourceOptions
            {
                SOURCE_ALL,
                SOURCE_DISABLE,
                SOURCE_VERBOSE,
                SOURCE_NUM_OPTIONS,    // must be last
            };
            typedef std::bitset<SOURCE_NUM_OPTIONS> SourceBitset;

            /**
             * @brief source command
             * @param filename The file to source
             */
            virtual bool DoSource(std::string filename, SourceBitset* pOptions = 0) = 0;

            /**
             * @brief sp command
             * @param production The production to add to working memory
             */
            virtual bool DoSP(const std::string& production) = 0;

            /**
             * @brief srand command
             * @param pSeed Number to seed the random number generator with, pass
             *         null to seed randomly.
             */
            virtual bool DoSRand(uint32_t* pSeed = 0) = 0;

            enum eStatsOptions
            {
                STATS_MEMORY,
                STATS_RETE,
                STATS_SYSTEM,
                STATS_MAX,
                STATS_RESET,
                STATS_TRACK,
                STATS_CYCLE,
                STATS_CSV,
                STATS_STOP_TRACK,
                STATS_DECISION,
                STATS_AGENT,
                STATS_NUM_OPTIONS, // must be last
            };
            typedef std::bitset<STATS_NUM_OPTIONS> StatsBitset;

            /**
             * @brief stats command
             * @param options The options for the stats command
             * @param sort The column to sort by
             */
            virtual bool DoStats(const StatsBitset& options, int sort = 0) = 0;

            /**
             * @brief stop-soar command
             * @param self Stop the only pAgent (false means stop all agents in kernel)
             * @param reasonForStopping optional reason for stopping
             */
            virtual bool DoStopSoar(bool self, const std::string* reasonForStopping = 0) = 0;

            /**
             * @brief tcl command
             */
            virtual bool DoTclCommand(const std::string& pMessage) = 0;

            /**
             * @brief time command
             * @param argv The command line with the time arg removed
             */
            virtual bool DoTime(std::vector<std::string>& argv) = 0;

            /**
             * @brief timers command
             * @param pSetting The timers setting, true to turn on, false to turn off,
             *        pass 0 (null) to query
             */
            virtual bool DoTimers(bool* pSetting = 0) = 0;

            virtual bool DoUnalias(std::vector<std::string>& argv) = 0;

            /**
             * @brief version command
             */
            virtual bool DoVersion() = 0;

            /**
             * @brief visualize command
             */
            virtual bool DoVisualize(const std::string* pArg = 0, const std::string* pArg2 = 0, const std::string* pArg3 = 0) = 0;

            enum eWatchOptions
            {
                WATCH_DECISIONS,
                WATCH_PHASES,
                WATCH_DEFAULT,
                WATCH_USER,
                WATCH_CHUNKS,
                WATCH_JUSTIFICATIONS,
                WATCH_TEMPLATES,
                WATCH_WMES,
                WATCH_PREFERENCES,
                WATCH_WME_DETAIL,
                WATCH_LEARNING,
                WATCH_BACKTRACING,
                WATCH_INDIFFERENT,
                WATCH_RL,
                WATCH_WATERFALL,
                WATCH_EPMEM,
                WATCH_SMEM,
                WATCH_WMA,
                WATCH_GDS,
                WATCH_NUM_OPTIONS, // must be last
            };
            typedef std::bitset<WATCH_NUM_OPTIONS> WatchBitset;

            /**
             * @brief watch command
             * @param options Options for the watch command
             * @param settings Settings for the watch command, if a flag (option) is set, its
             *        setting is set using this (true/on or false/off)
             * @param wmeSetting Setting for wme detail, not binary so it has its own arg
             * @param learnSetting Setting for learn level, not binary so it has its own arg
             */
            virtual bool DoWatch(const WatchBitset& options, const WatchBitset& settings, const int wmeSetting, const int learnSetting) = 0;

            enum eWatchWMEsMode
            {
                WATCH_WMES_ADD,
                WATCH_WMES_REMOVE,
                WATCH_WMES_LIST,
                WATCH_WMES_RESET,
            };

            enum eWatchWMEsOptions
            {
                WATCH_WMES_TYPE_ADDS,
                WATCH_WMES_TYPE_REMOVES,
                WATCH_WMES_TYPE_NUM_OPTIONS, // must be last
            };
            typedef std::bitset<WATCH_WMES_TYPE_NUM_OPTIONS> WatchWMEsTypeBitset;

            /**
             * @brief watch-wmes command
             */
            virtual bool DoWatchWMEs(const eWatchWMEsMode mode, WatchWMEsTypeBitset type, const std::string* pIdString = 0, const std::string* pAttributeString = 0, const std::string* pValueString = 0) = 0;

            /**
             * @brief wma command
             * @param pOp the wma switch to implement, pass 0 (null) for full parameter configuration
             * @param pAttr the attribute to get/set/stats, pass 0 (null) only if no pOp (all config) or stats (full stats)
             * @param pVal the value to set, pass 0 (null) only if no pOp (all config), get, or stats
             */
            virtual bool DoWMA(const char pOp = 0, const std::string* pAttr = 0, const std::string* pVal = 0) = 0;

    };
}

#endif // CLI_CLI_H
