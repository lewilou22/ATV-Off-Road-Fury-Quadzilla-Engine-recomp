#include "../iop_service.h"

#include <array>
#include <cstdint>
#include <mutex>

namespace ps2x::iop::detail
{
    namespace
    {
        // 989SND.IRX (ATV Offroad Fury / Rainbow). Custom SID, bit 31 clear.
        constexpr uint32_t k989SndSid = 0x00123456u;

        class Snd989Service final : public IopService
        {
        public:
            explicit Snd989Service(IopHost &host)
                : m_host(host)
            {
            }

            [[nodiscard]] std::string_view name() const override
            {
                return "989SND";
            }

            [[nodiscard]] std::span<const uint32_t> sids() const override
            {
                return kSids;
            }

            void reset() override
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_ready = true;
                m_commandSeq = 0u;
            }

            [[nodiscard]] RpcResult handleRpc(const RpcRequest &request) override
            {
                RpcResult result{};
                if (request.sid != k989SndSid)
                {
                    return result;
                }

                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_ready = true;
                    ++m_commandSeq;
                }

                result.handled = true;
                result.resultAddress = request.receive.address;
                result.signalNowaitCompletion = true;

                if (request.receive.address != 0u && request.receive.size != 0u)
                {
                    (void)m_host.zeroGuest(request.receive.address, request.receive.size);
                    // EE snd_SendIOPCommandAndWait / snd_GotReturns poll a 12-byte packet:
                    // word0 and word2 must become -1, word1 is the command result (v0).
                    const uint32_t done = 0xFFFFFFFFu;
                    const uint32_t resultWord = 0u;
                    if (request.receive.size >= sizeof(done))
                    {
                        (void)m_host.writeGuest(request.receive.address, &done, sizeof(done));
                    }
                    if (request.receive.size >= 8u)
                    {
                        (void)m_host.writeGuest(request.receive.address + 4u, &resultWord, sizeof(resultWord));
                    }
                    if (request.receive.size >= 12u)
                    {
                        (void)m_host.writeGuest(request.receive.address + 8u, &done, sizeof(done));
                    }
                }

                return result;
            }

        private:
            inline static constexpr std::array<uint32_t, 1> kSids{k989SndSid};

            IopHost &m_host;
            mutable std::mutex m_mutex;
            bool m_ready = true;
            uint32_t m_commandSeq = 0u;
        };
    }

    std::unique_ptr<IopService> create989SndService(IopHost &host)
    {
        return std::make_unique<Snd989Service>(host);
    }
}
