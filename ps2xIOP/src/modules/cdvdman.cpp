#include "../iop_service.h"

#include <array>
#include <cstdint>
#include <string_view>

namespace ps2x::iop::detail
{
    namespace
    {
        constexpr uint32_t kCdvdInitSid = 0x80000592u;
        constexpr uint32_t kCdvdSCmdSid = 0x80000593u;
        constexpr uint32_t kCdvdNCmdSid = 0x80000595u;
        constexpr uint32_t kCdvdSearchFileSid = 0x80000597u;
        constexpr uint32_t kCdvdDiskReadySid = 0x8000059Au;

        class CdvdmanService final : public IopService
        {
        public:
            explicit CdvdmanService(IopHost &)
            {
            }

            [[nodiscard]] std::string_view name() const override
            {
                return "CDVDMAN";
            }

            [[nodiscard]] std::span<const uint32_t> sids() const override
            {
                return kSids;
            }

            void reset() override
            {
            }

            RpcResult handleRpc(const RpcRequest &request) override
            {
                RpcResult result{};

                switch (request.sid)
                {
                case kCdvdInitSid:
                case kCdvdSCmdSid:
                case kCdvdNCmdSid:
                case kCdvdSearchFileSid:
                case kCdvdDiskReadySid:
                    // Control RPCs: acknowledge without touching the packet.
                    result.handled = true;
                    result.resultAddress = request.receive.address;
                    break;

                default:
                    break;
                }

                return result;
            }

        private:
            static constexpr std::array<uint32_t, 5> kSids{
                kCdvdInitSid,
                kCdvdSCmdSid,
                kCdvdNCmdSid,
                kCdvdSearchFileSid,
                kCdvdDiskReadySid,
            };
        };
    }

    std::unique_ptr<IopService> createCdvdmanService(IopHost &host)
    {
        return std::make_unique<CdvdmanService>(host);
    }
}