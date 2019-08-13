#include <functional>

#define sc_scope_exit(...) \
    const auto SC_ANONY_VAR(scse_fo_)##_ = syscrypto::utils::at_scope_exit_do(__VA_ARGS__)

#if defined(__COUNTER__)
# define SC_ANONY_VAR(prefix) SC_CAT(prefix, __COUNTER__)
#else
# define SC_ANONY_VAR(prefix) SC_CAT(prefix, __LINE__)
#endif 

#define SC_CAT_(s1, s2) s1##s2
#define SC_CAT(s1, s2) SC_CAT_(s1, s2)


namespace syscrypto::utils {

    template<typename Lambda>
    [[nodiscard]] auto at_scope_exit_do(Lambda&& f)
    {
        struct scse_f_ final {
            scse_f_(Lambda&& f) : f_(std::move(f)) {}
            ~scse_f_() {
                f_();
            }
        private:
            std::function<void()> f_;
        };

        return scse_f_(std::forward<Lambda>(f));
    }
}