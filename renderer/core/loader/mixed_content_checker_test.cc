// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/loader/mixed_content_checker.h"

#include <base/macros.h>
#include <memory>
#include "base/memory/scoped_refptr.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/mojom/loader/request_context_frame_type.mojom-blink.h"
#include "third_party/blink/public/platform/web_mixed_content.h"
#include "third_party/blink/public/platform/web_mixed_content_context_type.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/loader/empty_clients.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/platform/loader/fetch/fetch_client_settings_object.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_response.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/weborigin/security_origin.h"

namespace blink {

// Tests that MixedContentChecker::isMixedContent correctly detects or ignores
// many cases where there is or there is not mixed content, respectively.
// Note: Renderer side version of
// MixedContentNavigationThrottleTest.IsMixedContent. Must be kept in sync
// manually!
TEST(MixedContentCheckerTest, IsMixedContent) {
  struct TestCase {
    const char* origin;
    const char* target;
    bool expectation;
  } cases[] = {
      {"http://example.com/foo", "http://example.com/foo", false},
      {"http://example.com/foo", "https://example.com/foo", false},
      {"http://example.com/foo", "data:text/html,<p>Hi!</p>", false},
      {"http://example.com/foo", "about:blank", false},
      {"https://example.com/foo", "https://example.com/foo", false},
      {"https://example.com/foo", "wss://example.com/foo", false},
      {"https://example.com/foo", "data:text/html,<p>Hi!</p>", false},
      {"https://example.com/foo", "blob:https://example.com/foo", false},
      {"https://example.com/foo", "blob:http://example.com/foo", false},
      {"https://example.com/foo", "blob:null/foo", false},
      {"https://example.com/foo", "filesystem:https://example.com/foo", false},
      {"https://example.com/foo", "filesystem:http://example.com/foo", false},
      {"https://example.com/foo", "http://127.0.0.1/", false},
      {"https://example.com/foo", "http://[::1]/", false},
      {"https://example.com/foo", "http://a.localhost/", false},
      {"https://example.com/foo", "http://localhost/", false},

      {"https://example.com/foo", "http://example.com/foo", true},
      {"https://example.com/foo", "http://google.com/foo", true},
      {"https://example.com/foo", "ws://example.com/foo", true},
      {"https://example.com/foo", "ws://google.com/foo", true},
      {"https://example.com/foo", "http://192.168.1.1/", true},
  };

  for (const auto& test : cases) {
    SCOPED_TRACE(testing::Message()
                 << "Origin: " << test.origin << ", Target: " << test.target
                 << ", Expectation: " << test.expectation);
    KURL origin_url(NullURL(), test.origin);
    scoped_refptr<const SecurityOrigin> security_origin(
        SecurityOrigin::Create(origin_url));
    KURL target_url(NullURL(), test.target);
    EXPECT_EQ(test.expectation, MixedContentChecker::IsMixedContent(
                                    security_origin.get(), target_url));
  }
}

TEST(MixedContentCheckerTest, ContextTypeForInspector) {
  auto dummy_page_holder = std::make_unique<DummyPageHolder>(IntSize(1, 1));
  dummy_page_holder->GetFrame().Loader().CommitNavigation(
      WebNavigationParams::CreateWithHTMLBuffer(SharedBuffer::Create(),
                                                KURL("http://example.test")),
      nullptr /* extra_data */);
  blink::test::RunPendingTasks();

  ResourceRequest not_mixed_content("https://example.test/foo.jpg");
  not_mixed_content.SetRequestContext(mojom::RequestContextType::SCRIPT);
  EXPECT_EQ(WebMixedContentContextType::kNotMixedContent,
            MixedContentChecker::ContextTypeForInspector(
                &dummy_page_holder->GetFrame(), not_mixed_content));

  dummy_page_holder->GetFrame().Loader().CommitNavigation(
      WebNavigationParams::CreateWithHTMLBuffer(SharedBuffer::Create(),
                                                KURL("https://example.test")),
      nullptr /* extra_data */);
  blink::test::RunPendingTasks();

  EXPECT_EQ(WebMixedContentContextType::kNotMixedContent,
            MixedContentChecker::ContextTypeForInspector(
                &dummy_page_holder->GetFrame(), not_mixed_content));

  ResourceRequest blockable_mixed_content("http://example.test/foo.jpg");
  blockable_mixed_content.SetRequestContext(mojom::RequestContextType::SCRIPT);
  EXPECT_EQ(WebMixedContentContextType::kBlockable,
            MixedContentChecker::ContextTypeForInspector(
                &dummy_page_holder->GetFrame(), blockable_mixed_content));

  ResourceRequest optionally_blockable_mixed_content(
      "http://example.test/foo.jpg");
  blockable_mixed_content.SetRequestContext(mojom::RequestContextType::IMAGE);
  EXPECT_EQ(WebMixedContentContextType::kOptionallyBlockable,
            MixedContentChecker::ContextTypeForInspector(
                &dummy_page_holder->GetFrame(), blockable_mixed_content));
}

namespace {

class MixedContentCheckerMockLocalFrameClient : public EmptyLocalFrameClient {
 public:
  MixedContentCheckerMockLocalFrameClient() : EmptyLocalFrameClient() {}
  MOCK_METHOD0(DidContainInsecureFormAction, void());
  MOCK_METHOD0(DidDisplayContentWithCertificateErrors, void());
  MOCK_METHOD0(DidRunContentWithCertificateErrors, void());
};

}  // namespace

TEST(MixedContentCheckerTest, HandleCertificateError) {
  MixedContentCheckerMockLocalFrameClient* client =
      MakeGarbageCollected<MixedContentCheckerMockLocalFrameClient>();
  auto dummy_page_holder =
      std::make_unique<DummyPageHolder>(IntSize(1, 1), nullptr, client);

  KURL main_resource_url(NullURL(), "https://example.test");
  KURL displayed_url(NullURL(), "https://example-displayed.test");
  KURL ran_url(NullURL(), "https://example-ran.test");

  dummy_page_holder->GetFrame().GetDocument()->SetURL(main_resource_url);
  ResourceResponse response1(ran_url);
  EXPECT_CALL(*client, DidRunContentWithCertificateErrors());
  MixedContentChecker::HandleCertificateError(
      &dummy_page_holder->GetFrame(), response1,
      mojom::RequestContextType::SCRIPT);

  ResourceResponse response2(displayed_url);
  mojom::RequestContextType request_context = mojom::RequestContextType::IMAGE;
  ASSERT_EQ(
      WebMixedContentContextType::kOptionallyBlockable,
      WebMixedContent::ContextTypeFromRequestContext(
          request_context, dummy_page_holder->GetFrame()
                               .GetSettings()
                               ->GetStrictMixedContentCheckingForPlugin()));
  EXPECT_CALL(*client, DidDisplayContentWithCertificateErrors());
  MixedContentChecker::HandleCertificateError(&dummy_page_holder->GetFrame(),
                                              response2, request_context);
}

TEST(MixedContentCheckerTest, DetectMixedForm) {
  KURL main_resource_url(NullURL(), "https://example.test/");
  MixedContentCheckerMockLocalFrameClient* client =
      MakeGarbageCollected<MixedContentCheckerMockLocalFrameClient>();
  auto dummy_page_holder =
      std::make_unique<DummyPageHolder>(IntSize(1, 1), nullptr, client);
  dummy_page_holder->GetFrame().Loader().CommitNavigation(
      WebNavigationParams::CreateWithHTMLBuffer(SharedBuffer::Create(),
                                                main_resource_url),
      nullptr /* extra_data */);
  blink::test::RunPendingTasks();

  KURL http_form_action_url(NullURL(), "http://example-action.test/");
  KURL https_form_action_url(NullURL(), "https://example-action.test/");
  KURL javascript_form_action_url(NullURL(), "javascript:void(0);");
  KURL mailto_form_action_url(NullURL(), "mailto:action@example-action.test");

  // mailto and http are non-secure form targets.
  EXPECT_CALL(*client, DidContainInsecureFormAction()).Times(2);

  EXPECT_TRUE(MixedContentChecker::IsMixedFormAction(
      &dummy_page_holder->GetFrame(), http_form_action_url,
      ReportingDisposition::kSuppressReporting));
  EXPECT_FALSE(MixedContentChecker::IsMixedFormAction(
      &dummy_page_holder->GetFrame(), https_form_action_url,
      ReportingDisposition::kSuppressReporting));
  EXPECT_FALSE(MixedContentChecker::IsMixedFormAction(
      &dummy_page_holder->GetFrame(), javascript_form_action_url,
      ReportingDisposition::kSuppressReporting));
  EXPECT_TRUE(MixedContentChecker::IsMixedFormAction(
      &dummy_page_holder->GetFrame(), mailto_form_action_url,
      ReportingDisposition::kSuppressReporting));
}

TEST(MixedContentCheckerTest, DetectMixedFavicon) {
  KURL main_resource_url("https://example.test/");
  MixedContentCheckerMockLocalFrameClient* client =
      MakeGarbageCollected<MixedContentCheckerMockLocalFrameClient>();
  auto dummy_page_holder =
      std::make_unique<DummyPageHolder>(IntSize(1, 1), nullptr, client);
  dummy_page_holder->GetFrame().Loader().CommitNavigation(
      WebNavigationParams::CreateWithHTMLBuffer(SharedBuffer::Create(),
                                                main_resource_url),
      nullptr /* extra_data */);
  blink::test::RunPendingTasks();
  dummy_page_holder->GetFrame().GetSettings()->SetAllowRunningOfInsecureContent(
      false);

  KURL http_favicon_url("http://example.test/favicon.png");
  KURL https_favicon_url("https://example.test/favicon.png");

  // Test that a mixed content favicon is correctly blocked.
  EXPECT_TRUE(MixedContentChecker::ShouldBlockFetch(
      &dummy_page_holder->GetFrame(), mojom::RequestContextType::FAVICON,
      ResourceRequest::RedirectStatus::kNoRedirect, http_favicon_url,
      base::Optional<String>(), ReportingDisposition::kSuppressReporting));

  // Test that a secure favicon is not blocked.
  EXPECT_FALSE(MixedContentChecker::ShouldBlockFetch(
      &dummy_page_holder->GetFrame(), mojom::RequestContextType::FAVICON,
      ResourceRequest::RedirectStatus::kNoRedirect, https_favicon_url,
      base::Optional<String>(), ReportingDisposition::kSuppressReporting));
}

class TestFetchClientSettingsObject : public FetchClientSettingsObject {
 public:
  const KURL& GlobalObjectUrl() const override { return url; }
  HttpsState GetHttpsState() const override { return HttpsState::kModern; }
  mojom::blink::InsecureRequestPolicy GetInsecureRequestsPolicy()
      const override {
    return mojom::blink::InsecureRequestPolicy::kLeaveInsecureRequestsAlone;
  }

  // These are not used in test, but need to be implemented since they are pure
  // virtual.
  const KURL& BaseUrl() const override { return url; }
  const SecurityOrigin* GetSecurityOrigin() const override { return nullptr; }
  network::mojom::ReferrerPolicy GetReferrerPolicy() const override {
    return network::mojom::ReferrerPolicy::kAlways;
  }
  const String GetOutgoingReferrer() const override { return ""; }
  AllowedByNosniff::MimeTypeCheck MimeTypeCheckForClassicWorkerScript()
      const override {
    return AllowedByNosniff::MimeTypeCheck::kStrict;
  }
  network::mojom::IPAddressSpace GetAddressSpace() const override {
    return network::mojom::IPAddressSpace::kLocal;
  }
  const InsecureNavigationsSet& GetUpgradeInsecureNavigationsSet()
      const override {
    return set;
  }

 private:
  const KURL url = KURL("https://example.test");
  const InsecureNavigationsSet set;
};

TEST(MixedContentCheckerTest,
     NotAutoupgradedMixedContentHasUpgradeIfInsecureSet) {
  ResourceRequest request;
  request.SetUrl(KURL("https://example.test"));
  request.SetRequestContext(mojom::RequestContextType::AUDIO);
  TestFetchClientSettingsObject settings;
  // Used to get a non-null document.
  DummyPageHolder holder;

  MixedContentChecker::UpgradeInsecureRequest(
      request, &settings, holder.GetDocument().GetExecutionContext(),
      mojom::RequestContextFrameType::kTopLevel, nullptr);

  EXPECT_FALSE(request.IsAutomaticUpgrade());
  EXPECT_TRUE(request.UpgradeIfInsecure());
}

TEST(MixedContentCheckerTest, AutoupgradedMixedContentHasUpgradeIfInsecureSet) {
  ResourceRequest request;
  request.SetUrl(KURL("http://example.test"));
  request.SetRequestContext(mojom::RequestContextType::AUDIO);
  TestFetchClientSettingsObject settings;
  // Used to get a non-null document.
  DummyPageHolder holder;

  MixedContentChecker::UpgradeInsecureRequest(
      request, &settings, holder.GetDocument().GetExecutionContext(),
      mojom::RequestContextFrameType::kTopLevel, nullptr);

  EXPECT_TRUE(request.IsAutomaticUpgrade());
  EXPECT_TRUE(request.UpgradeIfInsecure());
}

}  // namespace blink
