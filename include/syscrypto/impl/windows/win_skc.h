#pragma once
#include "win_utils.h"
#include "../../bytes.h"
#include "../../bytes_span.h"

#include <dpapi.h>

namespace syscrypto {

    pd_bytes pde::encrypt(bytes_view data, bytes_view entropy_key, std::string_view description, encyption_scope s)
    {
        using namespace windows;

        const DWORD dwFlags = s == local_machine ? CRYPTPROTECT_LOCAL_MACHINE : 0;
        DATA_BLOB edata;

        const auto success = CryptProtectData(
            &make_data_blob(data),
            str2wstr(description).c_str(),
            &make_data_blob(entropy_key),
            NULL,
            NULL,
            dwFlags,
            &edata
        );

        if (!success) {
            throw error(GetLastError(), "protect_data failed");
        }

        const auto edit = reinterpret_cast<std::byte*>(edata.pbData);
        const auto edeit = edit + edata.cbData;

        // TODO: handle construction exception
        pd_bytes res(edit, edeit);

        LocalFree(edata.pbData);
        return res;
    }


    sec_bytes pde::decrypt(pd_bytes_view encrypted_data, bytes_view entropy_key)
    {
        using namespace windows;

        DATA_BLOB ddata;
        const auto success = CryptUnprotectData(
            &make_data_blob(encrypted_data),
            NULL, // TODO: set ppszDataDescr param to get description tag from encrypted data 
            &make_data_blob(entropy_key),
            NULL,
            NULL,
            0,
            &ddata
        );

        if (!success) {
            throw error(GetLastError(), "unprotect_data failed");
        }

        const auto ddit = reinterpret_cast<std::byte*>(ddata.pbData);
        const auto ddeit = ddit + ddata.cbData;

        // TODO: handle construction exception and zeromem od ddata
        sec_bytes res(ddit, ddeit);

        zeromem(pd_bytes_span{ ddit, ddeit });
        LocalFree(ddata.pbData);
        return res;
    }
}