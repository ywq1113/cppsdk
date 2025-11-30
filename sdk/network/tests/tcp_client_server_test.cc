// Copyright 2025 The cppsdk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file tcp_client_server_test.cc
 * @brief A frame codec demo for network communication.
 * @author wizyang
 */

#include "log/log.h"
#include "network/frame_codec_simple.h"
#include "network/tcp_client.h"
#include "network/tcp_server.h"
#include "sdk/network/include/tcp_server.h"

#include <gtest/gtest.h>
#include <thread>

using namespace sdk::net;
using namespace std::chrono_literals;

constexpr int kServerTestPort1 = 9001;
constexpr int kServerTestPort2 = 9002;

class TcpClientServerTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    ::sdk::log::InitOptions opts;
    opts.level = spdlog::level::info;
    sdk::log::InitLogging(opts);
    LOG(INFO) << "Log init success\n";
  }
};

TEST_F(TcpClientServerTest, BasicSendRecv) {
  auto server = sdk::net::TcpServer::Create(kServerTestPort1);
  server->start();

  auto client = TcpClient::Create("127.0.0.1", kServerTestPort1);
  client->setCodecFactory([] { return std::make_unique<SimpleMagicCodec>(); });

  std::atomic<bool> connected = false;

  client->onConnect([&](int) { connected = true; });
  client->start();
  std::this_thread::sleep_for(400ms);
  ASSERT_TRUE(connected.load());

  client->send("hello");

  server->stop();
  client->stop();
}

TEST_F(TcpClientServerTest, DISABLED_Reconnect) {
  auto server = TcpServer::Create(kServerTestPort2);
  server->setCodecFactory([] { return std::make_unique<SimpleMagicCodec>(); });
  server->onError([](int conn_id, int code, const std::string &what) {
    LOG(INFO) << "[ " << conn_id << "] code: " << code << ", msg: " << what
              << "\n";
  });
  server->start();

  std::atomic<int> reconnectCount = 0;

  auto client = TcpClient::Create("127.0.0.1", 9002);
  client->setCodecFactory([] { return std::make_unique<SimpleMagicCodec>(); });

  client->onConnect([&](int connId) { reconnectCount++; });

  client->start();

  std::this_thread::sleep_for(100ms);
  EXPECT_EQ(reconnectCount, 1);

  // simulate drop
  server->stop();

  // allow client to detect disconnect
  std::this_thread::sleep_for(200ms);

  // restart server
  auto server2 = TcpServer::Create(kServerTestPort2);
  server2->setCodecFactory([] { return std::make_unique<SimpleMagicCodec>(); });
  server2->onError([](int conn_id, int code, const std::string &what) {
    LOG(INFO) << "[ " << conn_id << "] code: " << code << ", msg: " << what
              << "\n";
  });
  server2->start();

  // wait for auto reconnect
  std::this_thread::sleep_for(500ms);

  EXPECT_GE(reconnectCount, 2);

  client->stop();
  server2->stop();
}
