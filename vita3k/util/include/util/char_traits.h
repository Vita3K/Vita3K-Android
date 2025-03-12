#include <string>

namespace std {
    template<>
    struct char_traits<unsigned char> : public char_traits<char> {
        typedef unsigned char char_type;
        typedef int int_type;

        static void assign(char_type &c1, const char_type &c2) noexcept {
            c1 = c2;
        }

        static bool eq(const char_type &c1, const char_type &c2) noexcept {
            return c1 == c2;
        }

        static bool lt(const char_type &c1, const char_type &c2) noexcept {
            return c1 < c2;
        }

        static int compare(const char_type *s1, const char_type *s2, size_t n) {
            for (size_t i = 0; i < n; ++i) {
                if (lt(s1[i], s2[i])) return -1;
                if (lt(s2[i], s1[i])) return 1;
            }
            return 0;
        }

        static size_t length(const char_type *s) {
            size_t i = 0;
            while (!eq(s[i], char_type())) ++i;
            return i;
        }

        static const char_type* find(const char_type *s, size_t n, const char_type &a) {
            for (size_t i = 0; i < n; ++i) {
                if (eq(s[i], a)) return s + i;
            }
            return nullptr;
        }

        static char_type* move(char_type *s1, const char_type *s2, size_t n) {
            return static_cast<char_type*>(memmove(s1, s2, n));
        }

        static char_type* copy(char_type *s1, const char_type *s2, size_t n) {
            return static_cast<char_type*>(memcpy(s1, s2, n));
        }

        static char_type* assign(char_type *s, size_t n, char_type a) {
            for (size_t i = 0; i < n; ++i) s[i] = a;
            return s;
        }

        static char_type to_char_type(const int_type &c) noexcept {
            return static_cast<char_type>(c);
        }

        static int_type to_int_type(const char_type &c) noexcept {
            return static_cast<int_type>(c);
        }

        static bool eq_int_type(const int_type &c1, const int_type &c2) noexcept {
            return c1 == c2;
        }

        static int_type eof() noexcept {
            return static_cast<int_type>(EOF);
        }

        static int_type not_eof(const int_type &c) noexcept {
            return eq_int_type(c, eof()) ? 0 : c;
        }
    };
}
