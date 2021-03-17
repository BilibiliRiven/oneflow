/*
Copyright 2020 The OneFlow Authors. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef ONEFLOW_CORE_RPC_LIB_LOCAL_
#define ONEFLOW_CORE_RPC_LIB_LOCAL_

#ifdef RPC_BACKEND_LOCAL

#include "glog/logging.h"
#include "oneflow/core/job/env_desc.h"
#include "oneflow/core/rpc/include/local.h"

namespace oneflow {

LocalCtrlClient::LocalCtrlClient(const ProcessCtx& process_ctx) {
  CHECK(process_ctx.ctrl_addr_size() == 1);
  CHECK(process_ctx.node_size() == 1);
}

void LocalCtrlClient::Barrier(const std::string& barrier_name) {
  Barrier(barrier_name, Global<EnvDesc>::Get()->TotalMachineNum());
}

void LocalCtrlClient::Barrier(const std::string& barrier_name, int32_t barrier_num) {
  CHECK(barrier_num == 1);
}

TryLockResult LocalCtrlClient::TryLock(const std::string& name) {
  std::unique_lock<std::mutex> lck(done_names_mtx_);
  if (done_names_.find(name) != done_names_.end()) {
    return TryLockResult::kDone;
  } else {
    return TryLockResult::kLocked;
  }
}

void LocalCtrlClient::NotifyDone(const std::string& name) {
  std::unique_lock<std::mutex> lck(done_names_mtx_);
  done_names_.insert(name);
  done_names_cv_.notify_all();
}

void LocalCtrlClient::WaitUntilDone(const std::string& name) {
  std::unique_lock<std::mutex> lck(done_names_mtx_);
  LOG(ERROR) << "waiting for name: " << name;
  done_names_cv_.wait(lck);
  CHECK(done_names_.find(name) != done_names_.end());
}

void LocalCtrlClient::PushKV(const std::string& k, std::function<void(std::string*)> VSetter) {
  std::unique_lock<std::mutex> lck(kv_mtx_);
  VSetter(&kv_[k]);
  kv_cv_.notify_all();
}

void LocalCtrlClient::PushMasterKV(const std::string& k,
                                   std::function<void(std::string*)> VSetter) {
  PushKV(k, VSetter);
}

void LocalCtrlClient::PushKV(const std::string& k, const std::string& v) {
  PushKV(k, [&](std::string* o) { *o = v; });
}

void LocalCtrlClient::PushKV(const std::string& k, const PbMessage& msg) {
  PushKV(k, [&](std::string* o) { msg.SerializeToString(o); });
}

void LocalCtrlClient::PushMasterKV(const std::string& k, const PbMessage& msg) {
  PushMasterKV(k, [&](std::string* o) { msg.SerializeToString(o); });
}

void LocalCtrlClient::ClearKV(const std::string& k) {
  std::unique_lock<std::mutex> lck(kv_mtx_);
  kv_.erase(k);
}

void LocalCtrlClient::ClearMasterKV(const std::string& k) { ClearKV(k); }

void LocalCtrlClient::PullKV(const std::string& k,
                             std::function<void(const std::string&)> VGetter) {
  std::unique_lock<std::mutex> lck(kv_mtx_);
  while (true) {
    auto it = kv_.find(k);
    if (it == kv_.end()) {
      LOG(ERROR) << "waiting for key: " << k;
      kv_cv_.wait(lck);
    } else {
      VGetter(kv_.at(k));
      break;
    }
  }
}

void LocalCtrlClient::PullMasterKV(const std::string& k,
                                   std::function<void(const std::string&)> VGetter) {
  PullKV(k, VGetter);
}

void LocalCtrlClient::PullKV(const std::string& k, std::string* v) {
  PullKV(k, [&](const std::string& i) { *v = i; });
}

void LocalCtrlClient::PullKV(const std::string& k, PbMessage* msg) {
  PullKV(k, [&](const std::string& i) { msg->ParseFromString(i); });
}

void LocalCtrlClient::PullMasterKV(const std::string& k, PbMessage* msg) {
  PullMasterKV(k, [&](const std::string& i) { msg->ParseFromString(i); });
}

void LocalCtrlClient::Clear() {
  {
    std::unique_lock<std::mutex> lck(done_names_mtx_);
    done_names_.clear();
    done_names_cv_.notify_all();
  }
  {
    std::unique_lock<std::mutex> lck(kv_mtx_);
    kv_.clear();
    kv_cv_.notify_all();
  }
}

int32_t LocalCtrlClient::IncreaseCount(const std::string& k, int32_t v) { UNIMPLEMENTED(); }

void LocalCtrlClient::EraseCount(const std::string& k) { UNIMPLEMENTED(); }

Maybe<void> LocalRpcManager::Bootstrap() {
  Address* addr = Global<ProcessCtx>::Get()->add_ctrl_addr();
  addr->set_host("localhost");
  Global<ProcessCtx>::Get()->set_rank(0);
  Global<ProcessCtx>::Get()->set_node_size(1);
  return Maybe<void>::Ok();
}

Maybe<void> LocalRpcManager::CreateClient() {
  auto* client = new LocalCtrlClient(*Global<ProcessCtx>::Get());
  Global<CtrlClient>::SetAllocated(client);
  return Maybe<void>::Ok();
}

Maybe<void> LocalRpcManager::CreateServer() { return Maybe<void>::Ok(); }

LocalRpcManager::~LocalRpcManager() { Global<CtrlClient>::Delete(); }

}  // namespace oneflow

#endif  // RPC_BACKEND_LOCAL

#endif  // ONEFLOW_CORE_RPC_LIB_LOCAL_
