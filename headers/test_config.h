#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

// 測試配置
constexpr const char* TEST_SERVER_ADDRESS = "127.0.0.1";
constexpr unsigned short TEST_SERVER_PORT = 53000;

// 生產配置
constexpr const char* PRODUCTION_SERVER_ADDRESS = "reversi.yourdomain.com";
constexpr unsigned short PRODUCTION_SERVER_PORT = 53000;

// 當前使用的配置
#ifdef TESTING_MODE
constexpr const char* CURRENT_SERVER = TEST_SERVER_ADDRESS;
constexpr unsigned short CURRENT_PORT = TEST_SERVER_PORT;
#else
constexpr const char* CURRENT_SERVER = PRODUCTION_SERVER_ADDRESS;
constexpr unsigned short CURRENT_PORT = PRODUCTION_SERVER_PORT;
#endif

#endif // TEST_CONFIG_H