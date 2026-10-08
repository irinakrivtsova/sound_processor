#include "producer.h"

#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>

namespace
{
    bool sameName(const char* lhs, const char* rhs)
    {
        return lhs != nullptr && rhs != nullptr && std::strcmp(lhs, rhs) == 0;
    }

    bool parseDouble(const char* str, double& value)
    {
        if (str == nullptr)
            return false;

        char* end = nullptr;
        errno = 0;

        value = std::strtod(str, &end);

        if (end == str || *end != '\0')
            return false;

        if (errno == ERANGE)
            return false;

        if (!std::isfinite(value))
            return false;

        return true;
    }

    bool parseInt(const char* str, int& value)
    {
        if (str == nullptr)
            return false;

        char* end = nullptr;
        errno = 0;

        long result = std::strtol(str, &end, 10);

        if (end == str || *end != '\0')
            return false;

        if (errno == ERANGE)
            return false;

        if (result < INT_MIN || result > INT_MAX)
            return false;

        value = static_cast<int>(result);
        return true;
    }

    bool parseTimeUnit(const char* str, Silence::TimeUnit& unit)
    {
        if (sameName(str, "sec"))
        {
            unit = Silence::TimeUnit::sec;
            return true;
        }

        if (sameName(str, "ms"))
        {
            unit = Silence::TimeUnit::ms;
            return true;
        }

        return false;
    }

    bool inRange(double value, double left, double right)
    {
        return left <= value && value <= right;
    }
}

namespace FilterProducers
{
    IFilter* amplFilterCreator(const FilterDescriptor& fd)
    {
        if (!sameName(fd.filterName, "ampl"))
            return nullptr;

        if (fd.params.size() != 1)
            return nullptr;

        double factor = 0.0;

        if (!parseDouble(fd.params[0], factor))
            return nullptr;

        if (factor < 0.0)
            return nullptr;

        return new AmplFilter(factor);
    }

    IFilter* normalizeFilterCreator(const FilterDescriptor& fd)
    {
        if (!sameName(fd.filterName, "normalize"))
            return nullptr;

        if (fd.params.size() > 1)
            return nullptr;

        double peak = 1.0;

        if (fd.params.size() == 1)
        {
            if (!parseDouble(fd.params[0], peak))
                return nullptr;
        }

        if (!inRange(peak, 0.0, 1.0))
            return nullptr;

        return new Normalize(peak);
    }

    IFilter* silenceFilterCreator(const FilterDescriptor& fd)
    {
        if (!sameName(fd.filterName, "silence"))
            return nullptr;

        if (fd.params.size() != 3)
            return nullptr;

        Silence::TimeUnit unit;
        double start = 0.0;
        double end = 0.0;

        if (!parseTimeUnit(fd.params[0], unit))
            return nullptr;

        if (!parseDouble(fd.params[1], start))
            return nullptr;

        if (!parseDouble(fd.params[2], end))
            return nullptr;

        if (start < 0.0)
            return nullptr;

        if (end < start)
            return nullptr;

        return new Silence(unit, start, end);
    }

    IFilter* timestretchFilterCreator(const FilterDescriptor& fd)
    {
        if (!sameName(fd.filterName, "timestretch"))
            return nullptr;

        if (fd.params.size() != 1)
            return nullptr;

        double factor = 0.0;

        if (!parseDouble(fd.params[0], factor))
            return nullptr;

        if (factor <= 0.0)
            return nullptr;

        return new Timestretch(factor);
    }

    IFilter* lowpassFilterCreator(const FilterDescriptor& fd)
    {
        if (!sameName(fd.filterName, "lowpass"))
            return nullptr;

        if (fd.params.size() != 1)
            return nullptr;

        int windowSize = 0;

        if (!parseInt(fd.params[0], windowSize))
            return nullptr;

        if (windowSize < 1)
            return nullptr;

        if (windowSize % 2 == 0)
            return nullptr;

        return new Lowpass(windowSize);
    }

    IFilter* generatorFilterCreator(const FilterDescriptor& fd)
    {
        if (!sameName(fd.filterName, "generator"))
            return nullptr;

        if (fd.params.size() < 1)
            return nullptr;

        const char* generatorType = fd.params[0];

        if (sameName(generatorType, "sin"))
        {
            if (fd.params.size() != 3)
                return nullptr;

            double frequencyHz = 0.0;
            double durationMs = 0.0;

            if (!parseDouble(fd.params[1], frequencyHz))
                return nullptr;

            if (!parseDouble(fd.params[2], durationMs))
                return nullptr;

            if (frequencyHz < 0.0)
                return nullptr;

            if (durationMs < 0.0)
                return nullptr;

            return new SinGenFilter(frequencyHz, durationMs);
        }

        if (sameName(generatorType, "am"))
        {
            if (fd.params.size() != 6)
                return nullptr;

            double amplitude = 0.0;
            double carrierHz = 0.0;
            double modulationHz = 0.0;
            double depth = 0.0;
            double durationMs = 0.0;

            if (!parseDouble(fd.params[1], amplitude))
                return nullptr;

            if (!parseDouble(fd.params[2], carrierHz))
                return nullptr;

            if (!parseDouble(fd.params[3], modulationHz))
                return nullptr;

            if (!parseDouble(fd.params[4], depth))
                return nullptr;

            if (!parseDouble(fd.params[5], durationMs))
                return nullptr;

            if (!inRange(amplitude, 0.0, 1.0))
                return nullptr;

            if (carrierHz < 0.0)
                return nullptr;

            if (modulationHz < 0.0)
                return nullptr;

            if (!inRange(depth, 0.0, 1.0))
                return nullptr;

            if (durationMs < 0.0)
                return nullptr;

            return new AmGenFilter(
                amplitude,
                carrierHz,
                modulationHz,
                depth,
                durationMs
            );
        }

        if (sameName(generatorType, "fm"))
        {
            if (fd.params.size() != 6)
                return nullptr;

            double amplitude = 0.0;
            double carrierHz = 0.0;
            double modulationHz = 0.0;
            double deviationHz = 0.0;
            double durationMs = 0.0;

            if (!parseDouble(fd.params[1], amplitude))
                return nullptr;

            if (!parseDouble(fd.params[2], carrierHz))
                return nullptr;

            if (!parseDouble(fd.params[3], modulationHz))
                return nullptr;

            if (!parseDouble(fd.params[4], deviationHz))
                return nullptr;

            if (!parseDouble(fd.params[5], durationMs))
                return nullptr;

            if (!inRange(amplitude, 0.0, 1.0))
                return nullptr;

            if (carrierHz < 0.0)
                return nullptr;

            if (modulationHz <= 0.0)
                return nullptr;

            if (deviationHz < 0.0)
                return nullptr;

            if (durationMs < 0.0)
                return nullptr;

            return new FmGenFilter(
                amplitude,
                carrierHz,
                modulationHz,
                deviationHz,
                durationMs
            );
        }

        return nullptr;
    }

    void addStandardFilterProducers(CmdLineArgs2PipelineConverter& converter)
    {
        converter.addFilterProducer("ampl", amplFilterCreator);
        converter.addFilterProducer("normalize", normalizeFilterCreator);
        converter.addFilterProducer("silence", silenceFilterCreator);
        converter.addFilterProducer("timestretch", timestretchFilterCreator);
        converter.addFilterProducer("lowpass", lowpassFilterCreator);
        converter.addFilterProducer("generator", generatorFilterCreator);
    }
}