/**
 * MAX_URI_LENGTH:
 *
 * The definition of the URI regexp in the above RFC has no size limit
 * In practice they are usually relativey short except for the
 * data URI scheme as defined in RFC 2397. Even for data URI the usual
 * maximum size before hitting random practical limits is around 64 KB
 * and 4KB is usually a maximum admitted limit for proper operations.
 * The value below is more a security limit than anything else and
 * really should never be hit by 'normal' operations
 * Set to 1 MByte in 2012, this is only enforced on output
 */
#define MAX_URI_LENGTH (1024 * 1024)

/*
 * Old rule from 2396 used in legacy handling code
 * alpha    = lowalpha | upalpha
 */
#define IS_ALPHA(x) (IS_LOWALPHA(x) || IS_UPALPHA(x))

/*
 * lowalpha = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" |
 *            "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" |
 *            "u" | "v" | "w" | "x" | "y" | "z"
 */

#define IS_LOWALPHA(x) (((x) >= 'a') && ((x) <= 'z'))

/*
 * upalpha = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" |
 *           "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" |
 *           "U" | "V" | "W" | "X" | "Y" | "Z"
 */
#define IS_UPALPHA(x) (((x) >= 'A') && ((x) <= 'Z'))

#ifdef IS_DIGIT
#undef IS_DIGIT
#endif
/*
 * digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
 */
#define IS_DIGIT(x) (((x) >= '0') && ((x) <= '9'))

/*
 * alphanum = alpha | digit
 */

#define IS_ALPHANUM(x) (IS_ALPHA(x) || IS_DIGIT(x))

/*
 * mark = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"
 */

#define IS_MARK(x) (((x) == '-') || ((x) == '_') || ((x) == '.') ||     \
    ((x) == '!') || ((x) == '~') || ((x) == '*') || ((x) == '\'') ||    \
    ((x) == '(') || ((x) == ')'))

/*
 * unwise = "{" | "}" | "|" | "\" | "^" | "`"
 */

#define IS_UNWISE(p)                                                    \
      (((*(p) == '{')) || ((*(p) == '}')) || ((*(p) == '|')) ||         \
       ((*(p) == '\\')) || ((*(p) == '^')) || ((*(p) == '[')) ||        \
       ((*(p) == ']')) || ((*(p) == '`')))
/*
 * reserved = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | "," |
 *            "[" | "]"
 */

#define IS_RESERVED(x) (((x) == ';') || ((x) == '/') || ((x) == '?') || \
        ((x) == ':') || ((x) == '@') || ((x) == '&') || ((x) == '=') || \
        ((x) == '+') || ((x) == '$') || ((x) == ',') || ((x) == '[') || \
        ((x) == ']'))

/*
 * unreserved = alphanum | mark
 */

#define IS_UNRESERVED(x) (IS_ALPHANUM(x) || IS_MARK(x))

/*
 * Skip to next pointer char, handle escaped sequences
 */

#define NEXT(p) ((*p == '%')? p += 3 : p++)

/*
 * Productions from the spec.
 *
 *    authority     = server | reg_name
 *    reg_name      = 1*( unreserved | escaped | "$" | "," |
 *                        ";" | ":" | "@" | "&" | "=" | "+" )
 *
 * path          = [ abs_path | opaque_part ]
 */


/************************************************************************
 *									*
 *                         RFC 3986 parser				*
 *									*
 ************************************************************************/

#define ISA_DIGIT(p) ((*(p) >= '0') && (*(p) <= '9'))
#define ISA_ALPHA(p) (((*(p) >= 'a') && (*(p) <= 'z')) ||		\
                      ((*(p) >= 'A') && (*(p) <= 'Z')))
#define ISA_HEXDIG(p)							\
       (ISA_DIGIT(p) || ((*(p) >= 'a') && (*(p) <= 'f')) ||		\
        ((*(p) >= 'A') && (*(p) <= 'F')))

/*
 *    sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
 *                     / "*" / "+" / "," / ";" / "="
 */
#define ISA_SUB_DELIM(p)						\
      (((*(p) == '!')) || ((*(p) == '$')) || ((*(p) == '&')) ||		\
       ((*(p) == '(')) || ((*(p) == ')')) || ((*(p) == '*')) ||		\
       ((*(p) == '+')) || ((*(p) == ',')) || ((*(p) == ';')) ||		\
       ((*(p) == '=')) || ((*(p) == '\'')))

/*
 *    gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
 */
#define ISA_GEN_DELIM(p)						\
      (((*(p) == ':')) || ((*(p) == '/')) || ((*(p) == '?')) ||         \
       ((*(p) == '#')) || ((*(p) == '[')) || ((*(p) == ']')) ||         \
       ((*(p) == '@')))

/*
 *    reserved      = gen-delims / sub-delims
 */
#define ISA_RESERVED(p) (ISA_GEN_DELIM(p) || (ISA_SUB_DELIM(p)))

/*
 *    unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
 */
#define ISA_UNRESERVED(p)						\
      ((ISA_ALPHA(p)) || (ISA_DIGIT(p)) || ((*(p) == '-')) ||		\
       ((*(p) == '.')) || ((*(p) == '_')) || ((*(p) == '~')))

/*
 *    pct-encoded   = "%" HEXDIG HEXDIG
 */
#define ISA_PCT_ENCODED(p)						\
     ((*(p) == '%') && (ISA_HEXDIG(p + 1)) && (ISA_HEXDIG(p + 2)))

/*
 *    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
 */
#define ISA_PCHAR(p)							\
     (ISA_UNRESERVED(p) || ISA_PCT_ENCODED(p) || ISA_SUB_DELIM(p) ||	\
      ((*(p) == ':')) || ((*(p) == '@')))
