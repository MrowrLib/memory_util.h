
#pragma once

#include <Psapi.h>  // MODULEINFO
#include <_Log_.h>
#include <string_format.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "Bytes.h"

namespace MemoryUtil {

    uintptr_t Find(
        const std::string& moduleName, Bytes pattern, const std::string& mask,
        uintptr_t startOffset = 0
    ) {
        std::string theMask{mask};
        if (theMask.empty()) theMask = std::string(pattern.size(), 'x');
        _Log_(
            "Search: Searching for pattern {} with mask {} in module {}", pattern.ToHexString(),
            theMask, moduleName
        );

        auto module = GetModuleHandleA(moduleName.c_str());
        if (module == nullptr) {
            _Log_("Search: Failed to get module handle for module {}", moduleName);
            return 0;
        }

        MODULEINFO moduleInfo;
        if (!GetModuleInformation(GetCurrentProcess(), module, &moduleInfo, sizeof(moduleInfo))) {
            _Log_("Search: Failed to get module information for module {}", moduleName);
            return 0;
        }

        auto moduleStart = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
        auto moduleEnd   = moduleStart + moduleInfo.SizeOfImage;
        auto searchStart = moduleStart + startOffset;
        _Log_(
            "Search: Module {} starts at 0x{:x} + offset 0x{:x} (0x{:x}) and ends at 0x{:x}",
            moduleName, moduleStart, startOffset, searchStart, moduleEnd
        );

        auto patternBytes = pattern.GetBytes();
        auto patternSize  = pattern.GetByteCount();

        for (uintptr_t i = searchStart; i < moduleEnd - patternSize; i++) {
            bool found = true;
            for (size_t j = 0; j < patternSize; j++) {
                if (theMask[j] == 'x' && patternBytes[j] != *reinterpret_cast<uint8_t*>(i + j)) {
                    found = false;
                    break;
                }
            }
            if (found) {
                _Log_("Search: Found pattern at 0x{:x}", i);
                return i;
            }
        }

        _Log_("Search: Failed to find pattern in module {}", moduleName);
        return 0;
    }

    uintptr_t Find(
        const std::string& moduleName, const std::vector<uint8_t>& patternBytes,
        const std::string& mask, uintptr_t startOffset = 0
    ) {
        return Find(moduleName, Bytes{patternBytes}, mask, startOffset);
    }

    uintptr_t Find(
        const std::string& moduleName, const std::string& pattern, const std::string& mask,
        uintptr_t startOffset = 0
    ) {
        return Find(moduleName, Bytes::FromString(pattern), mask, startOffset);
    }
}
