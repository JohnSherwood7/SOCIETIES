/**
 * Parse the command lines arguments the user enters when starting running the program
 * and the values of variables in config file.
 * For example, type "./societies -h" or "./societies --help" will show the instruction on how to run the program.
 * -p / --parameter: to specify the config file.
 * -z / --heterogeneous: to specify the agent config file (aconf file).
 *                       When the aconf file ends in .csv, it is set to heterogeneous and will use the csv file to generate .aconf file correspondingly.
 *                       Otherwise, it is set to homogeneous (no matter what NUM_GROUPS is) and use will use the aconf file to initialize agents.
 *                       If this option is not set at all, it will generate a homogeneous .aconf file based on the config file (.conf).
 */
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <stdlib.h>

#include "globals.h"
#include "logging.h"

using namespace std;
namespace po = boost::program_options;

/**
 * Evaluate the given arguments.
 * Throw an exception and stop when entering an arg of wrong type or wrong number of args.
 * All options will set to their default values if not specified in argc.
 * \param argc the number of arguments
 * \param argv an array of char*, the whole argument
 */
int parse_args(int argc, char **argv)
{
    vector<string> databaseArg;
    vector<int> remAgentArg;
    vector<int> remResArg;
    vector<string> saveInMiddleArg;
    try {
        /* parse the command line args */
        po::options_description cmdLineOpts("Allowed options");
        cmdLineOpts.add_options()
            ("help,h", "produce help message")
            ("parameter,p", po::value<string>(), "use given file for global variable values")
            ("heterogeneous,z", po::value<string>(), "use the given file for agent values. If the file ends with .csv, then an .aconf file will by generated and used for agent values.")
            ("save,s", po::value<string>(), "write results to folder")	// no default value
            ("database,d", po::value< vector<string> >(&databaseArg)->multitoken(), "write results to 4 csv files which are imported into database (First arg: a UUID parsed for batch ID; 2nd arg: a UUID parsed for runID; 3rd arg: the nth iteration in this batch; 4th arg: heteroID--includes RPT, DTP, GS, DS, IS; separated by /; 5th arg: the directory to be saved in)")
            ("seed,S", po::value<int>(), "initialize random number generator to given seed so that output is same for each run")
            ("title,t", po::value<string>()->default_value("default"), "give the simulation a title")
            ("graph,g", "allow choice of graphs to generate upon completion")
            ("norun,n", "load and print the current config values, then exit")
            ("agent,a", po::value< vector<int> >(&remAgentArg)->multitoken(), "remove an agent mid-run (first arg: which agent to remove, second arg: which day to remove the agent)")
            ("resource,r", po::value< vector<int> >(&remResArg)->multitoken(), "remove a resource mid-run (first arg: which resource to remove, second arg: which day to remove the resource, third arg: 1 for removing the holdings of that resource on the given day, 0 for not)")
            ("middle,m", po::value< vector<string> >(&saveInMiddleArg)->multitoken(), "save the simulation mid-run (first arg: destination folder, second arg: day on which to save)")
            ("verbose,v", po::value<int>(), "set level of debugging output from 0 to 3.  0 = nothing; 3 = everything")
            // NOTE: not adding --parallel now
            ("exchange,e", "store exchange rate data from all trades")
            ;

        /* parse the config file values */
        po::options_description cfgFileOpts("Config file options");
        cfgFileOpts.add_options()
            ("START_DAY",       po::value<int>(&glob.START_DAY),  "the day to start on when restarting a simulation")
            ("DAY_LENGTH",      po::value<int>(&glob.DAY_LENGTH), "the length of a day")
            ("NUM_DAYS",        po::value<int>(&glob.NUM_DAYS), "number of days in the simulation")
            ("NUM_AGENTS",      po::value<int>(&glob.NUM_AGENTS), "number of agents in the simulation")
            ("NUM_RESOURCES",   po::value<int>(&glob.NUM_RESOURCES), "number of resources in the simulation")
            ("NUM_AGENT_GROUPS",po::value<int>(&glob.NUM_AGENT_GROUPS), "number of types/groups of agents in the simulation")
            ("MAX_RES_AMOUNT",  po::value<long int>(&glob.MAX_RES_AMOUNT), "the max amount of resources allowed in the environment")
            ("MIN_RES_AMOUNT",  po::value<int>(&glob.MIN_RES_AMOUNT), "the min amount of resources allowed in the environment")
            ("RES_CONSIDERED_DEPLETED",  po::value<double>(&glob.RES_CONSIDERED_DEPLETED), "multiplied by MIN_RES_AMOUNT")
            ("RES_CONSIDERED_RENEWED",  po::value<double>(&glob.RES_CONSIDERED_RENEWED), "multiplied by initial amount")
            ("TAX_RATE",        po::value<double>(&glob.TAX_RATE), "the rate of a proportional tax each agent needs to pay")
            ("RES_TRADE_ROUNDS", po::value<int>(&glob.RES_TRADE_ROUNDS), "number of resource trading rounds")
            ("RES_TRADE_ATTEMPTS", po::value<int>(&glob.RES_TRADE_ATTEMPTS), "number of resource trading attempts")
            ("DEVICE_TRADE_ROUNDS", po::value<int>(&glob.DEVICE_TRADE_ROUNDS), "number of device trading rounds")
            ("DEVICE_TRADE_ATTEMPTS", po::value<int>(&glob.DEVICE_TRADE_ATTEMPTS), "number of device trading attempts")
            ("MENU_SIZE",       po::value<int>(&glob.MENU_SIZE), "Menu size: fix this!")
            ("DEVICE_TRADE_MEMORY_LENGTH", po::value<int>(&glob.DEVICE_TRADE_MEMORY_LENGTH), "HELP")
            ("DEVICE_PRODUCTION_MEMORY_LENGTH", po::value<int>(&glob.DEVICE_PRODUCTION_MEMORY_LENGTH), "HELP")
            ("MIN_DEVICE_FOR_DEV_DEVICE_CONSIDERATION", po::value<int>(&glob.MIN_DEVICE_FOR_DEV_DEVICE_CONSIDERATION), "HELP")
            ("MIN_RES_HELD_FOR_DEVICE_CONSIDERATION", po::value<int>(&glob.MIN_RES_HELD_FOR_DEVICE_CONSIDERATION), "HELP")
            ("DAILY_EXP_PENALTY", po::value<double>(&glob.DAILY_EXP_PENALTY), "HELP")
            ("PRODUCTION_EPSILON", po::value<double>(&glob.PRODUCTION_EPSILON), "HELP")
            ("RESOURCES_IN_TOOL",  po::value<int>(&glob.RESOURCES_IN_TOOL), "number of resources needed to make a tool (?)")
            ("INVENTOR_DEVICE_EXPERIENCE",  po::value<double>(&glob.INVENTOR_DEVICE_EXPERIENCE), "HELP")
            ("NUM_DEVICE_COMPONENTS",  po::value<int>(&glob.NUM_DEVICE_COMPONENTS), "HELP")
            ("DAILY_DEVICE_DECAY",  po::value<double>(&glob.DAILY_DEVICE_DECAY), "HELP")
            ("MIN_HELD_DEVICE_EXPERIENCE", po::value<double>(&glob.MIN_HELD_DEVICE_EXPERIENCE), "HELP")
            ("MIN_RES_UTIL", po::value<double>(&glob.MIN_RES_UTIL), "HELP")
            ("TRADE_EPSILON", po::value<double>(&glob.TRADE_EPSILON), "HELP")
            ("TOOL_PROBABILITY_FACTOR", po::value<double>(&glob.TOOL_PROBABILITY_FACTOR), "HELP")
            ("DEVICE_PROBABILITY_FACTOR", po::value<double>(&glob.DEVICE_PROBABILITY_FACTOR), "HELP")
            ("TOOL_FACTOR", po::value<double>(&glob.TOOL_FACTOR), "HELP")
            ("TOOL_LIFETIME", po::value<double>(&glob.TOOL_LIFETIME), "HELP")
            ("MACHINE_FACTOR", po::value<double>(&glob.MACHINE_FACTOR), "HELP")
            ("MACHINE_LIFETIME", po::value<double>(&glob.MACHINE_LIFETIME), "HELP")
            ("FACTORY_FACTOR", po::value<double>(&glob.FACTORY_FACTOR), "HELP")
            ("FACTORY_LIFETIME", po::value<double>(&glob.FACTORY_LIFETIME), "HELP")
            ("INDUSTRY_FACTOR", po::value<double>(&glob.INDUSTRY_FACTOR), "HELP")
            ("INDUSTRY_LIFETIME", po::value<double>(&glob.INDUSTRY_LIFETIME), "HELP")
            ("DEV_MACHINE_FACTOR", po::value<double>(&glob.DEV_MACHINE_FACTOR), "HELP")
            ("DEV_MACHINE_LIFETIME", po::value<double>(&glob.DEV_MACHINE_LIFETIME), "HELP")
            ("DEV_FACTORY_FACTOR", po::value<double>(&glob.DEV_FACTORY_FACTOR), "HELP")
            ("DEV_FACTORY_LIFETIME", po::value<double>(&glob.DEV_FACTORY_LIFETIME), "HELP")
            ("DAYS_OF_DEVICE_TO_HOLD", po::value<double>(&glob.DAYS_OF_DEVICE_TO_HOLD), "HELP")
            ("TRADE_EXISTS", po::value<bool>(&glob.TRADE_EXISTS), "HELP")
            ("DEVICES_EXIST", po::value<bool>(&glob.DEVICES_EXIST), "HELP")
            ("TOOLS_ONLY", po::value<bool>(&glob.TOOLS_ONLY), "HELP")
            ;

        cmdLineOpts.add(cfgFileOpts);

       /*
        * If the paramFile (config file) the user enters is found, set glob.configFilename to it.
        * If it's not found, set it to a defaultValues.conf.
        */
        bool foundParamFile = false;
        for (int a = 0; a < argc; a++) {
            if (strcmp(argv[a], "--parameter") == 0 || strcmp(argv[a], "-p") == 0) {
                glob.configFilename = argv[++a];
                foundParamFile = true;
                break;
            }
        }
        if (! foundParamFile) {
            glob.configFilename = "../conf/defaultValues.conf";  //!< default value for config file
        }

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, cmdLineOpts), vm);
        // true means there can be lines in the config file we don't know about.
        po::store(po::parse_config_file<char>(glob.configFilename.c_str(), cfgFileOpts, true), vm);
        po::notify(vm);

        /*
         * If the user enters -h or --help as arg, print the help info and stops running.
         */
        if (vm.count("help")) {
            cout << cmdLineOpts << "\n";
            return 0;
        }

        if (vm.count("heterogeneous")) {
            glob.configAgentFilename = vm["heterogeneous"].as<string>();
            glob.configAgentCSV = "";
            /*
             * If the agent config file that the user enters is not found, stop running.
             */
            if ( ! boost::filesystem::exists(glob.configAgentFilename)) {
                cout << "Agent file does not exist;" << endl;
                return 0;
            }
            /*
             * If the agent config file the user enters ends in .csv,
             * call hetero_gen_config.py to generate an .aconf file based on the .csv file.
             */
            if (glob.configAgentFilename.substr(glob.configAgentFilename.size()-4, 4) == ".csv") {
                int command;
                string commandLine = "python ../conf/hetero_gen_config.py "
                                    + glob.configAgentFilename + " " + glob.configFilename;
                command = system(commandLine.c_str());
                glob.configAgentCSV = glob.configAgentFilename;
                glob.configAgentFilename = glob.configAgentFilename.substr(0, glob.configAgentFilename.size()-4) + ".aconf";
            } else {
                cout << "Agent file needs to be a csv file." << endl;
                return 0;
            }
        } else {
            glob.configAgentCSV = "../conf/AgentTypes.csv";
            int command;
            string commandLine = "python ../conf/hetero_gen_config.py "
                                + glob.configAgentCSV + " " + glob.configFilename;
            command = system(commandLine.c_str());
            glob.configAgentFilename = "../conf/AgentTypes.aconf";
        }
        if (vm.count("save")) {
            glob.saveFileFolderSet = true;
            glob.saveFileFolder = vm["save"].as<string>();
        } else {
            glob.saveFileFolderSet = false;
        }
        if (vm.count("database")) {
            glob.saveDatabaseSet = true;
            if (databaseArg.size() == 5) {
                glob.batchID = databaseArg[0];
                glob.iterationID = databaseArg[1];
                glob.iterationNum = boost::lexical_cast<int>(databaseArg[2]);
                glob.heteroID = databaseArg[3];
                glob.saveDatabaseFolder = databaseArg[4];
            } else {
                cout << "Wrong number of arguments. Should be five." << endl;
            }
        } else {
            glob.saveDatabaseSet = false;
        }
        if (vm.count("seed")) {
            glob.randomSeedSet = true;
            glob.randomSeed = vm["seed"].as<int>();
        } else {
            glob.randomSeedSet = false;
        }
        if (vm.count("title")) {
            glob.simTitle = vm["title"].as<string>();
        }
        if (vm.count("graph")) {
            glob.graphSet = true;
        } else {
            glob.graphSet = false;
        }
        if (vm.count("norun")) {
            glob.noRunSet = true;
        } else {
            glob.noRunSet = false;
        }
        if (vm.count("agent")) {
            glob.removeAgentMidRun = true;
            if (remAgentArg.size() == 2) {
                glob.removeAgentId = remAgentArg[0];
                glob.removeAgentDay = remAgentArg[1];
            } else {
                cout << "Wrong number of arguments. Should be two." << endl;
                return 0;
            }
        } else {
            glob.removeAgentMidRun = false;
            glob.removeAgentDay = -1;
        }
        if (vm.count("resource")) {
            glob.removeResMidRun = true;
            if (remResArg.size() == 3) {
                glob.removeResId = remResArg[0];
                glob.removeResDay = remResArg[1];
                if (remResArg[2] == 0) {
                    glob.removeResHoldings = false;
                } else if (remResArg[2] == 1) {
                    glob.removeResHoldings = true;
                }
            } else {
                cout << "Wrong number of arguments. Should be three. " << endl;
                return 0;
            }
        } else {
            glob.removeResMidRun = false;
            glob.removeResDay = -1;
        }
        if (vm.count("middle")) {
            glob.saveInMiddle = true;
            if (saveInMiddleArg.size() == 2) {
                glob.saveInMiddleFoldername = saveInMiddleArg[0];
                glob.saveInMiddleDay = boost::lexical_cast<int>(saveInMiddleArg[1]);
            } else {
                cout << "Wrong number of arguments. Should be two." << endl;
            }
        } else {
            glob.saveInMiddle = false;
        }
        /*
         * For logging level. Print everything whose logging level is <= the number set.
         */
        if (vm.count("verbose")) {
            glob.verboseLevel = vm["verbose"].as<int>();
        } else {
            glob.verboseLevel = 3;
        }
        if (vm.count("exchange")) {
            glob.saveExchangeRateData = true;
        } else {
            glob.saveExchangeRateData = false;
        }


        if (vm.count("START_DAY")) {
            glob.START_DAY = vm["START_DAY"].as<int>();
        } else {
            cerr << "START_DAY value not in config file" << endl;
            return -1;
        }
        if (vm.count("DAY_LENGTH")) {
            glob.DAY_LENGTH = vm["DAY_LENGTH"].as<int>();
        } else {
            cerr << "DAY_LENGTH value not in config file" << endl;
            return -1;
        }
        if (vm.count("NUM_DAYS")) {
            glob.NUM_DAYS = vm["NUM_DAYS"].as<int>();
        } else {
            cerr << "NUM_DAYS value not in config file" << endl;
            return -1;
        }
        if (vm.count("NUM_AGENTS")) {
            glob.NUM_AGENTS = vm["NUM_AGENTS"].as<int>();
            ifstream file;
            file.open(glob.configAgentCSV.c_str());
            int agentsNum = 0;
            string line;
            int lineNum = 0;
            while (file >> line) {
                if ((lineNum % 2 == 0) && (lineNum != 0)) {
                    int pos;
                    for (unsigned i = 0; i < line.size(); i++) {
                        if (line[i] == ',') {
                            pos = i;
                        }
                    }
                    agentsNum += boost::lexical_cast<int>(line.substr(pos+1, line.size()-pos-1));    // Get the last number of the line
                }
                lineNum++;
            }
            // print error message and stop running when NUM_AGENTS is wrong.
            if (agentsNum != glob.NUM_AGENTS) {
                cerr << "NUM_AGENTS not correct" << endl;
                return -1;
            }
        } else {
            cerr << "NUM_AGENTS value not in config file" << endl;
            return -1;
        }
        if (vm.count("NUM_RESOURCES")) {
            glob.NUM_RESOURCES = vm["NUM_RESOURCES"].as<int>();
        } else {
            cerr << "NUM_RESOURCES value not in config file" << endl;
            return -1;
        }
        if (vm.count("NUM_AGENT_GROUPS")) {
            glob.NUM_AGENT_GROUPS = vm["NUM_AGENT_GROUPS"].as<int>();
            ifstream file;
            file.open(glob.configAgentCSV.c_str());
            int lineNum = 0;
            string line;
            while (file >> line) {
                lineNum++;
            }
            file.close();
            /* "type 0",... is taken as two lines in C++.
             * So the number of groups = IlineNum-1) / 2
             */ 
            if ( glob.NUM_AGENT_GROUPS != (lineNum - 1) / 2) {
                cerr << "NUM_AGENT_GROUPS not correct" << endl;
                return -1;
            }
        } else {
            cerr << "NUM_AGENT_GROUPS value not in config file" << endl;
            return -1;
        }
        if (vm.count("MAX_RES_AMOUNT")) {
            glob.MAX_RES_AMOUNT = vm["MAX_RES_AMOUNT"].as<long int>();
            if (glob.MAX_RES_AMOUNT < 0) {
                cerr << "MAX_RES_AMOUNT cannot be negative." << endl;
                return -1;
            }
        } else {
            cerr << "MAX_RES_AMOUNT value not in config file" << endl;
            return -1;
        }
        if (vm.count("MIN_RES_AMOUNT")) {
            glob.MIN_RES_AMOUNT = vm["MIN_RES_AMOUNT"].as<int>();
            if (glob.MIN_RES_AMOUNT < 0) {
                cerr << "MIN_RES_AMOUNT cannot be negative." << endl;
                return -1;
            }
        } else {
            cerr << "MIN_RES_AMOUNT value not in config file" << endl;
            return -1;
        }
        if (vm.count("RES_CONSIDERED_DEPLETED")) {
            glob.RES_CONSIDERED_DEPLETED = vm["RES_CONSIDERED_DEPLETED"].as<double>();
            if (glob.RES_CONSIDERED_DEPLETED < 0) {
                cerr << "RES_CONSIDERED_DEPLETED cannot be negative." << endl;
                return -1;
            }
        } else {
            cerr << "RES_CONSIDERED_DEPLETED value not in config file" << endl;
            return -1;
        }
        if (vm.count("RES_CONSIDERED_RENEWED")) {
            glob.RES_CONSIDERED_RENEWED = vm["RES_CONSIDERED_RENEWED"].as<double>();
            if (glob.RES_CONSIDERED_RENEWED < 0) {
                cerr << "RES_CONSIDERED_RENEWED cannot be negative." << endl;
                return -1;
            }
        } else {
            cerr << "RES_CONSIDERED_RENEWED value not in config file" << endl;
            return -1;
        }
        if (vm.count("TAX_RATE")) {
            glob.TAX_RATE = vm["TAX_RATE"].as<double>();
        } else {
            cerr << "TAX_RATE value not in config file." << endl;
        }
        if (vm.count("RES_TRADE_ROUNDS")) {
            glob.RES_TRADE_ROUNDS = vm["RES_TRADE_ROUNDS"].as<int>();
        } else {
            cerr << "RES_TRADE_ROUNDS value not in config file" << endl;
            return -1;
        }
        if (vm.count("RES_TRADE_ATTEMPTS")) {
            glob.RES_TRADE_ATTEMPTS = vm["RES_TRADE_ATTEMPTS"].as<int>();
        } else {
            cerr << "RES_TRADE_ATTEMPTS value not in config file" << endl;
            return -1;
        }
        if (vm.count("DEVICE_TRADE_ROUNDS")) {
            glob.DEVICE_TRADE_ROUNDS = vm["DEVICE_TRADE_ROUNDS"].as<int>();
        } else {
            cerr << "DEVICE_TRADE_ROUNDS value not in config file" << endl;
            return -1;
        }
        if (vm.count("DEVICE_TRADE_ATTEMPTS")) {
            glob.DEVICE_TRADE_ATTEMPTS = vm["DEVICE_TRADE_ATTEMPTS"].as<int>();
        } else {
            cerr << "DEVICE_TRADE_ATTEMPTS value not in config file" << endl;
            return -1;
        }
        if (vm.count("MENU_SIZE")) {
            glob.MENU_SIZE = vm["MENU_SIZE"].as<int>();
        } else {
            cerr << "MENU_SIZE value not in config file" << endl;
            return -1;
        }
        if (vm.count("DEVICE_TRADE_MEMORY_LENGTH")) {
            glob.DEVICE_TRADE_MEMORY_LENGTH = vm["DEVICE_TRADE_MEMORY_LENGTH"].as<int>();
        } else {
            cerr << "DEVICE_TRADE_MEMORY_LENGTH value not in config file" << endl;
            return -1;
        }
        if (vm.count("DEVICE_PRODUCTION_MEMORY_LENGTH")) {
            glob.DEVICE_PRODUCTION_MEMORY_LENGTH = vm["DEVICE_PRODUCTION_MEMORY_LENGTH"].as<int>();
        } else {
            cerr << "DEVICE_PRODUCTION_MEMORY_LENGTH value not in config file" << endl;
            return -1;
        }
        if (vm.count("MIN_DEVICE_FOR_DEV_DEVICE_CONSIDERATION")) {
            glob.MIN_DEVICE_FOR_DEV_DEVICE_CONSIDERATION = vm["MIN_DEVICE_FOR_DEV_DEVICE_CONSIDERATION"].as<int>();
        } else {
            cerr << "MIN_DEVICE_FOR_DEV_DEVICE_CONSIDERATION value not in config file" << endl;
            return -1;
        }
        if (vm.count("MIN_RES_HELD_FOR_DEVICE_CONSIDERATION")) {
            glob.MIN_RES_HELD_FOR_DEVICE_CONSIDERATION = vm["MIN_RES_HELD_FOR_DEVICE_CONSIDERATION"].as<int>();
        } else {
            cerr << "MIN_RES_HELD_FOR_DEVICE_CONSIDERATION value not in config file" << endl;
            return -1;
        }
        if (vm.count("DAILY_EXP_PENALTY")) {
            glob.DAILY_EXP_PENALTY = vm["DAILY_EXP_PENALTY"].as<double>();
        } else {
            cerr << "DAILY_EXP_PENALTY value not in config file" << endl;
            return -1;
        }
        if (vm.count("PRODUCTION_EPSILON")) {
            glob.PRODUCTION_EPSILON = vm["PRODUCTION_EPSILON"].as<double>();
        } else {
            cerr << "PRODUCTION_EPSILON value not in config file" << endl;
            return -1;
        }
        if (vm.count("RESOURCES_IN_TOOL")) {
            glob.RESOURCES_IN_TOOL = vm["RESOURCES_IN_TOOL"].as<int>();
        } else {
            cerr << "RESOURCES_IN_TOOL value not in config file" << endl;
            return -1;
        }
        if (vm.count("INVENTOR_DEVICE_EXPERIENCE")) {
            glob.INVENTOR_DEVICE_EXPERIENCE = vm["INVENTOR_DEVICE_EXPERIENCE"].as<double>();
        } else {
            cerr << "INVENTOR_DEVICE_EXPERIENCE value not in config file" << endl;
            return -1;
        }
        if (vm.count("NUM_DEVICE_COMPONENTS")) {
            glob.NUM_DEVICE_COMPONENTS = vm["NUM_DEVICE_COMPONENTS"].as<int>();
        } else {
            cerr << "NUM_DEVICE_COMPONENTS value not in config file" << endl;
            return -1;
        }
        if (vm.count("DAILY_DEVICE_DECAY")) {
            glob.DAILY_DEVICE_DECAY = vm["DAILY_DEVICE_DECAY"].as<double>();
        } else {
            cerr << "DAILY_DEVICE_DECAY value not in config file" << endl;
            return -1;
        }
        if (vm.count("MIN_HELD_DEVICE_EXPERIENCE")) {
            glob.MIN_HELD_DEVICE_EXPERIENCE = vm["MIN_HELD_DEVICE_EXPERIENCE"].as<double>();
        } else {
            cerr << "MIN_HELD_DEVICE_EXPERIENCE value not in config file" << endl;
        }
        if (vm.count("MIN_RES_UTIL")) {
            glob.MIN_RES_UTIL = vm["MIN_RES_UTIL"].as<double>();
        } else {
            cerr << "MIN_RES_UTIL value not in config file" << endl;
        }
        if (vm.count("TRADE_EPSILON")) {
            glob.TRADE_EPSILON = vm["TRADE_EPSILON"].as<double>();
        } else {
            cerr << "TRADE_EPSILON value not in config file" << endl;
        }
        if (vm.count("TOOL_PROBABILITY_FACTOR")) {
            glob.TOOL_PROBABILITY_FACTOR = vm["TOOL_PROBABILITY_FACTOR"].as<double>();
        } else {
            cerr << "TOOL_PROBABILITY_FACTOR value not in config file" << endl;
        }
        if (vm.count("DEVICE_PROBABILITY_FACTOR")) {
            glob.DEVICE_PROBABILITY_FACTOR = vm["DEVICE_PROBABILITY_FACTOR"].as<double>();
        } else {
            cerr << "DEVICE_PROBABILITY_FACTOR value not in config file" << endl;
        }
        if (vm.count("TOOL_FACTOR")) {
            glob.TOOL_FACTOR = vm["TOOL_FACTOR"].as<double>();
        } else {
            cerr << "TOOL_FACTOR value not in config file" << endl;
        }
        if (vm.count("TOOL_LIFETIME")) {
            glob.TOOL_LIFETIME = vm["TOOL_LIFETIME"].as<double>();
        } else {
            cerr << "TOOL_LIFETIME value not in config file" << endl;
        }
        if (vm.count("MACHINE_FACTOR")) {
            glob.MACHINE_FACTOR = vm["MACHINE_FACTOR"].as<double>();
        } else {
            cerr << "MACHINE_FACTOR value not in config file" << endl;
        }
        if (vm.count("MACHINE_LIFETIME")) {
            glob.MACHINE_LIFETIME = vm["MACHINE_LIFETIME"].as<double>();
        } else {
            cerr << "MACHINE_LIFETIME value not in config file" << endl;
        }
        if (vm.count("FACTORY_FACTOR")) {
            glob.FACTORY_FACTOR = vm["FACTORY_FACTOR"].as<double>();
        } else {
            cerr << "FACTORY_FACTOR value not in config file" << endl;
        }
        if (vm.count("FACTORY_LIFETIME")) {
            glob.FACTORY_LIFETIME = vm["FACTORY_LIFETIME"].as<double>();
        } else {
            cerr << "FACTORY_LIFETIME value not in config file" << endl;
        }
        if (vm.count("INDUSTRY_FACTOR")) {
            glob.INDUSTRY_FACTOR = vm["INDUSTRY_FACTOR"].as<double>();
        } else {
            cerr << "INDUSTRY_FACTOR value not in config file" << endl;
        }
        if (vm.count("INDUSTRY_LIFETIME")) {
            glob.INDUSTRY_LIFETIME = vm["INDUSTRY_LIFETIME"].as<double>();
        } else {
            cerr << "INDUSTRY_LIFETIME value not in config file" << endl;
        }
        if (vm.count("DEV_MACHINE_FACTOR")) {
            glob.DEV_MACHINE_FACTOR = vm["DEV_MACHINE_FACTOR"].as<double>();
        } else {
            cerr << "DEV_MACHINE_FACTOR value not in config file" << endl;
        }
        if (vm.count("DEV_MACHINE_LIFETIME")) {
            glob.DEV_MACHINE_LIFETIME = vm["DEV_MACHINE_LIFETIME"].as<double>();
        } else {
            cerr << "DEV_MACHINE_LIFETIME value not in config file" << endl;
        }
        if (vm.count("DEV_FACTORY_FACTOR")) {
            glob.DEV_FACTORY_FACTOR = vm["DEV_FACTORY_FACTOR"].as<double>();
        } else {
            cerr << "DEV_FACTORY_FACTOR value not in config file" << endl;
        }
        if (vm.count("DEV_FACTORY_LIFETIME")) {
            glob.DEV_FACTORY_LIFETIME = vm["DEV_FACTORY_LIFETIME"].as<double>();
        } else {
            cerr << "DEV_FACTORY_LIFETIME value not in config file" << endl;
        }
        if (vm.count("DAYS_OF_DEVICE_TO_HOLD")) {
            glob.DAYS_OF_DEVICE_TO_HOLD = vm["DAYS_OF_DEVICE_TO_HOLD"].as<double>();
        } else {
            cerr << "DAYS_OF_DEVICE_TO_HOLD value not in config file" << endl;
        }
        if (vm.count("TRADE_EXISTS")) {
            glob.TRADE_EXISTS = vm["TRADE_EXISTS"].as<bool>();
        } else {
            cerr << "TRADE_EXISTS value not in config file" << endl;
        }
        if (vm.count("DEVICES_EXIST")) {
            glob.DEVICES_EXIST = vm["DEVICES_EXIST"].as<bool>();
        } else {
            cerr << "DEVICES_EXIST value not in config file" << endl;
        }
        if (vm.count("TOOLS_ONLY")) {
            glob.TOOLS_ONLY = vm["TOOLS_ONLY"].as<bool>();
        } else {
            cerr << "TOOLS_ONLY value not in config file" << endl;
        }
    } catch (exception& e) {
        cout << "Wrong args: " << e.what() << endl;
        return -1;
    }
    return 0;
}
