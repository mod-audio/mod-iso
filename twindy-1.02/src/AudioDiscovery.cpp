#define AUDIO_PREFS_SKIP_CODE 1

#include "AudioPreferences.cpp"

int main()
{
    StringArray inputIds, inputNames, outputIds, outputNames;
    enumerateAlsaSoundcards(inputNames, outputNames, inputIds, outputIds);

    inputNames.appendNumbersToDuplicates(false, true);
    outputNames.appendNumbersToDuplicates(false, true);

    uint minChansOut = 0;
    uint maxChansOut = 0;
    uint minChansIn  = 0;
    uint maxChansIn  = 0;
    Array<uint> bufferSizes;
    Array<double> sampleRates;

    for (int i=0, size=outputIds.size(); i<size; ++i)
    {
        const String deviceId(outputIds[i]);
        const String deviceName(outputNames[i]);

        getDeviceProperties(deviceId,
                            minChansOut, maxChansOut, minChansIn, maxChansIn,
                            bufferSizes, sampleRates, true, true);

        printf("=============================================================\n");
        printf("DeviceId:    %s\n", deviceId.toUTF8());
        printf("DeviceName:  %s\n", deviceName.toUTF8());
        printf("minChansIn:  %u\n", minChansIn);
        printf("maxChansIn:  %u\n", maxChansIn);
        printf("minChansOut: %u\n", minChansOut);
        printf("maxChansOut: %u\n", maxChansOut);
        printf("bufferSizes:\n");
        for (int j=0, size2=bufferSizes.size(); j<size2; ++j)
            printf("    %u\n", bufferSizes[j]);
        printf("sampleRates:\n");
        for (int j=0, size2=sampleRates.size(); j<size2; ++j)
            printf("    %f\n", sampleRates[j]);
    }

    return 0;
}
