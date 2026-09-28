#include "../iop_service.h"

#include <array>
#include <cstdint>
#include <cstring>

namespace ps2x::iop::detail
{
    namespace
    {
        // ps2tek: MTAPMAN system server IDs (not in BIOS).
        constexpr uint32_t kMtapPortOpenSid = 0x80000901u;
        constexpr uint32_t kMtapPortCloseSid = 0x80000902u;
        constexpr uint32_t kMtapGetConnectionSid = 0x80000903u;
        constexpr uint32_t kMtapUnknown4Sid = 0x80000904u;
        constexpr uint32_t kMtapUnknown5Sid = 0x80000905u;
        constexpr int32_t kSuccess = 1;
        constexpr int32_t kDirectPadSlots = 1;

        class MtapmanService final : public IopService
        {
        public:
            explicit MtapmanService(IopHost &host)
                : m_host(host)
            {
            }

            [[nodiscard]] std::string_view name() const override
            {
                return "MTAPMAN";
            }

            [[nodiscard]] std::span<const uint32_t> sids() const override
            {
                return kSids;
            }

            void reset() override
            {
                m_openPorts = 0u;
            }

            [[nodiscard]] RpcResult handleRpc(const RpcRequest &request) override
            {
                RpcResult result{};
                int32_t response = kSuccess;
                switch (request.sid)
                {
                case kMtapPortOpenSid:
                    m_openPorts |= portMask(request);
                    response = kSuccess;
                    break;
                case kMtapPortCloseSid:
                    m_openPorts &= ~portMask(request);
                    response = kSuccess;
                    break;
                case kMtapGetConnectionSid:
                    // No multitap: one DualShock on the port.
                    response = kDirectPadSlots;
                    break;
                case kMtapUnknown4Sid:
                case kMtapUnknown5Sid:
                    response = kSuccess;
                    break;
                default:
                    return result;
                }

                result.handled = true;
                result.resultAddress = request.receive.address;
                writeI32(request.receive, response);
                return result;
            }

        private:
            static constexpr std::array<uint32_t, 5> kSids{
                kMtapPortOpenSid,
                kMtapPortCloseSid,
                kMtapGetConnectionSid,
                kMtapUnknown4Sid,
                kMtapUnknown5Sid,
            };

            uint32_t portMask(const RpcRequest &request) const
            {
                int32_t port = 0;
                if (request.send.address != 0u && request.send.size >= sizeof(port))
                {
                    (void)m_host.readGuest(request.send.address, &port, sizeof(port));
                }
                if (port < 0 || port > 1)
                {
                    port = 0;
                }
                return 1u << static_cast<uint32_t>(port);
            }

            void writeI32(const GuestBuffer &receive, int32_t value)
            {
                if (receive.address == 0u || receive.size < sizeof(value))
                {
                    return;
                }
                (void)m_host.writeGuest(receive.address, &value, sizeof(value));
            }

            IopHost &m_host;
            uint32_t m_openPorts = 0u;
        };
    }

    std::unique_ptr<IopService> createMtapmanService(IopHost &host)
    {
        return std::make_unique<MtapmanService>(host);
    }
}
