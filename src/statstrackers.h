#ifndef _SOC_STATSTRACKERS_H_
#define _SOC_STATSTRACKERS_H_

#include "globals.h"
using namespace std;

class TradeStats
{
private:
    int *resTradeVolume;
    int *resTradeForDeviceVolume;

public:
    TradeStats();
    void dailyUpdate(const int dayNum);
};

class ExtractionStats
{
private:
    int *resExtracted;
    int **resExtractedByAgent;
    int ***resExtractedByResByAgent;

    double *timeSpentExtractingWithoutDevice;
    double **timeSpentExtractingWithoutDeviceByAgent;

    /* resExtractedByRes, resExtractedByDevice, resExtractedByDeviceByRes are indexed by TOOL through INDUSTRY.
       percentResExtractedByDevice and percentResExtractedByDeviceByRes are indexed by TOOL through INDUSTRY.
       timeSpentExtractingWithDevice and timeSpentExtractingWithDeviceByAgent are indexed by TOOL through INDUSTRY.
       We'll still make 6 vectors, but will only fill in the vectors inside of
       those for the first 4. */
    int **resExtractedByRes;
    int **resExtractedByDevice;
    int ***resExtractedByDeviceByRes; // Not being tracked

    double **percentResExtractedByDevice; // Not being tracked
    double ***percentResExtractedByDeviceByRes; // Not being tracked

    double **timeSpentExtractingWithDevice; // Not being tracked
    double ***timeSpentExtractingWithDeviceByAgent; // Not being tracked

public:
    ExtractionStats();
    void dailyUpdate(const int dayNum);
};

class ProductionStats
{
private:
    int **devicesMade; // Not being tracked
    int ***devicesMadeByRes; // Not being tracked

    double *timeSpentMakingDevices; // Not being tracked
    double **timeSpentMakingDevicesByAgent; // Not being tracked

    /* devicesMadeWithDevDevice and devicesMadeWithDevDeviceByRes are indexed by DEVMACHINE through DEVFACTORY.
       We'll still make 6 vectors, but will only fill in the vectors inside of
       those for the last 2. */
    int **devicesMadeWithDevDevice; // Not being tracked
    int ***devicesMadeWithDevDeviceByRes; // Not being tracked

public:
    ProductionStats();
    void dailyUpdate(const int dayNum);
};

class OtherStats
{
private:
    int *activeAgents; // Not being tracked
    int *sumRes; // Not being tracked 
    int **sumResByAgent; // Not being tracked
    double *sumUtil; // Not being tracked
    double **sumUtilByAgent; // Not being tracked
    int **numberOfInventedDevices; // Not being tracked

public:
    OtherStats();
    void dailyUpdate(const int dayNum);
};

#endif
