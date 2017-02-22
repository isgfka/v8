// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "testing/gtest/include/gtest/gtest.h"

#include "include/v8.h"
#include "test/unittests/test-utils.h"

namespace v8 {

typedef TestWithIsolate RemoteObjectTest;

namespace {

bool AccessCheck(Local<Context> accessing_context,
                 Local<Object> accessed_object, Local<Value> data) {
  return false;
}

void NamedGetter(Local<Name> property,
                 const PropertyCallbackInfo<Value>& info) {}

void Constructor(const FunctionCallbackInfo<Value>& info) {
  ASSERT_TRUE(info.IsConstructCall());
}

}  // namespace

TEST_F(RemoteObjectTest, CreationContextOfRemoteContext) {
  Local<ObjectTemplate> global_template = ObjectTemplate::New(isolate());
  global_template->SetAccessCheckCallbackAndHandler(
      AccessCheck, NamedPropertyHandlerConfiguration(NamedGetter),
      IndexedPropertyHandlerConfiguration());

  Local<Object> remote_context =
      Context::NewRemoteContext(isolate(), global_template).ToLocalChecked();
  EXPECT_TRUE(remote_context->CreationContext().IsEmpty());
}

TEST_F(RemoteObjectTest, CreationContextOfRemoteObject) {
  Local<FunctionTemplate> constructor_template =
      FunctionTemplate::New(isolate(), Constructor);
  constructor_template->InstanceTemplate()->SetAccessCheckCallbackAndHandler(
      AccessCheck, NamedPropertyHandlerConfiguration(NamedGetter),
      IndexedPropertyHandlerConfiguration());

  Local<Object> remote_object =
      constructor_template->NewRemoteInstance().ToLocalChecked();
  EXPECT_TRUE(remote_object->CreationContext().IsEmpty());
}

TEST_F(RemoteObjectTest, RemoteContextInstanceChecks) {
  Local<FunctionTemplate> parent_template =
      FunctionTemplate::New(isolate(), Constructor);

  Local<FunctionTemplate> constructor_template =
      FunctionTemplate::New(isolate(), Constructor);
  constructor_template->InstanceTemplate()->SetAccessCheckCallbackAndHandler(
      AccessCheck, NamedPropertyHandlerConfiguration(NamedGetter),
      IndexedPropertyHandlerConfiguration());
  constructor_template->Inherit(parent_template);

  Local<Object> remote_context =
      Context::NewRemoteContext(isolate(),
                                    constructor_template->InstanceTemplate())
          .ToLocalChecked();
  EXPECT_TRUE(parent_template->HasInstance(remote_context));
  EXPECT_TRUE(constructor_template->HasInstance(remote_context));
}

}  // namespace v8
