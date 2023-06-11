#ifndef TEXT_STYLES_H
#define TEXT_STYLES_H

#define TEXT_ESCAPED_BOLD "\033[1m"
#define TEXT_ESCAPED_RESET "\033[0m"

#define TEXT_ESCAPED_RED    "\033[0;31m"
#define TEXT_ESCAPED_GREEN  "\033[0;32m"
#define TEXT_ESCAPED_YELLOW "\033[0;33m"
#define TEXT_ESCAPED_BLUE   "\033[0;34m"
#define TEXT_ESCAPED_MAGENTA "\033[0;35m"
#define TEXT_ESCAPED_CYAN   "\033[0;36m"
#define TEXT_ESCAPED_GRAY   "\033[0;37m"

#define TEXT_ESCAPED_FATAL(str)     TEXT_ESCAPED_BOLD   TEXT_ESCAPED_RED    str TEXT_ESCAPED_RESET
#define TEXT_ESCAPED_ERROR(str)                         TEXT_ESCAPED_MAGENTA str TEXT_ESCAPED_RESET
#define TEXT_ESCAPED_WARNING(str)   TEXT_ESCAPED_BOLD   TEXT_ESCAPED_YELLOW str TEXT_ESCAPED_RESET
#define TEXT_ESCAPED_INFO(str)      TEXT_ESCAPED_BOLD   TEXT_ESCAPED_CYAN   str TEXT_ESCAPED_RESET
#define TEXT_ESCAPED_TRACE(str)                         TEXT_ESCAPED_GRAY   str TEXT_ESCAPED_RESET
#define TEXT_ESCAPED_NOTE(str)      TEXT_ESCAPED_BOLD   TEXT_ESCAPED_BLUE   str TEXT_ESCAPED_RESET

#define TEXT_HTML_END "</span>"

#define TEXT_HTML_RED      "<span style=\"color:red\">"
#define TEXT_HTML_GREEN    "<span style=\"color:green\">"
#define TEXT_HTML_YELLOW   "<span style=\"color:yellow\">"
#define TEXT_HTML_MAGENTA  "<span style=\"color:magenta\""
#define TEXT_HTML_BLUE     "<span style=\"color:dodgerblue\">"
#define TEXT_HTML_CYAN     "<span style=\"color:cyan\">"
#define TEXT_HTML_GRAY     "<span style=\"color:lightsteelblue\">"

#define TEXT_HTML_FATAL(str)    TEXT_HTML_RED       str TEXT_HTML_END
#define TEXT_HTML_ERROR(str)    TEXT_HTML_MAGENTA   str TEXT_HTML_END
#define TEXT_HTML_WARNING(str)  TEXT_HTML_YELLOW    str TEXT_HTML_END
#define TEXT_HTML_INFO(str)     TEXT_HTML_CYAN      str TEXT_HTML_END
#define TEXT_HTML_TRACE(str)    TEXT_HTML_GRAY      str TEXT_HTML_END
#define TEXT_HTML_NOTE(str)     TEXT_HTML_BLUE      str TEXT_HTML_END

#endif