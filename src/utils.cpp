/**
 * Utils check what phase (trading, making device, etc) the agents are currently in
 * and call the corresponding functions to execute the phase.
 */
#include <vector>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <cassert>
#include <stdlib.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "globals.h"
#include "utils.h"
#include "agent.h"
#include "device.h"
#include "marketplace.h"
#include "statstracker.h"
#include "devmarketplace.h"
#include "logging.h"

using namespace std;

/**
 * Print the number of agents, number of days, nunber of resources of this run.
 */
void Utils::printStartConditions()
{
    cout << "Num agents: " << glob.NUM_AGENTS << endl;
    cout << "Num days: " << glob.NUM_DAYS << endl;
    cout << "Num resources: " << glob.NUM_RESOURCES << endl;
}

/**
 * Used below for shuffling the list.
 */
static int my_random_int_from_0(int upper)
{
    return glob.random_int(0, upper);
}

/**
 * Agents are selected in random order to work.
 */
void Utils::agentsWork()
{
    LOG(2) << "Entering agents work phase";
    vector<Agent *> inventors;
    for (int aId = 0; aId < glob.NUM_AGENTS; aId++) {
        inventors.push_back(glob.agent[aId]);
    }
//#ifndef DONT_RANDOMIZE
    random_shuffle(inventors.begin(), inventors.end(), my_random_int_from_0);
//#endif
    BOOST_FOREACH(Agent *agent, inventors) {
        if (agent->inSimulation) {
            agent->workDay();
        }
    }
}

void Utils::updateDeviceRecipe()
{
    vector<int>::iterator idx;
    vector<int>::iterator idx2;

    // Update tool recipes
    vector<int> newlyObsoleteTools = vector<int>();           // To store all the tools that need to be deleted
    for (unsigned i = 0; i < glob.depletedRes.size(); i++) {
        int resId = glob.depletedRes[i];
        for (unsigned j = 0; j < glob.toolsKnown.size(); j++) {
            int toolId = glob.toolsKnown[j];
            /*
             * If an invented tool has resId--one of the depelted resources--as a component,
             * and the tool has not been added to newlyObsoleteTools already,
             * add it to newlyObsoleteTools.
             */
            idx = find(glob.discoveredDevices[TOOL][toolId]->components.begin(),
                                             glob.discoveredDevices[TOOL][toolId]->components.end(),
                                             resId);
            idx2 = find(newlyObsoleteTools.begin(), newlyObsoleteTools.end(), toolId);
            if ( (idx != glob.discoveredDevices[TOOL][toolId]->components.end()) &&
                 (idx2 == newlyObsoleteTools.end()) ) {
                newlyObsoleteTools.push_back(toolId);
            }
        }
    }
    /*
     * Go through all of the newlyObsoleteTools,
     * delete it from invented tools,
     * add it to not invented tools.
     */
    for (unsigned i = 0; i < newlyObsoleteTools.size(); i++) {
        int toolId = newlyObsoleteTools[i];
        idx = find(glob.toolsKnown.begin(), glob.toolsKnown.end(), toolId);
        assert(idx != glob.toolsKnown.end());
        glob.toolsKnown.erase(idx);
        glob.toolsNotKnown.push_back(toolId);
    }

        
    // Update machine and devMachine recipes
    vector<int> newlyObsoleteMachines = vector<int>();
    vector<int> newlyObsoleteDevMachines = vector<int>();
    for (unsigned i = 0; i < newlyObsoleteTools.size(); i++) {
        int toolId = newlyObsoleteTools[i];
        for (unsigned j = 0; j < glob.machinesKnown.size(); j++) {
            int machineId = glob.machinesKnown[j];
            idx = find(glob.discoveredDevices[MACHINE][machineId]->components.begin(),
                                             glob.discoveredDevices[MACHINE][machineId]->components.end(),
                                             toolId);
            idx2 = find(newlyObsoleteMachines.begin(), newlyObsoleteMachines.end(), machineId);
            if ( (idx != glob.discoveredDevices[MACHINE][machineId]->components.end()) &&
                (idx2 == newlyObsoleteMachines.end()) ) {
               newlyObsoleteMachines.push_back(machineId);
            }
        }
        for (unsigned j = 0; j < glob.devMachinesKnown.size(); j++) {
            int devMachineId = glob.devMachinesKnown[j];
            idx = find(glob.discoveredDevices[DEVMACHINE][devMachineId]->components.begin(),
                                             glob.discoveredDevices[DEVMACHINE][devMachineId]->components.end(),
                                             toolId);
            idx2 = find(newlyObsoleteDevMachines.begin(), newlyObsoleteDevMachines.end(), devMachineId);
            if ( (idx != glob.discoveredDevices[DEVMACHINE][devMachineId]->components.end()) &&
                (idx2 == newlyObsoleteDevMachines.end()) ) {
               newlyObsoleteDevMachines.push_back(devMachineId);
            }
        }
    }

    for (unsigned i = 0; i < newlyObsoleteMachines.size(); i++) {
        int machineId = newlyObsoleteMachines[i];
        idx = find(glob.machinesKnown.begin(), glob.machinesKnown.end(), machineId);
        assert(idx != glob.machinesKnown.end());
        glob.machinesKnown.erase(idx);
        glob.machinesNotKnown.push_back(machineId);
    }
    for (unsigned i = 0; i < newlyObsoleteDevMachines.size(); i++) {
        int devMachineId = newlyObsoleteDevMachines[i];
        idx = find(glob.devMachinesKnown.begin(), glob.devMachinesKnown.end(), devMachineId);
        assert(idx != glob.devMachinesKnown.end());
        glob.devMachinesKnown.erase(idx);
        glob.devMachinesNotKnown.push_back(devMachineId);
    }

    // Update factory recipes
    vector<int> newlyObsoleteFactories = vector<int>();
    for (unsigned i = 0; i < newlyObsoleteMachines.size(); i++) {
        int machineId = newlyObsoleteMachines[i];
        for (unsigned j = 0; j < glob.factoriesKnown.size(); j++) {
            int factoryId = glob.factoriesKnown[j];
            idx = find(glob.discoveredDevices[FACTORY][factoryId]->components.begin(),
                                             glob.discoveredDevices[FACTORY][factoryId]->components.end(),
                                             machineId);
            idx2 = find(newlyObsoleteFactories.begin(), newlyObsoleteFactories.end(), factoryId);
            if ( (idx != glob.discoveredDevices[FACTORY][factoryId]->components.end()) &&
                 (idx2 == newlyObsoleteFactories.end()) ) {
                newlyObsoleteFactories.push_back(factoryId);
            }
        }
    }
    for (unsigned i = 0; i < newlyObsoleteFactories.size(); i++) {
        int factoryId = newlyObsoleteFactories[i];
        idx = find(glob.factoriesKnown.begin(), glob.factoriesKnown.end(), factoryId);
        assert(idx != glob.factoriesKnown.end());
        glob.factoriesKnown.erase(idx);
        glob.factoriesNotKnown.push_back(factoryId);
    }

    // Update industry recipes
    vector<int> newlyObsoleteIndustries = vector<int>();
    for (unsigned i = 0; i < newlyObsoleteFactories.size(); i++) {
        int factoryId = newlyObsoleteFactories[i];
        for (unsigned j = 0; j < glob.industriesKnown.size(); j++) {
            int industryId = glob.industriesKnown[j];
            idx = find(glob.discoveredDevices[INDUSTRY][industryId]->components.begin(),
                                             glob.discoveredDevices[INDUSTRY][industryId]->components.end(),
                                             factoryId);
            idx2 = find(newlyObsoleteIndustries.begin(), newlyObsoleteIndustries.end(), industryId);
            if ( (idx != glob.discoveredDevices[INDUSTRY][industryId]->components.end()) &&
                 (idx2 == newlyObsoleteIndustries.end()) ) {
                newlyObsoleteIndustries.push_back(industryId);
            }
        }
    }
    for (unsigned i = 0; i < newlyObsoleteIndustries.size(); i++) {
        int industryId = newlyObsoleteIndustries[i];
        idx = find(glob.industriesKnown.begin(), glob.industriesKnown.end(), industryId);
        assert(idx != glob.industriesKnown.end());
        glob.industriesKnown.erase(idx);
        glob.industriesNotKnown.push_back(industryId);
    }

    // Update devFactory recipes
    vector<int> newlyObsoleteDevFactories = vector<int>();
    for (unsigned i = 0; i < newlyObsoleteDevMachines.size(); i++) {
        int devMachineId = newlyObsoleteDevMachines[i];
        for (unsigned j = 0; j < glob.devFactoriesKnown.size(); j++) {
            int devFactoryId = glob.devFactoriesKnown[j];
            idx = find(glob.discoveredDevices[DEVFACTORY][devFactoryId]->components.begin(),
                                             glob.discoveredDevices[DEVFACTORY][devFactoryId]->components.end(),
                                             devMachineId);
            idx2 = find(newlyObsoleteDevFactories.begin(), newlyObsoleteDevFactories.end(), devFactoryId);
            if ( (idx != glob.discoveredDevices[DEVFACTORY][devFactoryId]->components.end()) &&
                 (idx2 == newlyObsoleteDevFactories.end()) ) {
                newlyObsoleteDevFactories.push_back(devFactoryId);
            }
        }
    }
    for (unsigned i = 0; i < newlyObsoleteDevFactories.size(); i++) {
        int devFactoryId = newlyObsoleteDevFactories[i];
        idx = find(glob.devFactoriesKnown.begin(), glob.devFactoriesKnown.end(), devFactoryId);
        assert(idx != glob.devFactoriesKnown.end());
        glob.devFactoriesKnown.erase(idx);
        glob.devFactoriesNotKnown.push_back(devFactoryId);
    }

    for (unsigned i = 0; i < newlyObsoleteIndustries.size(); i++) {
        glob.discoveredDevices[INDUSTRY][newlyObsoleteIndustries[i]] = NULL;
    }
    for (unsigned i = 0; i < newlyObsoleteFactories.size(); i++) {
        glob.discoveredDevices[FACTORY][newlyObsoleteFactories[i]] = NULL;
    }
    for (unsigned i = 0; i < newlyObsoleteMachines.size(); i++) {
        glob.discoveredDevices[MACHINE][newlyObsoleteMachines[i]] = NULL;
    }
    for (unsigned i = 0; i < newlyObsoleteDevFactories.size(); i++) {
        glob.discoveredDevices[DEVFACTORY][newlyObsoleteDevFactories[i]] = NULL;
    }
    for (unsigned i = 0; i < newlyObsoleteDevMachines.size(); i++) {
        glob.discoveredDevices[DEVMACHINE][newlyObsoleteDevMachines[i]] = NULL;
    }
    for (unsigned i = 0; i < newlyObsoleteTools.size(); i++) {
        glob.discoveredDevices[TOOL][newlyObsoleteTools[i]] = NULL;
    }
}




/**
 * Let the agents start trading resources.
 */
void Utils::agentsTrade()
{
    if (glob.TRADE_EXISTS) {
        LOG(2) << "Entering agents trade phase";
        glob.resourceMarket->tradeResources();
    } else {
        LOG(2) << "Skipping disabled agents trade phase";
    }
}

/**
 * Agents are selected in random order to invent devices.
 */
void Utils::agentsInvent()
{
    if (! glob.DEVICES_EXIST) {
        LOG(2) << "Skipping agents invent phase, DEVICES_EXIST being disabled";
        return;
    }
    LOG(2) << "Entering agents invent phase";
    vector<Agent *> inventors;
    for (int aId = 0; aId < glob.NUM_AGENTS; aId++) {
        inventors.push_back(glob.agent[aId]);
        if (glob.agent[aId]->inSimulation) {
            glob.agent[aId]->updateDeviceComponentExperience();
        }
    }
    // shuffle the inventors list, and then call toolInvention() on each.
#ifndef DONT_RANDOMIZE
    random_shuffle(inventors.begin(), inventors.end(), my_random_int_from_0);
#endif
    BOOST_FOREACH(Agent *agent, inventors) {
        if (agent->inSimulation) {
            agent->toolInvention();
        }
    }
}

/**
 * At the end of each day, update the status.
 */
void Utils::endDay()
{
    LOG(2) << "Entering end day checks phase";
    for (int aId = 0; aId < glob.NUM_AGENTS; aId++) {
        if (glob.agent[aId]->inSimulation) {
            glob.agent[aId]->endDayChecks();

            glob.agent[aId]->logAgentData();
            //if (aId == 0) {
            //    saveEndDayData();
            //}
        }
    }
}


/**
 * At the end of each day, renewable resources will regenerate based on their renewalRate.
 */
void Utils::resourceRenewal()
{
    LOG(2) << "Entering resources renew phase";
    /*
     * The renewal rate for each resource should be the same for agents. 
     * So use agent 0.
     */
    Agent *ag = glob.agent[0];
    for (int resId = 0; resId < glob.NUM_RESOURCES; resId++) {
        // If the resource is limited and has not been used up, regenerate.
        if ( (glob.RES_AMOUNT[resId].first == true) && (glob.RES_AMOUNT[resId].second > 0) ) {
            /*
             * the amount of newly generated resources
             * Do not make use int until finishing all the intermediate calculations.
             */
            double temp = ag->resProp[resId].renewalRate * (double)glob.RES_AMOUNT[resId].second;
            long int regeneratedAmount = temp;

            /*
             * If the calculated generated amount plus the current amount is greater than the upper limit,
             * the current amount is set to the limit -> to avoid memory overflow.
             * Otherwise, add the generated amount to current amount.
             */

            if ( (regeneratedAmount+glob.RES_AMOUNT[resId].second) > glob.MAX_RES_AMOUNT ) {
                glob.RES_AMOUNT[resId].second = glob.MAX_RES_AMOUNT;
            } else {
                glob.RES_AMOUNT[resId].second += regeneratedAmount;
            }

           /*
            * If the resource's new amount >= RES_CONSIDERED_RENEWED and is in depletedRes,
            * move it from depletedRes to nonDepletedRes.
            */
           vector<int>::iterator idx = find(glob.depletedRes.begin(), glob.depletedRes.end(), resId);
           if ( (idx != glob.depletedRes.end()) && (glob.RES_AMOUNT[resId].second >= glob.RES_CONSIDERED_RENEWED) ) {
               glob.depletedRes.erase(idx);
               glob.nonDepletedRes.push_back(resId);
               vector<int>::iterator idx2 = find(glob.newlyDepletedRes.begin(), glob.newlyDepletedRes.end(), resId);
               if (idx2 != glob.newlyDepletedRes.end()) {
                   glob.newlyDepletedRes.erase(idx2);
               }
           }
        }
    }
}


/*
 * At the end of each day, unused resources and devices will decay.
 */
void Utils::endDayDecay()
{
    LOG(2) << "Entering day decay phase";
    BOOST_FOREACH(Agent *agent, glob.agent) {
        if (agent->inSimulation) {
            agent->decay();
        }
    }

    BOOST_FOREACH(Agent *agent, glob.agent) {
        assert(agent->calcMinHeld() >= 0);
    }
}


/**
 * From most complex to least complex, agents trade devices, then decide
 * to make devices for themselves, then trade, and so on
 */
void Utils::agentsTradeDevices()
{
    /* Shouldn't trade when TRADE_EXISTS is turned off. */
    if (glob.TRADE_EXISTS) {
        LOG(2) << "Entering trade devices phase";
        BOOST_FOREACH(Agent *agent, glob.agent) {
            if (agent->overtime < glob.DAY_LENGTH) {
                agent->utilGainThroughDevSoldToday = 0.0;
            }
        }
        if (! glob.DEVICES_EXIST) {
            return;
        }
        glob.resourceMarket->tradeResources();
        device_name_t devarr[] = { INDUSTRY, FACTORY, DEVFACTORY, MACHINE, DEVMACHINE, TOOL };
        for (int i = 0; i < 6; i++) {  // 6 is length of array above.
            device_name_t types = devarr[i];

            glob.deviceMarket->tradeDevices(types);

            BOOST_FOREACH(Agent *agent, glob.agent) {
                if (agent->inSimulation) {
                    agent->personalDevices(types);
                }
            }
            glob.resourceMarket->tradeResources();
        }
    } else {
        LOG(2) << "Skipping disabled agents trade phase";
    }
}

/**
 * Let agents start producing some of the devices they already invented.
 */
void Utils::agentsProduceDevices()
{
    LOG(2) << "Entering produce devices phase";
    if (! glob.DEVICES_EXIST) {
        return;
    }
    BOOST_FOREACH(Agent *agent, glob.agent) {
        if (agent->inSimulation) {
            agent->deviceProduction();
        }
    }
}

/*
 * Check if any device has been made / exists.
 */
bool Utils::deviceExists(device_name_t devType)
{
    bool exist = false;
    for (int resId = 0; resId < glob.NUM_RESOURCES; resId++) {
        if ( (glob.discoveredDevices[devType][resId] != NULL) && (glob.discoveredDevices[devType][resId]->agentsKnown() > 0) ) {
            exist = true;
            break;
        }
    }
    return exist;
}


/**
 * Print the total number of utils and units of resources.
 */
void Utils::printSumUtilAndRes()
{
    cout << "SumUtil:" << glob.otherStats->getSumUtilBack() << endl;
    cout << "SumRes:" << glob.otherStats->getSumResBack() << endl;
}

/**
 * At the end of each day, compile the stats of the day
 * \param dayNumber the current day in the simulation
 */
void Utils::dayAnalysis(int dayNumber)
{
    LOG(2) << "Entering day analysis phase";
    BOOST_FOREACH(Agent *agent, glob.agent) {
        agent->calcUtilityToday();
    }

    // Update each statistics module in turn
    glob.tradeStats->dailyUpdate();
    glob.productionStats->dailyUpdate();
    glob.otherStats->dailyUpdate();
    glob.otherStats->getSumResByAgent();

    BOOST_FOREACH(Agent *agent, glob.agent) {
        agent->resetTodayStats();
    }
}

/**
 * Decide if to save the current day status, to remove any resources or agents.
 */
void Utils::removeOrSave(int dayNumber)
{
    LOG(2) << "Entering remove or save phase";
    if (glob.SAVE_DAY_STATUS) {
        if (dayNumber == glob.DAY_FOR_SAVE) {
            saveDayStatus(dayNumber);
        }
    }
    if (glob.REMOVE_RES) {
        if (dayNumber == glob.REMOVE_RES_DAY) {
            glob.removeRes(glob.RES_TO_REMOVE, dayNumber);
        }
    }
    if (glob.REMOVE_AGENT) {
        if (dayNumber == glob.REMOVE_AGENT_DAY) {
            glob.removeAgent(glob.AGENT_TO_REMOVE, dayNumber);
        }
    }
}


/**
 * Call saveResults at the end of each simulation to save the results.
 */
void Utils::endSim()
{
    printStartConditions();
    printSumUtilAndRes();
    if (glob.SAVE_DAY_STATUS) {
        cout << "Your status on day " << glob.DAY_FOR_SAVE << " is saved in " <<
            glob.DAY_STATUS_SAVE_FOLDER << endl;
    }
    saveResults();
}

/**
 * Used to sort a vector of vectors. Only sort on the first item.
 */
bool mySort(const vector<double>& i, const vector<double>& j)
{
    return i[0] < j[0];
}


/**
 * Compute the five quartiles of elements of each index position.
 * \param data a vector of a vector of double
 * \return five vectors of double with min, 25%, median, 75%, max.
 */
vector<vector<double> > Utils::calcQuartiles(vector<vector<double> > data)
{
    vector<double> min;     // a vector of each index's minimum value of all sub-vectors in data
    vector<double> med;     // a vector of each index's median value of all sub-vectors in data
    vector<double> max;     // a vector of each index's maximum value of all sub-vectors in data
    vector<vector<double> > copy = data;    // make a copy so the original data will not be changed

    bool even;
    if (copy.size() % 2 == 0) {
        even = true;
    } else {
        even = false;
    }

    /*
     * Find the min, median, and max.
     */
    for ( unsigned int m = 0; m < copy[0].size(); m++) {
        if ( m != 0 ) {
            for (unsigned int i = 0; i < copy.size(); i++) {
                swap(copy[i][m], copy[i][0]);
            }
        }
        sort(copy.begin(), copy.end(), mySort);
        min.push_back(copy[0][0]);
        if (even) {
            med.push_back( (copy[copy.size()/2-1][0] + copy[copy.size()/2][0]) / 2.0 );
        } else {
            med.push_back(copy[copy.size() / 2][0]);
        }
        max.push_back(copy[copy.size()-1][0]);
    }

    /*
     * Find the first and third quartile.
     */
    vector<double> qOne;
    vector<double> qThree;
    vector<double> aboveMedian;
    vector<double> belowMedian;
    for (int dayNum = 0; dayNum < glob.NUM_DAYS; dayNum++) {
        aboveMedian.clear();
        belowMedian.clear();
        for (unsigned int vecId = 0; vecId < data.size(); vecId++) {
            if (data[vecId][dayNum] >= med[dayNum]) {
                aboveMedian.push_back(data[vecId][dayNum]);
            }
            if (data[vecId][dayNum] <= med[dayNum]) {
                belowMedian.push_back(data[vecId][dayNum]);
            }
        }

        sort(belowMedian.begin(), belowMedian.end());
        if (belowMedian.size() % 2 == 0) {
            qOne.push_back( (belowMedian[belowMedian.size()/2-1] + belowMedian[belowMedian.size()/2]) / 2.0 );
        } else {
            qOne.push_back( belowMedian[belowMedian.size() / 2] );
        }
        sort(aboveMedian.begin(), aboveMedian.end());
        if (aboveMedian.size() % 2 == 0) {
            qThree.push_back( (aboveMedian[aboveMedian.size()/2-1] + aboveMedian[aboveMedian.size()/2]) / 2.0 );
        } else {
            qThree.push_back( aboveMedian[aboveMedian.size() / 2] );
        }
    }

    vector<vector<double> > result;
    result.push_back(min);
    result.push_back(qOne);
    result.push_back(med);
    result.push_back(qThree);
    result.push_back(max);

    copy.clear();

    return result;
}

/**
 * Print the first line of a csv file with day numbers.
 */
void Utils::headerByDay(ofstream &file, string filePath)
{
    if ( !boost::filesystem::exists(filePath)) {
        file.open(filePath.c_str());
        file << ",";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            file << (i + 1) << ",";
        }
        file << "\n";
        file.close();
    }
}

/**
 * BRH 2013.05.23: This graph shows the gini coefficient.  The formula used is can be found at
 * http://en.wikipedia.org/wiki/Gini_coefficient.  The python adaption was corrected to the one in
 * http://econpy.googlecode.com/svn/trunk/pytrix/utilities.py
 *
 * First and last day don't have a gini, so leave as blank.
 */
void Utils::saveGini()
{
    /* Calculate the gini coefficients for all agents. */
    vector<double> orderedUtils;
    vector<double> y;
    vector<vector<double> > sumUtilByAgent = glob.otherStats->getSumUtilByAgent();
    vector<double> results;
    for (int dayNum = 1; dayNum < glob.NUM_DAYS-1; dayNum++) {
        orderedUtils.clear();
        for (int aId = 0; aId < glob.NUM_AGENTS; aId++) {
            if (glob.agent[aId]->inSimulation) {
                orderedUtils.push_back(accumulate(sumUtilByAgent[aId].begin() + dayNum - 1,
                                                sumUtilByAgent[aId].begin() + dayNum + 2, 0.0));
            }
        }
        sort(orderedUtils.begin(), orderedUtils.end());
        y.clear();
        for (unsigned int i = 0; i < orderedUtils.size(); i++) {
            y.push_back(accumulate(orderedUtils.begin(), orderedUtils.begin() + i + 1, 0));
        }

        double ySum = accumulate(y.begin(), y.end(), 0.0);
        double B = ySum / (y[y.size()-1] * double(orderedUtils.size()));
        
        results.push_back(1.0 + (1.0 / double(orderedUtils.size())) - (2.0 * B));
    }


    /* If set to save to files, save results to gini.csv. */
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/gini.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        file << "giniPerAgent_" << glob.SIM_NAME << ",,";   // Leave the first day blank
        for (unsigned int i = 0; i < results.size(); i++) {
            file << results[i] << ",";
        }
        file << ",\n";      // leave the last day blank
        file.close();
    }
    /* If set to save to database, store results to allResults[0] (0 is for average). */
    if (glob.saveDatabaseSet) {
        /*
         * First and last day do not have gini. So results lack two values when saved into database.
         * Use 0.0 for those two values.
         */
        allResults[0].push_back(vector<double>());
        allResults[0][0].push_back(0.0);
        for (unsigned int i = 0; i < results.size(); i++) {
            allResults[0][0].push_back(results[i]);     // Gini is the first field saved into database, so its index is 0.
        }
        allResults[0][0].push_back(0.0);
    }
    results.clear();        // Clear the memory.

    /* Calculate gini for each group. */
    if (glob.NUM_AGENT_GROUPS > 1) {
        vector<vector<double> > groupResults;
        for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
            groupResults.push_back(vector<double>());
            for (int dayNum = 1; dayNum < glob.NUM_DAYS-1; dayNum++) {
                orderedUtils.clear();
                for (int aId = 0; aId < glob.NUM_AGENTS; aId++) {
                    if (glob.agent[aId]->group == gId) {
                        if (glob.agent[aId]->inSimulation) {
                            orderedUtils.push_back(accumulate(sumUtilByAgent[aId].begin() + dayNum - 1,
                                                            sumUtilByAgent[aId].begin() + dayNum + 2, 0.0));
                        }
                    }
                }
                sort(orderedUtils.begin(), orderedUtils.end());
                y.clear();
                for (unsigned int i = 0; i < orderedUtils.size(); i++) {
                    y.push_back(accumulate(orderedUtils.begin(), orderedUtils.begin() + i + 1, 0));
                }
                double B = accumulate(y.begin(), y.end(), 0.0) / (y[y.size()-1] * double(orderedUtils.size()));
                groupResults[gId].push_back(1.0 + (1.0 / double(orderedUtils.size())) - (2.0 * B));
            }
        }

        /* If set to save to files, save results to gini.csv. */
        if (glob.END_SAVE) {
            ofstream file;
            string filePath = glob.SAVE_FOLDER + "/gini.csv";
            file.open(filePath.c_str(), ios::app);
            for (unsigned int gId = 0; gId < groupResults.size(); gId++) {
                file << "giniByGroup_" << gId << "_" << glob.SIM_NAME << ",,";
                for (unsigned int day = 0; day < groupResults[gId].size(); day++) {
                    file << groupResults[gId][day] << ",";
                }
                file << ",\n";      // leave the last day blank
            }
            file << "\n";
            file.close();
        }
        /*
         * If set to save to database, save to allResults[gId]
         * In allResults, 0 is for perActiveAgents/allAgents.
         * So Group0 is at index 1.
         */
        if (glob.saveDatabaseSet) {
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                allResults[gId+1].push_back(vector<double>());
                allResults[gId+1][0].push_back(0.0);
                for (unsigned int i = 0; i < groupResults[gId].size(); i++) {
                    allResults[gId+1][0].push_back(groupResults[gId][i]);
                }
                allResults[gId+1][0].push_back(0.0);
            }
        }
        groupResults.clear();
    }

    orderedUtils.clear();
    y.clear();
    sumUtilByAgent.clear();
}

/**
 * Save the min, first quartile, median, third quartile and max of HHIs into csv files.
 */
void Utils::saveHHIQuartiles()
{
    string quartileNames[] = { "min", "q1", "median", "q3", "max" };
    vector<vector<int> > resGatheredByRes = glob.productionStats->getResGatheredByRes();
    vector<vector<vector<int> > > resGatheredByResByAgent = glob.productionStats->getResGatheredByResByAgent();

    /*
     * Save HHI for all agents
     */
    vector<vector<double> > data;
    vector<double> HHIpoints;
    double HHI = 0;
    for (int resId = 0; resId < glob.NUM_RESOURCES; resId++) {
        HHIpoints.clear();
        for (int dayNum = 0; dayNum < glob.NUM_DAYS; dayNum++) {
            HHI = 0;
            if (resGatheredByRes[resId][dayNum] > 0) {
                for (int aId = 0; aId < glob.NUM_AGENTS; aId++) {
                    HHI += pow((double(resGatheredByResByAgent[resId][aId][dayNum]) / double(resGatheredByRes[resId][dayNum])), 2);
                }
            }
            HHIpoints.push_back(HHI);
        }
        data.push_back(HHIpoints);
    }
    vector<vector<double> > q = calcQuartiles(data);

    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/HHI.csv";

        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);

        for (unsigned int eachQ = 0; eachQ < q.size(); eachQ++) {
            file << "HHI_" << quartileNames[eachQ] << "_ByAgents_" << glob.SIM_NAME << ",";
            for (int day = 0; day < glob.NUM_DAYS; day++) {
                file << q[eachQ][day] << ",";
            }
            file << "\n";
        }
        file.close();
    }
    if (glob.saveDatabaseSet) {
        for (unsigned int i = 0; i < q.size(); i++) {
            allResults[0].push_back(q[i]);
        }
    }
    q.clear();

    /* Save HHI for each group's agents. */
    if (glob.NUM_AGENT_GROUPS > 1) {
        vector<vector<vector<double> > > dataGroup;
        vector<vector<vector<double> > > groupResults;
        data.clear();
        for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
            for (int resId = 0; resId < glob.NUM_RESOURCES; resId++) {
                HHIpoints.clear();
                for (int dayNum = 0; dayNum < glob.NUM_DAYS; dayNum++) {
                    HHI = 0;
                    if (resGatheredByRes[resId][dayNum] > 0) {
                        for (int aId = 0; aId < glob.NUM_AGENTS; aId++) {
                            if (glob.agent[aId]->group == gId) {
                                HHI += pow((double(resGatheredByResByAgent[resId][aId][dayNum]) / double(resGatheredByRes[resId][dayNum])), 2);
                            }
                        }
                    }
                    HHIpoints.push_back(HHI);
                }
                data.push_back(HHIpoints);

            }
            dataGroup.push_back(data);
        }
        for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
            q = calcQuartiles(dataGroup[gId]);
            groupResults.push_back(q);
        }
        if (glob.END_SAVE) {
            ofstream file;
            string filePath = glob.SAVE_FOLDER + "/HHI.csv";
            file.open(filePath.c_str(), ios::app);
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (unsigned int eachQuartile = 0; eachQuartile < groupResults[gId].size(); eachQuartile++) {
                    file << "HHI_" << quartileNames[eachQuartile] << "_ByGroup_" << gId << glob.SIM_NAME << ",";
                    for (int dayNum = 0; dayNum < glob.NUM_DAYS; dayNum++) {
                        file << groupResults[gId][eachQuartile][dayNum] << ",";
                    }
                    file << "\n";
                }
            }
            file << "\n";
            file.close();
        }
        if (glob.saveDatabaseSet) {
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (unsigned int i = 0; i < groupResults[gId].size(); i++) {
                    allResults[gId+1].push_back(groupResults[gId][i]);
                }
            }
        }
        dataGroup.clear();
        groupResults.clear();
    }
    resGatheredByRes.clear();
    resGatheredByResByAgent.clear();
    data.clear();
    HHIpoints.clear();
}

/**
 * Save the total utility for each day into a csv file.
 */
void Utils::saveTotalUtility()
{
    vector<double> sumUtil = glob.otherStats->getSumUtil();
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/totalUtility.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        file << "sumUtilByAllAgents_" << glob.SIM_NAME << ",";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            file << sumUtil[i] << ",";
        }
        file << "\n";
        file.close();
    }
    if (glob.saveDatabaseSet) {
        allResults[0].push_back(sumUtil);
    }

    /* Save each group's total utility. */
    if (glob.NUM_AGENT_GROUPS > 1) {
        vector<vector<double> > sumUtilByGroup = glob.otherStats->getSumUtilByGroup();
        if (glob.END_SAVE) {
            ofstream file;
            string filePath = glob.SAVE_FOLDER + "/totalUtility.csv";
            file.open(filePath.c_str(), ios::app);
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                file << "sumUtilByGroup_" << gId << ",";
                for (int i = 0; i < glob.NUM_DAYS; i++) {
                    file << sumUtilByGroup[gId][i] << ",";
                }
                file << "\n";
            }
            file << "\n";
            file.close();
        }
        if (glob.saveDatabaseSet) {
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                allResults[gId+1].push_back(sumUtilByGroup[gId]);
            }
        }
        sumUtilByGroup.clear();
    }
    sumUtil.clear();
}

/**
 * Save the mean utility by all agents and by each group of each day into a csv file.
 */
void Utils::saveMeanUtility()
{
    /* mean utility per agent */
    vector<double> sumUtil = glob.otherStats->getSumUtil();
    vector<int> activeAgents = glob.otherStats->getActiveAgents();
    vector<double> results;
    for (int i = 0; i < glob.NUM_DAYS; i++) {
        results.push_back( (double)sumUtil[i] / (double)activeAgents[i]);
    }
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/meanUtility.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        file << "meanUtilPerAgent,";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            file << results[i] << ",";
        }
        file << "\n";
        file.close();
    }
    if (glob.saveDatabaseSet) {
        allResults[0].push_back(results);
    }

    /* mean utility per group */
    if (glob.NUM_AGENT_GROUPS > 1) {
        vector<vector<double> > sumUtilByGroup = glob.otherStats->getSumUtilByGroup();
        vector<vector<int> > activeGroupAgents = glob.otherStats->getActiveGroupAgents();
        vector<vector<double> > groupResults;
        for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
            groupResults.push_back(vector<double>());
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                groupResults[i].push_back((double)sumUtilByGroup[i][j] / (double)activeGroupAgents[i][j]);
            }
        }
        if (glob.END_SAVE) {
            ofstream file;
            string filePath = glob.SAVE_FOLDER + "/meanUtility.csv";
            file.open(filePath.c_str(), ios::app);
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                file << "meanUtilByGroup_" << i << "_" << glob.SIM_NAME << ",";
                for (int j = 0; j < glob.NUM_DAYS; j++) {
                    file << groupResults[i][j] << ",";
                }
                file << "\n";
            }
            file << "\n";
            file.close();
        }
        if (glob.saveDatabaseSet) {
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                    allResults[i+1].push_back(groupResults[i]);
            }
        }
        activeGroupAgents.clear();
        sumUtilByGroup.clear();
    }
    sumUtil.clear();
    activeAgents.clear();
}

/**
 * Save the mean units gathered by all agents and by each group of each day into a csv file.
 */
void Utils::saveUnitsGathered()
{
    vector<int> resGath = glob.productionStats->getResGathered();
    vector<int> activeAgents = glob.otherStats->getActiveAgents();
    vector<double> results;
    for (int i = 0; i < glob.NUM_DAYS; i++) {
        results.push_back( (double) resGath[i] / (double) activeAgents[i] );
    }
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/unitsGathered.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        file << "unitsGatheredPerActiveAgent_" << glob.SIM_NAME << ",";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            file << results[i] << ",";
        }
        file << "\n";
        file.close();
    }
    if (glob.saveDatabaseSet) {
        allResults[0].push_back(results);
    }
    results.clear();

    /* unitsGathered per group */
    if (glob.NUM_AGENT_GROUPS > 1) {
        vector<vector<int> > resGatheredByGroup = glob.productionStats->getResGatheredByGroup();
        vector<vector<int> > activeGroupAgents = glob.otherStats->getActiveGroupAgents();
        vector<vector<double> > groupResults;
        for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
            groupResults.push_back(vector<double>());
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                groupResults[i].push_back((double)resGatheredByGroup[i][j] / (double)activeGroupAgents[i][j]);
            }
        }
        if (glob.END_SAVE) {
            ofstream file;
            string filePath = glob.SAVE_FOLDER + "/unitsGathered.csv";
            file.open(filePath.c_str(), ios::app);
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                file << "unitsGatheredByGroup_" << i << "_" << glob.SIM_NAME << ",";
                for (int j = 0; j < glob.NUM_DAYS; j++) {
                    file << groupResults[i][j] << ",";
                }
                file << "\n";
            }
            file << "\n";
            file.close();
        }
        if (glob.saveDatabaseSet) {
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                allResults[i+1].push_back(groupResults[i]);
            }
        }
        groupResults.clear();
        activeGroupAgents.clear();
        resGatheredByGroup.clear();
    }
    resGath.clear();
    activeAgents.clear();
}

/**
 * Save the mean units held by all agents and by each group of each day into a csv file.
 */
void Utils::saveUnitsHeld()
{
    /* unitsHeld per agent */
    vector<int> sumRes = glob.otherStats->getSumRes();
    vector<int> activeAgents = glob.otherStats->getActiveAgents();
    vector<double> results;
    for (int i = 0; i < glob.NUM_DAYS; i++) {
        results.push_back((double) sumRes[i] / (double) activeAgents[i]);
    }
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/unitsHeld.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        file << "unitsHeldPerActiveAgent_" << glob.SIM_NAME << ",";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            file << results[i] << ",";
        }
        file << "\n";
        file.close();
    }
    if (glob.saveDatabaseSet) {
        allResults[0].push_back(results);
    }

    /* unitsHeld per group */
    if (glob.NUM_AGENT_GROUPS > 1) {
        vector<vector<int> > resHeldByGroup = glob.otherStats->getSumResByGroup();
        vector<vector<int> > activeGroupAgents = glob.otherStats->getActiveGroupAgents();
        vector<vector<double> > groupResults;
        for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
            groupResults.push_back(vector<double>());
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                groupResults[i].push_back((double)resHeldByGroup[i][j] / (double)activeGroupAgents[i][j]);
            }
        }
        if (glob.END_SAVE) {
            ofstream file;
            string filePath = glob.SAVE_FOLDER + "/unitsHeld.csv";
            file.open(filePath.c_str(), ios::app);
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                file << "unitsHeldByGroup_" << i << "_" << glob.SIM_NAME << ",";
                for (int j = 0; j < glob.NUM_DAYS; j++) {
                    file << groupResults[i][j] << ",";
                }
                file << "\n";
            }
            file << "\n";
            file.close();
        }
        if (glob.saveDatabaseSet) {
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                allResults[i+1].push_back(groupResults[i]);
            }
        }
        groupResults.clear();
        activeGroupAgents.clear();
        resHeldByGroup.clear();
    }
    sumRes.clear();
    activeAgents.clear();
}

/**
 * Save the number of all resources traded for each device and for resources 
 *     per agent, per group, across group, and within each group into a csv file.
 */
void Utils::saveUnitsTraded()
{
    /* Per Agent */
    vector<int> activeAgents = glob.otherStats->getActiveAgents();

    /* unitsTradedForDevice per agent */
    vector<int> resTradeForDeviceVolume = glob.tradeStats->getResTradeForDeviceVolume();
    vector<int> resTradeForDeviceVolumeCrossGroup = glob.tradeStats->getResTradeForDeviceVolumeCrossGroup();
    vector<vector<int> > resTradedForDeviceVolumeWithinGroup = glob.tradeStats->getResTradeForDeviceVolumeWithinGroup();
    vector<int> resTradeVolume = glob.tradeStats->getResTradeVolume();
    vector<int> resTradeVolumeCrossGroup = glob.tradeStats->getResTradeVolumeCrossGroup();
    vector<vector<int> > resTradeVolumeWithinGroup = glob.tradeStats->getResTradeVolumeWithinGroup();

    vector<double> resultsForDev, resultsForDevCrossGroup, resultsForDevWithinGroup;
    vector<double> resultsForRes, resultsForResCrossGroup, resultsForResWithinGroup;
    for (int i = 0; i < glob.NUM_DAYS; i++) {
        resultsForDev.push_back((double)resTradeForDeviceVolume[i] / (double)activeAgents[i]);
        resultsForDevCrossGroup.push_back((double)resTradeForDeviceVolumeCrossGroup[i] / (double)activeAgents[i]);

        // The average units Traded within all groups
        int temp;
        for (int j = 0; j < glob.NUM_AGENT_GROUPS; j++) {
            temp += resTradedForDeviceVolumeWithinGroup[j][i];
        }
        resultsForDevWithinGroup.push_back((double)temp / (double)activeAgents[i]);
        temp = 0;

        resultsForRes.push_back((double)resTradeVolume[i] / (double)activeAgents[i]);
        resultsForResCrossGroup.push_back((double)resTradeVolumeCrossGroup[i] / (double)activeAgents[i]);
        for (int j = 0; j < glob.NUM_AGENT_GROUPS; j++) {
            temp += resTradeVolumeWithinGroup[j][i];
        }
        resultsForResWithinGroup.push_back((double)temp / (double)activeAgents[i]);
        temp = 0;
    }
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/unitsTraded.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        file << "unitsTradedForDeviceAgent_" << glob.SIM_NAME << ",";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            file << resultsForDev[i] << ",";
        }
        file << "\nunitsTradedForResPerActiveAgent_" << glob.SIM_NAME << ",";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            file << resultsForRes[i] << ",";
        }
        file << "\n";
        file.close();
    }
    if (glob.saveDatabaseSet) {
        allResults[0].push_back(resultsForDev);
        /*
         * Still save crossGroup and withinGroup to be consistent in the database.
         */
        allResults[0].push_back(resultsForDevCrossGroup);
        allResults[0].push_back(resultsForDevWithinGroup);
        allResults[0].push_back(resultsForRes);
        allResults[0].push_back(resultsForResCrossGroup);
        allResults[0].push_back(resultsForResWithinGroup);
    }

    /* Per Group */
    if (glob.NUM_AGENT_GROUPS > 1) {
        vector<vector<int> > activeGroupAgents = glob.otherStats->getActiveGroupAgents();

        /* unitsTradedForDevice per group */
        vector<vector<int> > resTradedForDeviceVolumeByGroup = glob.tradeStats->getResTradeForDeviceVolumeByGroup();
        vector<vector<int> > resTradeVolumeByGroup = glob.tradeStats->getResTradeVolumeByGroup();

        vector<vector<double> > groupResultsForDev, groupResultsForDevWithinGroup;
        vector<vector<double> > groupResultsForRes, groupResultsForResWithinGroup;
        for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
            groupResultsForDev.push_back(vector<double>());
            groupResultsForDevWithinGroup.push_back(vector<double>());
            groupResultsForRes.push_back(vector<double>());
            groupResultsForResWithinGroup.push_back(vector<double>());
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                groupResultsForDev[i].push_back( (double)resTradedForDeviceVolumeByGroup[i][j] / (double)activeGroupAgents[i][j]);
                groupResultsForDevWithinGroup[i].push_back((double)resTradedForDeviceVolumeWithinGroup[i][j] / (double)activeGroupAgents[i][j]);
                groupResultsForRes[i].push_back((double)resTradeVolumeByGroup[i][j] / (double)activeGroupAgents[i][j]);
                groupResultsForResWithinGroup[i].push_back((double)resTradeVolumeWithinGroup[i][j] / (double)activeGroupAgents[i][j]);
            }
        }
        if (glob.END_SAVE) {
            ofstream file;
            string filePath = glob.SAVE_FOLDER + "/unitsTraded.csv";
            file.open(filePath.c_str(), ios::app);
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                file << "unitsTradedForDeviceByGroup_" << i << "_" << glob.SIM_NAME << ",";
                for (int j = 0; j < glob.NUM_DAYS; j++) {
                    file << groupResultsForDev[i][j] << ",";
                }
                file << "\n";
            }
            file << "unitsTradedForDeviceCrossGroups_" << glob.SIM_NAME << ",";
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                file << resultsForDevCrossGroup[j] << ",";
            }
            file << "\n";
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                file << "unitsTradedForDeviceWithinGroup " << i << "_" << glob.SIM_NAME << ",";
                for (int j = 0; j < glob.NUM_DAYS; j++) {
                    file << groupResultsForDevWithinGroup[i][j] << ",";
                }
                file << "\n";
            }
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                file << "unitsTradedForResByGroup_" << i << "_" << glob.SIM_NAME << ",";
                for (int j = 0; j < glob.NUM_DAYS; j++) {
                    file << groupResultsForRes[i][j] << ",";
                }
                file << "\n";
            }
            file << "unitsTradedForResourceCrossGroups_" << glob.SIM_NAME << ",";
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                file << resultsForResCrossGroup[j] << ",";
            }
            file << "\n";
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                file << "unitsTradedForResourceWithinGroup " << i << "_" << glob.SIM_NAME << ",";
                for (int j = 0; j < glob.NUM_DAYS; j++) {
                    file << groupResultsForResWithinGroup[i][j] << ",";
                }
                file << "\n";
            }
            file << "\n";
            file.close();
        }
        if (glob.saveDatabaseSet) {
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                allResults[i+1].push_back(groupResultsForDev[i]);
                allResults[i+1].push_back(resultsForDevCrossGroup);     // Only have two groups now. So same for the two groups.
                allResults[i+1].push_back(groupResultsForDevWithinGroup[i]);
                allResults[i+1].push_back(groupResultsForRes[i]);
                allResults[i+1].push_back(resultsForResCrossGroup);     // Only have two groups now. So same for the two groups.
                allResults[i+1].push_back(groupResultsForResWithinGroup[i]);
            }
        }
        activeGroupAgents.clear();
        resTradedForDeviceVolumeByGroup.clear();
        resTradeVolumeByGroup.clear();
        groupResultsForDev.clear();
        resultsForDevCrossGroup.clear();
        groupResultsForDevWithinGroup.clear();
        groupResultsForRes.clear();
        groupResultsForRes.clear();
        groupResultsForResWithinGroup.clear();
    }
    resultsForDev.clear();
    resultsForDevWithinGroup.clear();
    resultsForRes.clear();
    resultsForResWithinGroup.clear();
    activeAgents.clear();
    resTradeForDeviceVolume.clear();
    resTradeForDeviceVolumeCrossGroup.clear();
    resTradeVolume.clear();
    resTradeVolumeCrossGroup.clear();
}

/**
 * Save the number of made devices for each device type per agent and per group.
 */
void Utils::saveDeviceMade()
{
    /* device made per agent */
    vector<int> activeAgents = glob.otherStats->getActiveAgents();
    string devicesStr[] = { "TOOL", "MACHINE", "FACTORY", "INDUSTRY", "DEVMACHINE", "DEVFACTORY" };
    vector< vector<int> > devicesMade = glob.productionStats->getDevicesMade();
    vector<vector<double> > results;
    for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
        results.push_back(vector<double>());
        for (int j = 0; j < glob.NUM_DAYS; j++) {
            results[i].push_back((double)devicesMade[i][j] / (double)activeAgents[j]);
        }
    }
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/deviceMade.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
            file << devicesStr[i] << "MadePerActiveAgent_" << glob.SIM_NAME << ",";
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                file << results[i][j] << ",";
            }
            file << "\n";
        }
        file << "\n";
        file.close();
    }
    if (glob.saveDatabaseSet) {
        for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
            allResults[0].push_back(results[i]);
        }
    }
    results.clear();

    /* device made per group */
    if (glob.NUM_AGENT_GROUPS > 1) {
        vector< vector< vector <int> > > devicesMadeByGroup = glob.productionStats->getDevicesMadeByGroup();
        vector<vector<int> > activeGroupAgents = glob.otherStats->getActiveGroupAgents();
        vector<vector<vector<double> > > groupResults;
        for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
            groupResults.push_back(vector<vector<double> >());
            for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
                groupResults[gId].push_back(vector<double>());
                for (int j = 0; j < glob.NUM_DAYS; j++) {
                    groupResults[gId][i].push_back( (double) devicesMadeByGroup[gId][i][j] / (double) activeGroupAgents[gId][j] );
                }
            }
        }
        if (glob.END_SAVE) {
            ofstream file;
            string filePath = glob.SAVE_FOLDER + "/deviceMade.csv";
            file.open(filePath.c_str(), ios::app);
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
                    file << devicesStr[i] << "MadeByGroup" << gId << "_" << glob.SIM_NAME << ",";
                    for (int j = 0; j < glob.NUM_DAYS; j++) {
                        file << groupResults[gId][i][j] << ",";
                    }
                    file << "\n";
                }
            }
            file << "\n";
            file.close();
        }
        if (glob.saveDatabaseSet) {
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
                    allResults[gId+1].push_back(groupResults[gId][i]);
                }
            }
        }
        devicesMadeByGroup.clear();
        activeGroupAgents.clear();
        groupResults.clear();
    }

    activeAgents.clear();
    devicesMade.clear();
}


/**
 * Save the number of minutes used to gather resources with each type of device per agent and per group,
 *     and the number of minutes used to make devices per agent and per group,
 *     and the number of minutes used to gather resources without devices per agent and per group.
 */
void Utils::saveTotalTimeUsage()
{
    /* Per Agent: time spent gathering with device, time spent mMaking device, time spent gathering with device */
    vector<int> activeAgents = glob.otherStats->getActiveAgents();
    string devicesStr[] = { "TOOL", "MACHINE", "FACTORY", "INDUSTRY", "DEVMACHINE", "DEVFACTORY" };
    vector< vector<int> > timeSpentGatheringWithDevice = glob.productionStats->getTimeSpentGatheringWithDevice();
    vector< vector<int> > timeSpentMakingDevices = glob.productionStats->getTimeSpentMakingDevices();
    vector<int> timeSpentGatheringWithoutDevice = glob.productionStats->getTimeSpentGatheringWithoutDevice();
    vector<vector<double> > resultsGatherWithDev, resultsMakeDev;
    vector<double> resultsGatherWithoutDev;
    for (int i = 0; i < NUM_DEVICE_TYPES - 2; i++) {
        resultsGatherWithDev.push_back(vector<double>());
        for (int j = 0; j < glob.NUM_DAYS; j++) {
            resultsGatherWithDev[i].push_back( (double) timeSpentGatheringWithDevice[i][j] / (double) activeAgents[j] );
        }
    }
    for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
        resultsMakeDev.push_back(vector<double>());
        for (int j = 0; j < glob.NUM_DAYS; j++) {
            resultsMakeDev[i].push_back( (double) timeSpentMakingDevices[i][j] / (double) activeAgents[j] );
        }
    }
    for (int i = 0; i < glob.NUM_DAYS; i++) {
        resultsGatherWithoutDev.push_back((double) timeSpentGatheringWithoutDevice[i] / (double) activeAgents[i]);
    }
    if (glob.END_SAVE) {
        /*
         * Save the time usage into totalTimeUsage.csv
         */
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/totalTimeUsage.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        for (int i = 0; i < NUM_DEVICE_TYPES - 2; i++) {
            file << devicesStr[i] << "_timeGatheringWithDevicePerActiveAgent_" << glob.SIM_NAME << ",";
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                file << resultsGatherWithDev[i][j] << ",";
            }
            file << "\n";
        }
        for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
            file << devicesStr[i] << " timeMakingDevicesPerActiveAgent_" << glob.SIM_NAME << ",";
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                file << resultsMakeDev[i][j] << ",";
            }
            file << "\n";
        }
        file << "timeGatheringWithoutDevicePerActiveAgent_" << glob.SIM_NAME << ",";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            file << resultsGatherWithoutDev[i] << ",";
        }
        file << "\n" << "\n";
        file.close();

        /*
         * Save technology index into technology.csv:
         * percent of the time to extract using each level of technology.
         * %industry * 4 + %factory * 3 + %machine * 2 + %tool * 1
         */
        filePath = glob.SAVE_FOLDER + "/technology.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        file << "technologyIndex_" << glob.SIM_NAME << ",";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            double totalGatherTime = 0;
            double techIndex;
            for (int dev = 0; dev < NUM_DEVICE_TYPES-2; dev++) {
                totalGatherTime += timeSpentGatheringWithDevice[dev][i];
            }
            totalGatherTime += timeSpentGatheringWithoutDevice[i];
            techIndex = timeSpentGatheringWithDevice[0][i] / totalGatherTime +
                        timeSpentGatheringWithDevice[1][i] * 2 / totalGatherTime +
                        timeSpentGatheringWithDevice[2][i] * 3 / totalGatherTime +
                        timeSpentGatheringWithDevice[3][i] * 4 / totalGatherTime;
            file << techIndex << ",";
        }
        file << "\n";
        file.close();
    }
    if (glob.saveDatabaseSet) {
        for (int i = 0; i < NUM_DEVICE_TYPES - 2; i++) {
            allResults[0].push_back(resultsGatherWithDev[i]);
        }
        for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
            allResults[0].push_back(resultsMakeDev[i]);
        }
        allResults[0].push_back(resultsGatherWithoutDev);
    }
    activeAgents.clear();
    timeSpentGatheringWithDevice.clear();
    timeSpentMakingDevices.clear();
    timeSpentGatheringWithoutDevice.clear();
    resultsGatherWithDev.clear();
    resultsMakeDev.clear();
    resultsGatherWithoutDev.clear();

    /* Per Group: time spent gathering with device, time spent mMaking device, time spent gathering with device */
    if (glob.NUM_AGENT_GROUPS > 1) {
        vector< vector<vector<int> > > timeSpentGatheringWithDeviceByGroup = glob.productionStats->getTimeSpentGatheringWithDeviceByGroup();
        vector< vector<vector<int> > > timeSpentMakingDevicesByGroup = glob.productionStats->getTimeSpentMakingDevicesByGroup();
        vector<vector<int> > timeSpentGatheringWithoutDeviceByGroup = glob.productionStats->getTimeSpentGatheringWithoutDeviceByGroup();
        vector<vector<int> > activeGroupAgents = glob.otherStats->getActiveGroupAgents();
        vector<vector<vector<double> > > groupResultsGatherWithDev, groupResultsMakeDev;
        vector<vector<double> > groupResultsGatherWithoutDev;
        for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
            groupResultsGatherWithDev.push_back(vector<vector<double> >());
            for (int i = 0; i < NUM_DEVICE_TYPES - 2; i++) {
                groupResultsGatherWithDev[gId].push_back(vector<double>());
                for (int j = 0; j < glob.NUM_DAYS; j++) {
                    groupResultsGatherWithDev[gId][i].push_back((double) timeSpentGatheringWithDeviceByGroup[gId][i][j] / (double) activeGroupAgents[gId][j]);
                }
            }
        }
        for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
            groupResultsMakeDev.push_back(vector<vector<double> >());
            for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
                groupResultsMakeDev[gId].push_back(vector<double>());
                for (int j = 0; j < glob.NUM_DAYS; j++) {
                    groupResultsMakeDev[gId][i].push_back((double) timeSpentMakingDevicesByGroup[gId][i][j] / (double) activeGroupAgents[gId][j]);
                }
            }
        }
        for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
            groupResultsGatherWithoutDev.push_back(vector<double>());
            for (int i = 0; i < glob.NUM_DAYS; i++) {
                groupResultsGatherWithoutDev[gId].push_back((double) timeSpentGatheringWithoutDeviceByGroup[gId][i] / (double)activeGroupAgents[gId][i]);
            }
        }
        if (glob.END_SAVE) {
            ofstream file;
            string filePath = glob.SAVE_FOLDER + "/totalTimeUsage.csv";
            file.open(filePath.c_str(), ios::app);
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (int i = 0; i < NUM_DEVICE_TYPES - 2; i++) {
                    file << devicesStr[i] << "_timeGatheringWithDeviceByGroup " << gId << "_" << glob.SIM_NAME << ",";
                    for (int j = 0; j < glob.NUM_DAYS; j++) {
                        file << groupResultsGatherWithDev[gId][i][j] << ",";
                    }
                    file << "\n";
                }
            }
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
                    file << devicesStr[i] << " timeMakingDevicesByGroup " << gId << "_" << glob.SIM_NAME << ",";
                    for (int j = 0; j < glob.NUM_DAYS; j++) {
                        file << groupResultsMakeDev[gId][i][j] << ",";
                    }
                    file << "\n";
                }
            }
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                file << "timeGatheringWithoutDeviceByGroup " << gId << "_" << glob.SIM_NAME << ",";
                for (int i = 0; i < glob.NUM_DAYS; i++) {
                    file << groupResultsGatherWithoutDev[gId][i] << ",";
                }
                file << "\n";
            }
            file << "\n";
            file.close();
        }
        if (glob.saveDatabaseSet) {
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (int i = 0; i < NUM_DEVICE_TYPES - 2; i++) {
                    allResults[gId+1].push_back(groupResultsGatherWithDev[gId][i]);
                }
                for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
                    allResults[gId+1].push_back(groupResultsMakeDev[gId][i]);
                }
                allResults[gId+1].push_back(groupResultsGatherWithoutDev[gId]);
            }
        }
        activeGroupAgents.clear();
        timeSpentGatheringWithDeviceByGroup.clear();
        timeSpentMakingDevicesByGroup.clear();
        timeSpentGatheringWithoutDeviceByGroup.clear();
        groupResultsGatherWithDev.clear();
        groupResultsMakeDev.clear();
        groupResultsGatherWithoutDev.clear();
    }
}

/**
 * Save the percent of resources gathered by each type of device.
 */
void Utils::saveDevicePercents()
{
    string devicesStr[] = { "TOOL", "MACHINE", "FACTORY", "INDUSTRY", "DEVMACHINE", "DEVFACTORY" };
    vector< vector<double> > percentResGatheredByDevice = glob.productionStats->getPercentResGatheredByDevice();
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/devicePercents.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        for (int i = 0; i < NUM_DEVICE_TYPES - 2; i++) {
            file << devicesStr[i] << "percentResGatheredWith_" << glob.SIM_NAME << ",";
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                file << percentResGatheredByDevice[i][j] << ",";
            }
            file << "\n";
        }
        file << "\n";
        file.close();
    }
    if (glob.saveDatabaseSet) {
        for (int i = 0; i < NUM_DEVICE_TYPES - 2; i++) {
            allResults[0].push_back(percentResGatheredByDevice[i]);
        }
        /*
         * If the results are not calculated by groups but the program runs under heterogeneous,
         * then when saved to database, all groups have the same results as all agents.
         */
        if (glob.NUM_AGENT_GROUPS > 1) {
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (int i = 0; i < NUM_DEVICE_TYPES - 2; i++) {
                    allResults[gId+1].push_back(percentResGatheredByDevice[i]);
                }
            }
        }

    }
    percentResGatheredByDevice.clear();
}

/**
 * Save the percent of made devices by DEVMACHINE and DEVFACTORY by all agents and by each group's all agents.
 */
void Utils::saveDevDevice()
{
    int totalMade = 0;
    string devicesStr[] = { "TOOL", "MACHINE", "FACTORY", "INDUSTRY", "DEVMACHINE", "DEVFACTORY" };

    /* percent of devices made with devDevice by total agents */
    vector< vector<int> > devicesMade = glob.productionStats->getDevicesMade();
    vector< vector<int> > devicesMadeWithDevDevice = glob.productionStats->getDevicesMadeWithDevDevice();
    vector<vector<double> > results;
    for (int i = 4; i < NUM_DEVICE_TYPES; i++) {
        results.push_back(vector<double>());
        for (int j = 0; j < glob.NUM_DAYS; j++) {
            if (devicesStr[i] == "DEVMACHINE") {
                totalMade = devicesMade[0][j];
            } else if (devicesStr[i] == "DEVFACTORY") {
                totalMade = devicesMade[1][j];
            }
            if (totalMade > 0) {
                results[i-4].push_back((double) devicesMadeWithDevDevice[i][j] / (double) totalMade);
            } else {
                results[i-4].push_back(0.0);
            }
        }
    }
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/devDeviceUse.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        for (int i = 4; i < NUM_DEVICE_TYPES; i++) {
            file << devicesStr[i] << "Use_" << glob.SIM_NAME  << ",";
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                file << results[i-4][j] << ",";
            }
            file << "\n";
        }
        file.close();
    }
    if (glob.saveDatabaseSet) {
        for (int i = 4; i < NUM_DEVICE_TYPES; i++) {
            allResults[0].push_back(results[i-4]);
        }
    }
    results.clear();
    devicesMade.clear();
    devicesMadeWithDevDevice.clear();

    /* percent of devices made with devDevice by each group */
    if (glob.NUM_AGENT_GROUPS > 1) {
        vector< vector< vector <int> > > devicesMadeByGroup = glob.productionStats->getDevicesMadeByGroup();
        vector<vector<vector<int> > > devicesMadeWithDevDeviceByGroup = glob.productionStats->getDevicesMadeWithDevDeviceByGroup();
        vector<vector<vector<double> > > groupResults;
        for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
            groupResults.push_back(vector<vector<double> >());
            for (int i = 4; i < NUM_DEVICE_TYPES; i++) {
                groupResults[gId].push_back(vector<double>());
                for (int j = 0; j < glob.NUM_DAYS; j++) {
                    if (devicesStr[i] == "DEVMACHINE") {
                        totalMade = devicesMadeByGroup[gId][0][j];
                    } else if (devicesStr[i] == "DEVFACTORY") {
                        totalMade = devicesMadeByGroup[gId][1][j];
                    }
                    if (totalMade > 0) {
                        groupResults[gId][i-4].push_back((double) devicesMadeWithDevDeviceByGroup[gId][i][j] / (double) totalMade );
                    } else {
                        groupResults[gId][i-4].push_back(0.0);
                    }
                }
            }
        }
        if (glob.END_SAVE) {
            ofstream file;
            string filePath = glob.SAVE_FOLDER + "/devDeviceUse.csv";
            file.open(filePath.c_str(), ios::app);
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (int i = 4; i < NUM_DEVICE_TYPES; i++) {
                    file << "Group " << gId << "_" << devicesStr[i] << " Use_" << glob.SIM_NAME  << ",";
                    for (int j = 0; j < glob.NUM_DAYS; j++) {
                        file << groupResults[gId][i-4][j] << ",";
                    }
                    file << "\n";
                }
            }
            file << "\n";
            file.close();
        }
        if (glob.saveDatabaseSet) {
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (int i = 4; i < NUM_DEVICE_TYPES; i++) {
                    allResults[gId+1].push_back(groupResults[gId][i-4]);
                }
            }
        }
        groupResults.clear();
        devicesMadeByGroup.clear();
        devicesMadeWithDevDeviceByGroup.clear();
    }
}

/**
 * save deviceComplexity for each day
 */
void Utils::saveDeviceComplexity()
{
    vector< vector<double> > percentResGatheredByDevice = glob.productionStats->getPercentResGatheredByDevice();
    double complexityToday;
    vector<double> results;
    for (int i = 0; i < glob.NUM_DAYS; i++) {
        complexityToday = 0.0;
        for (int j = 0; j < 4; j++) {
            complexityToday += percentResGatheredByDevice[j][i] * glob.RES_IN_DEV[j];
        }
        results.push_back(complexityToday);
    }
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/complexity.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        file << "complexity_" << glob.SIM_NAME << ",";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            file << results[i] << ",";
        }
        file << "\n";
        file.close();
    }
    if (glob.saveDatabaseSet) {
        allResults[0].push_back(results);
        /*
         * If the results are not calculated by groups but the program runs under heterogeneous,
         * then when saved to database, all groups have the same results as all agents.
         */
        if (glob.NUM_AGENT_GROUPS > 1) {
            for (int i = 0; i < glob.NUM_AGENT_GROUPS; i++) {
                allResults[i+1].push_back(results);
            }
        }
    }
    percentResGatheredByDevice.clear();
    results.clear();
}


/**
 * save the number of discovered / invented devices by all agents for each day
 */
void Utils::saveDiscoveredDevices()
{
    vector< vector<int> > numberOfInventedDevices = glob.otherStats->getNumberOfInventedDevices();
    string devicesStr[] = { "TOOL", "MACHINE", "FACTORY", "INDUSTRY", "DEVMACHINE", "DEVFACTORY" };
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/deviceDiscovered.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
            file << devicesStr[i] << "Discovered_" << glob.SIM_NAME  << ",";
            for (int j = 0; j < glob.NUM_DAYS; j++) {
                file << numberOfInventedDevices[i][j] << ",";
            }
            file << "\n";
        }
        file << "\n";
        file.close();
    }
    if (glob.saveDatabaseSet) {
        vector<vector<double> > results;
        for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
            vector<double> temp(numberOfInventedDevices[i].begin(), numberOfInventedDevices[i].end());
            results.push_back(temp);
        }
        for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
            allResults[0].push_back(results[i]);
        }
        /*
         * If the results are not calculated by groups but the program runs under heterogeneous,
         * then when saved to database, all groups have the same results as all agents.
         */
        if (glob.NUM_AGENT_GROUPS > 1) {
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (int i = 0; i < NUM_DEVICE_TYPES; i++) {
                    allResults[gId+1].push_back(results[i]);
                }
            }
        }
        results.clear();
    }
    numberOfInventedDevices.clear();
}


/**
 * At the end of each day,
 * 1. record the remaining amount of each resource in the environment.
 *    When a resource is unlimited, record it as -1.
 * 2. record the number of resources that are at or below RES_CONSIDERED_DEPLETED.
 */
void Utils::saveResAmount()
{
    vector< vector<long int> > resAmount = glob.otherStats->getResAmount();
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/resAmount.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
	for (int resId = 0; resId < glob.NUM_RESOURCES; resId++) {
	    file << "res" << resId << "Amount_" << glob.SIM_NAME << ",";
            for (int i = 0; i < glob.NUM_DAYS; i++) {
                file << resAmount[resId][i] << ",";
            }
            file << "\n";
        }
        file << "\n\n";
        file.close();
    }
}

/**
 * Save the number of depleted resources into depletedRes.csv.
 */
void Utils::saveDepletedRes()
{
    vector<int> depletedRes = glob.otherStats->getDepletedRes();
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/depletedRes.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        file << "numberOfDepletedRes_" << glob.SIM_NAME << ",";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            file << depletedRes[i] << ",";
        }
        file << "\n";
        file.close();
    }
}

/**
 * Save the environment into environment.csv file.
 */
void Utils::saveEnvironment()
{
    vector<double> environment = glob.otherStats->getEnvironment();
    if (glob.END_SAVE) {
        ofstream file;
        string filePath =glob.SAVE_FOLDER + "/environment.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        file << "resAmountRatio_" << glob.SIM_NAME << ",";
        for (int i = 0; i < glob.NUM_DAYS; i++) {
            file << environment[i] << ",";
        }
        file << "\n";
        file.close();
    }
}

void Utils::saveRecipe()
{
    vector< vector< vector< vector<int> > > > recipe = glob.productionStats->getRecipe();
    string devicesStr[] = { "TOOL", "MACHINE", "FACTORY", "INDUSTRY", "DEVMACHINE", "DEVFACTORY" };
    if (glob.END_SAVE) {
        ofstream file;
        string filePath = glob.SAVE_FOLDER + "/recipe.csv";
        headerByDay(file, filePath);
        file.open(filePath.c_str(), ios::app);
        for (int type = 0; type < glob.getNumDeviceTypes(); type++) {
            for (int resId = 0; resId < glob.NUM_RESOURCES; resId++) {
                file << devicesStr[type] << resId << "_" << glob.SIM_NAME << ",";
                for (int i = 0; i < glob.NUM_DAYS; i++) {
                    if (recipe[type][resId][i].size() != 0) {
                        string comp = "";
                        for (unsigned j = 0; j < recipe[type][resId][i].size(); j++) {
                            comp = comp + boost::lexical_cast<string>(recipe[type][resId][i][j]) + "_";
                        }
                        file << comp << ",";
                    } else {
                        file << "NULL,";
                    }
                }
                file << "\n";
            }
        }
        file << "\n\n";
        file.close();
    }
}

/**
 * Save results into csv files -- part of graph.py in python.
 * Different from saveResults() in utils.py
 */
void Utils::saveResults()
{
    /*
     * Save the config files into configFiles folder
     * The following save..() functions will write results into different csv files.
     */
    if (glob.END_SAVE) { 
        int command;
        string conf = glob.SAVE_FOLDER + "/configFiles";
        string commandLine;
        boost::filesystem::create_directory(conf);
        if (glob.configAgentCSV != "") {
            commandLine = "cp " + glob.configAgentFilename + " "
                    + glob.configAgentCSV + " "
                    + glob.configFilename + " " + conf;
        } else {
            commandLine = "cp " + glob.configAgentFilename + " " + glob.configFilename + " " + conf;
        }
        command = system(commandLine.c_str());
    } else {
        // TODO: we are not doing this in C++...  remove this else part?
        /*
        while (options.graph):
            ag.showGraph()
            command = raw_input('Enter c to continue (anything else to quit):')
            if command.lower() == 'c':
                ag.showGraph()
            else:
                break
                    }
        */
    }

    /**
     * Save all the results into four csv files,
     * and import them into database.
     */
    if (glob.saveDatabaseSet) {
        ofstream file;
        /*
         * First csv file: batch.csv.
         * Stores batchID; number of agents, resources, groups of agents, days;
         *          scenario ID and hetero ID.
         */
        string filePath = glob.saveDatabaseFolder + "/batch.csv";
        if ( !boost::filesystem::exists(filePath)) {
            file.open(filePath.c_str());
            file << "batchID,num_agents,num_resources,num_days,num_groups,scenario,hetero,removeAgentDay,removeResDay\n";
            file.close();
        }
        file.open(filePath.c_str(), ios::app);
        file << glob.batchID << "," << glob.NUM_AGENTS << "," << glob.NUM_RESOURCES << ","
                << glob.NUM_DAYS << "," << glob.NUM_AGENT_GROUPS << "," << glob.calcScenarioNumber() << ","
                << glob.heteroID << "," << glob.removeAgentDay << "," << glob.removeResDay << ",\n";
        file.close();

        /*
         * Second csv file: interation.csv
         * Stores iterationID -- a UUID parsed from options;
         *          batchID -- a UUID parsed from options, all iterations in one batch have the same batchID;
         *          iteration number -- the nth iteration in this batch.
         */
        filePath = glob.saveDatabaseFolder + "/iteration.csv";
        if ( !boost::filesystem::exists(filePath)) {
            file.open(filePath.c_str());
            file << "iterationID,batchID,iteration number\n";
            file.close();
        }
        file.open(filePath.c_str(), ios::app);
        file << glob.iterationID << "," << glob.batchID << "," << glob.iterationNum << "\n";
        file.close();

        /*
         * Third csv file: group.csv
         * Stores the properties of each group's agents -- the values specified in aconf file.
         */
        filePath = glob.saveDatabaseFolder + "/group.csv";
        if ( !boost::filesystem::exists(filePath)) {
            file.open(filePath.c_str());
            file << "groupID,iterationID,description,res_tradePower,dev_tradePower,minResEff,maxResEff,inventSpeed,minDevEff,maxDevEff\n";
            file.close();
        }
        file.open(filePath.c_str(), ios::app);
        if (glob.NUM_AGENT_GROUPS == 1) {
            Agent *ag = glob.agent[0];
            // For now, minDevEff and maxDevEff are same for all types of devices of one agent, so use TOOL (0).
            file << "-1," << glob.iterationID << ",AVG,"
                    << ag->resTradePower << "," << ag->devTradePower << ","
                    << ag->resProp[0].minResEffort << "," << ag->resProp[0].maxResEffort << ","
                    << ag->inventSpeed << "," << ag->devProp[0][0].minDeviceEffort << ","
                    << ag->devProp[0][0].maxDeviceEffort << "\n";
        } else {
            file << "-1," << glob.iterationID << ",AVG,,,,,,,,\n";
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                for (int aId = 0; aId < glob.NUM_AGENTS; aId++) {
                    if (glob.agent[aId]->group == gId) {
                        Agent *ag = glob.agent[aId];
                        file << gId << "," << glob.iterationID << ",Group " << gId << ","
                                << ag->resTradePower << "," << ag->devTradePower << ","
                                << ag->resProp[0].minResEffort << "," << ag->resProp[0].maxResEffort << ","
                                << ag->inventSpeed << "," << ag->devProp[0][0].minDeviceEffort << ","
                                << ag->devProp[0][0].maxDeviceEffort << "\n";
                        break;
                    }
                }
            }
        }
        file.close();

        /*
         * Forth csv file: results.csv
         * Stores all the results generated by saveResults().
         * The following save..() functions will write results into results.csv.
         */
        filePath = glob.saveDatabaseFolder + "/results.csv";
        if ( !boost::filesystem::exists(filePath)) {
            file.open(filePath.c_str());
            file << "GroupID,iterationID,Day,"
                    << "gini,HHI_min,HHI_q1,HHI_median,HHI_q3,HHI_max,"
                    << "totalUtility,meanUtil,unitsGathered,unitsHeld,"
                    << "unitsTradedForDev,unitsTradedForDevCrossGroup,unitsTradedForDevWithinGroup,"
                    << "unitsTradedForRes,unitsTradedForResCrossGroup,unitsTradedForResWithinGroup,"
                    << "ToolMade,MachineMade,FactoryMade,IndustryMade,DevMachineMade,DevFactoryMade,"
                    << "ToolGatherTime,MachineGatherTime,FactoryGatherTime,IndustryGatherTime,"
                    << "ToolMakeTime,MachineMakeTime,FactoryMakeTime,IndustryMakeTime,"
                    << "DevMachineMakeTime,DevFactoryMakeTime,timeGatherWithoutDevice,"
                    << "ToolPercentResGathered,MachinePercentResGathered,FactoryPercentResGathered,IndustryPercentResGathered,"
                    << "DevMachineUse,DevFactoryUse,complexity,ToolDiscovered,MachineDiscovered,"
                    << "FactoryDiscovered,IndustryDiscovered,DevMachineDiscovered,DevFactoryDiscovered\n";
            file.close();
        }
        allResults.push_back(vector<vector<double> >());
        if (glob.NUM_AGENT_GROUPS > 1) {
            for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                allResults.push_back(vector<vector<double> >());
            }
        }
    }

    /* Calculate and save the data. */
    if (glob.END_SAVE || glob.saveDatabaseSet) {
        saveGini();
        saveHHIQuartiles();
        saveTotalUtility();
        saveMeanUtility();
        saveUnitsGathered();
        saveUnitsHeld();
        saveUnitsTraded();
        saveRecipe();
        saveDeviceMade();
        saveTotalTimeUsage();
        saveDevicePercents();
        saveDevDevice();
        saveDeviceComplexity();
        saveDiscoveredDevices();
        saveResAmount();
        saveDepletedRes();
        saveEnvironment();
       if (glob.END_SAVE) {
            cout << "Your results have been saved in " << glob.SAVE_FOLDER << endl;
            cout << "The simulation was named " << glob.SIM_NAME << endl;
        }
        if (glob.saveDatabaseSet) {
            /* Wrte all the results stored in allResults into results.csv */
            ofstream file;;
            string filePath = glob.saveDatabaseFolder + "/results.csv";
            file.open(filePath.c_str(), ios::app);
            /* Write the results for all agents */
            for (int day = 0; day < glob.NUM_DAYS; day++) {
                file << "-1," << glob.iterationID << "," << day+1 << ",";
                for (int field = 0; field < 46; field++) {      // The number of fields calculated above.
                    file << allResults[0][field][day] << ",";
                }
                file << "\n";
            }
            file.close();
            /* Write the results for each group if there are more than 1 groups */
            if (glob.NUM_AGENT_GROUPS > 1) {
                file.open(filePath.c_str(), ios::app);
                for (int gId = 0; gId < glob.NUM_AGENT_GROUPS; gId++) {
                    for (int day = 0; day < glob.NUM_DAYS; day++) {
                        file << gId << "," << glob.iterationID << "," << day+1 << ",";
                        for (int field = 0; field < 46; field++) {      // The number of fields calculated above.
                            file << allResults[gId+1][field][day] << ",";   // Group 0 is at index 1
                        }
                        file << "\n";
                    }
                }
                file.close();
            }
            cout << "Your results have been saved in " << glob.saveDatabaseFolder << endl;
            cout << "The unique batch ID is " << endl;
        }
    }
}


/**
 * Save the number of units held, marginal utilities, and average marginal utility ratio
 *     for each resource by agent[0] before it starts work and after the second trade,
 *     and the number of units bought and sold for each resource by agent[0] during the second trade,
 *     and the number of units gathered for each resource by agent[0] during work
 *     into file agentsData.csv under the saving directory specified when start running.
 */
void Utils::saveEndDayData()
{
    ofstream file;
    string filePath = glob.SAVE_FOLDER + "/agentsData.csv";
    if ( !boost::filesystem::exists(filePath)) {
        file.open(filePath.c_str());
        file << "Day,resId,held(before work),MU(before work),avgMURatio(before work),gathered(after work),bought,sold,held(after trade),MU(after trade),avgMURatio(after trade)\n";
        file.close();
    }
    file.open(filePath.c_str(), ios::app);

    // For now, only do one agent
    for (int resId = 0; resId < glob.NUM_RESOURCES; resId++) {
        ResProperties &resPr = glob.agent[0]->resProp[resId];
        double avgMURatioBeforeWork = 0;
        double avgMURatioAfterTrade = 0;
        for (int i = 0; i < glob.NUM_RESOURCES; i++) {
            if (i != resId) {
                avgMURatioBeforeWork += (resPr.beforeWorkMU / glob.agent[0]->resProp[i].beforeWorkMU);
                avgMURatioAfterTrade += (resPr.endDayUtilities / glob.agent[0]->resProp[i].endDayUtilities);
            }
        }
        file << glob.currentDay << "," << resId << ","
                << resPr.beforeWorkHeld << "," << resPr.beforeWorkMU << "," << avgMURatioBeforeWork << "," << resPr.unitsGatheredEndWork << ","
                << (resPr.boughtEndDay - resPr.boughtEndWork) << "," << (resPr.soldEndDay - resPr.soldEndWork) << ","
                << resPr.getHeld() << "," << resPr.endDayUtilities << "," << avgMURatioAfterTrade << ",\n";
    }
    file.close();
}

void Utils::removeAgent(int agentNumber, int day)
{
    glob.agent[agentNumber]->remove();
    glob.activeAgents--;
    glob.NUM_ACTIVE_AGENTS_IN_GROUP[glob.agent[agentNumber]->group]--;
    cout << "current number of agents in group " << glob.agent[agentNumber]->group << " = " << glob.NUM_ACTIVE_AGENTS_IN_GROUP[glob.agent[agentNumber]->group] << endl;
    cout << "Agent " << agentNumber << " has been removed on day " << day << endl;
}

void Utils::removeRes(int resNumber, int day)
{
    glob.res[resNumber].remove();
    for (unsigned int i = 0; i < glob.agent.size(); i++) {
        for (int device = 0; device < 6; device++) {
            glob.agent[i]->devProp[device][resNumber].deviceHeld = 0;
        }
    }
    if (glob.ELIMINATE_RESERVES) {
        for (unsigned int i = 0; i < glob.agent.size(); i++) {
            glob.agent[i]->resProp[resNumber].setHeld(0);
            glob.agent[i]->resProp[resNumber].experience = 0;
        }
    }
    cout << "Res " << resNumber << " has been removed on day " << day << endl;
}

/* TODO NOT IMPLEMENTED YET */
void Utils::saveDayStatus(int day)
{
    cout << "saveDayStatus: NOT IMPLEMENTED YET!" << endl;
}

/* TODO NOT IMPLEMENTED YET */
void Utils::loadDayStatus()
{
    cout << "loadDayStatus: NOT IMPLEMENTED YET!" << endl;
}