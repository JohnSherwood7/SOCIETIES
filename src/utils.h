/**
 * The utils module includes commonly-used functions for use during simulations.
 */

#ifndef _SOC_UTILS_H_
#define _SOC_UTILS_H_

#include "globals.h"

class Utils {
public:
    /**
     * Contains all of the results generated by saving results functions.
     * First indexed by group number (average, 0, 1, ...).
     * Then by fields: gini,HHI (min, q1, median, q3, max), totalUtil, meanUtil,
     *              unitsGathered, unitsHeld, unitsTradedForDev, unitsTradedForDevCrossGroup,
     *              unitsTradedForDevWithinGroup, unitsTradedForRes, unitsTradedForResCrossGroup,
     *              unitsTradedForResWithinGroup, devicesMade (6), timeGatherWithDevice (4),
     *              timeMakingDev (6), timeGatherWithoutDev, devicePercents, devDevice (2), complexity, discoveredDevices (6).
     * Then by day.
     */
    vector<vector<vector<double> > > allResults;
	void printStartConditions();
	void agentsWork();
        void updateDeviceRecipe();
	void agentsTrade();
	void agentsInvent();
	void endDay();
        void resourceRenewal();
	void endDayDecay();
	void agentsTradeDevices();
	void agentsProduceDevices();
	bool deviceExists(device_name_t deviceType);
	void printSumUtilAndRes();
	void dayAnalysis(int dayNumber);
	void removeOrSave(int dayNumber);
	void endSim();
	vector<vector<double> > calcQuartiles(vector<vector<double> > data);
	void headerByDay(ofstream &file, string filePath);

	/*
	 * Saving results functions.
	 */
	void saveResults();
	void saveGini();
	void saveHHIQuartiles();
	void saveTotalUtility();
	void saveMeanUtility();
	void saveUnitsGathered();
	void saveUnitsHeld();
	void saveUnitsTraded();
	void saveDeviceMade();
	void saveTotalTimeUsage();
	void saveDevicePercents();
	void saveDevDevice();
	void saveDeviceComplexity();
	void saveDiscoveredDevices();
        void saveResAmount();
        void saveDepletedRes();
        void saveEnvironment();
        void saveRecipe();

	void saveToDB();

	void saveEndDayData();

    void removeAgent(int agentNumber, int day);
    void removeRes(int resNumber, int day);

	void saveDayStatus(int day);		// NOT IMPLEMENtED YET.
	void loadDayStatus();		// NOT IMPLEMENtED YET.
};

extern Utils util;
extern OtherStats oStats;
#endif