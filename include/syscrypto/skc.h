#pragma once
#include "bytes.h"
#include "bytes.h"
#include "bytes_span.h"

#include <string>
#include <string_view>
#include <system_error>

/* This header defines functions for symmetric encryption cryptography */

namespace syscrypto {

    /** 
    * Platform dependent data encryption. 
    * The encrypted data should not be shared between platforms.
    */
    struct pde {
        enum encyption_scope {
            local_user    = 1, //! encrypted data is associated with local user.
            local_machine = 2  //! encrypted data is associated with local machine. Any local user can decrypt it. 
        };

        struct error : std::system_error {
            error(int ec);
            error(int ec, const std::string& what);
            error(int ec, const char* what);
        };

        /**
         * Encrypts data.
         *
         * @param Data to be encrypted.
         * @param Entropy key.
         * @param Optional encryption scope.
         * @return Encrypted data.
         */
        [[nodiscard]]
        static pd_bytes encrypt(bytes_view data, encyption_scope s = local_user);

        /**
         * Encrypts data.
         *
         * @param Data to be encrypted.
         * @param Entropy key.
         * @param Description text of data to be encrypted.
         *        On windows description text is included with the encrypted data.
         * @param Optional encryption scope.
         * @return Encrypted data.
         */
        [[nodiscard]]
        static pd_bytes encrypt(bytes_view data, std::string_view description, encyption_scope s = local_user);

        /**
         * Encrypts data using additional entropy key.
         * Entropy key must be present when decrypting encrypted data.
         *
         * @param Data to be encrypted.
         * @param Entropy key.
         * @param Optional encryption scope.
         * @return Encrypted data.
         */
        [[nodiscard]]
        static pd_bytes encrypt(bytes_view data, bytes_view entropy_key, encyption_scope s = local_user);

        /**
         * Encrypts data using additional entropy key.
         * Entropy key must be present when decrypting encrypted data.
         *
         * @param Data to be encrypted.
         * @param Entropy key.
         * @param Description text of data to be encrypted. 
         *        On windows description text is included with the encrypted data.
         * @param Optional encryption scope.
         * @return Encrypted data.
         */
        [[nodiscard]]
        static pd_bytes encrypt(bytes_view data, bytes_view entropy_key, std::string_view description, encyption_scope s = local_user);

        [[nodiscard]]
        static sec_bytes decrypt(pd_bytes_view encrypted_data);

        [[nodiscard]]
        static sec_bytes decrypt(pd_bytes_view encrypted_data, bytes_view entropy_key);
    };
}

#include "impl/skc.impl"