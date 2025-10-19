#pragma once
#include <chrono>
#include <ctime>
#include <cstdint>
#include <mutex>
#include <thread>

/*
 * Sonyflake generates a unique unsigned 64 bit integer using a timestamp, sequence number, and a machine id.
 *
 * The first bit of the 64 bit integer is unused and always 0, the remaining 63 bits are allocated as follows:
 * - 39 bits for the timestamp since the epoch (2025-01-01 00:00 UTC) in 10ms units
 * - 8 bits for a sequence number which increments each time there is a collision in timestamp within 10ms of each other
 * - 16 bits for a machine id, default 0 if single node
 */

class Sonyflake {
public:
    using ten_ms = std::chrono::duration<long long, std::ratio<1, 100>>;

    Sonyflake(uint64_t machineId = 0) : m_machineId(machineId & ((1ULL << m_machineIdBits) - 1)) // mask to 16 bits
    {
        /* Precompute custom epoch */
        m_customEpoch = std::chrono::steady_clock::now();
    }

    uint64_t GetUniqueId()
    {
        std::lock_guard<std::mutex> lock(m_mtx);

        auto now = std::chrono::steady_clock::now();
        auto timestamp = static_cast<uint64_t>(std::chrono::duration_cast<ten_ms>(now - m_customEpoch).count());

        if (timestamp <= m_lastTimestamp) {
            m_sequenceNum = (m_sequenceNum + 1) & ((1 << m_sequenceBits) - 1); // mask to 8 bits
            /* Wait until next 10ms tick if overflow so timestamps differ */
            if (m_sequenceNum == 0) {
                m_lastTimestamp++;
                std::this_thread::sleep_until(m_customEpoch + ten_ms(m_lastTimestamp));
            }
        } else {
            m_lastTimestamp = timestamp;
            m_sequenceNum = 0;
        }

        if (m_lastTimestamp >= (1ULL << m_timestampBits)) {
            throw std::runtime_error("Sonyflake: Timestamp overflow");
        }

        return (uint64_t(m_lastTimestamp) << (m_sequenceBits + m_machineIdBits)) |
               (uint64_t(m_sequenceNum) << m_machineIdBits) | uint64_t(m_machineId);
    }

    void SetMachineId(uint64_t machineId)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_machineId = machineId & ((1ULL << m_machineIdBits) - 1); // mask to 16 bits
    }

private:
    std::mutex m_mtx;

    static constexpr uint32_t m_timestampBits = 39;
    static constexpr uint32_t m_sequenceBits = 8;
    static constexpr uint32_t m_machineIdBits = 63 - m_timestampBits - m_sequenceBits;

    std::chrono::steady_clock::time_point m_customEpoch;
    uint64_t m_lastTimestamp = 0;
    uint16_t m_sequenceNum = 0;
    uint16_t m_machineId = 0;
};