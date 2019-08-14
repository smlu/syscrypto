#pragma once
#include "win_utils.h"
#include "../utils.h"
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
            throw error(GetLastError(), "encryption failed");
        }

        sc_scope_exit([&edata](){
            LocalFree(edata.pbData);
        });

        const auto edit  = reinterpret_cast<std::byte*>(edata.pbData);
        const auto edeit = edit + edata.cbData;
        return pd_bytes(edit, edeit);
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
            throw error(GetLastError(), "decryption failed");
        }

        const auto ddit  = reinterpret_cast<std::byte*>(ddata.pbData);
        const auto ddeit = ddit + ddata.cbData;

        sc_scope_exit([&](){
            zeromem(pd_bytes_span{ ddit, ddeit });
            LocalFree(ddata.pbData);
        });

        return sec_bytes(ddit, ddeit);
    }
}