include filenames.mk

# ASYNC IO
ifeq ($(filter test_asyncio_echo,$(MAKECMDGOALS)),test_asyncio_echo)
    SRCS += $(SRCS_ASYNC_IO)

else ifeq ($(filter test_asyncio_singleio,$(MAKECMDGOALS)),test_asyncio_singleio)
    SRCS += $(SRCS_ASYNC_IO)

# CONFIG PARSER
else ifeq ($(filter test_configparser,$(MAKECMDGOALS)),test_configparser)
    SRCS += $(SRCS_CONFIG_PARSER)

# HTTP REQUEST
else ifeq ($(filter test_http_request,$(MAKECMDGOALS)),test_http_request)
    SRCS += $(SRCS_HTTP_COMMON) \
			$(SRCS_HTTP_HEADER) \
			$(SRCS_HTTP_REQUEST) \

# HTTP RESPONSE
else ifeq ($(filter test_http_response,$(MAKECMDGOALS)),test_http_response)
    SRCS += $(SRCS_HTTP_COMMON) \
			$(SRCS_HTTP_HEADER) \
			$(SRCS_HTTP_RESPONSE) \

# HTTP SERVER
else ifeq ($(filter test_http_server_constructor,$(MAKECMDGOALS)),test_http_server_constructor)
    SRCS += $(SRCS_HTTP_COMMON) \
			$(SRCS_HTTP_HEADER) \
			$(SRCS_HTTP_REQUEST) \
			$(SRCS_HTTP_RESPONSE) \
			$(SRCS_CONFIG_PARSER) \
			$(SRCS_HTTP_SERVER) \

endif
