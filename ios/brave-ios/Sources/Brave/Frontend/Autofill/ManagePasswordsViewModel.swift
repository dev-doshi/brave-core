// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import BraveCore
import Combine
import SwiftUI

@MainActor
@Observable
class ManagePasswordsViewModel {

  /// A thin bridge between `CWVAutofillDataManagerObserver` and the view model. Both delegate
  /// methods funnel into a single `notify` closure, decoupling the observer from the view model
  /// and avoiding the need for a direct reference back to it.
  private class AutofillDataManagerObserver: NSObject, CWVAutofillDataManagerObserver {
    /// Called whenever passwords change. Expected to trigger a fresh password fetch, with results delivered back on the main thread.
    var notify: () -> Void

    init(notify: @escaping () -> Void) {
      self.notify = notify
    }

    /// Protocol conformance: Triggered when any autofill data changes, not limited to passwords (e.g. credit cards, addresses).
    func autofillDataManagerDataDidChange(_ autofillDataManager: CWVAutofillDataManager) {
      notify()
    }

    /// Protocol conformance: Triggered with a breakdown of which passwords were added, updated, or removed.
    /// Both this and `autofillDataManagerDataDidChange` may fire for the same change event,
    /// so `notify` is intentionally idempotent — re-fetching all passwords each time is safe.
    func autofillDataManager(
      _ autofillDataManager: CWVAutofillDataManager,
      didChangePasswordsByAdding added: [CWVPassword],
      updating updated: [CWVPassword],
      removing removed: [CWVPassword]
    ) {
      notify()
    }
  }

  var isRefreshing: Bool = false
  var savedGroups: [(domain: String, credentials: [CWVPassword])] = []
  var blockedGroups: [(domain: String, credentials: [CWVPassword])] = []
  var filteredSavedGroups: [(domain: String, credentials: [CWVPassword])] = []
  var filteredBlockedGroups: [(domain: String, credentials: [CWVPassword])] = []
  var searchText: String = "" {
    didSet { applyFilter() }
  }

  @ObservationIgnored private let autofillDataManager: CWVAutofillDataManager
  @ObservationIgnored private let observer: AutofillDataManagerObserver
  @ObservationIgnored private var cancellables = Set<AnyCancellable>()
  /// Tracks whether a fetch was requested while one was already in flight, ensuring
  /// no change notification is silently dropped.
  @ObservationIgnored private var needsRefetch: Bool = false

  init(autofillDataManager: CWVAutofillDataManager) {
    self.autofillDataManager = autofillDataManager

    let passwordDidChangeSubject = PassthroughSubject<Void, Never>()
    observer = AutofillDataManagerObserver {
      passwordDidChangeSubject.send(())
    }
    autofillDataManager.add(observer)

    passwordDidChangeSubject
      .receive(on: DispatchQueue.main)
      .sink { [weak self] _ in
        self?.fetchPasswords()
      }
      .store(in: &cancellables)

    fetchPasswords()
  }

  deinit {
    autofillDataManager.remove(observer)
  }

  private func updateGroups(allowed: [CWVPassword], blocked: [CWVPassword]) {
    savedGroups = allowed.groupedByDomain()
    blockedGroups = blocked.groupedByDomain()
    applyFilter()
  }

  /// Filters `savedGroups` and `blockedGroups`  using the current `searchText`,
  /// lowercasing the query once and matching against both domain and username.
  /// When the query is empty the filtered results mirror the full groups unchanged.
  private func applyFilter() {
    guard !searchText.isEmpty else {
      filteredSavedGroups = savedGroups
      filteredBlockedGroups = blockedGroups
      return
    }
    let lower = searchText.lowercased()
    let filter: ([(domain: String, credentials: [CWVPassword])]) -> [(domain: String, credentials: [CWVPassword])] = { groups in
      groups.filter { group in
        group.domain.lowercased().contains(lower)
          || group.credentials.contains { ($0.username ?? "").lowercased().contains(lower) }
      }
    }
    filteredSavedGroups = filter(savedGroups)
    filteredBlockedGroups = filter(blockedGroups)
  }

  func fetchPasswords() {
    guard !isRefreshing else {
      needsRefetch = true
      return
    }
    isRefreshing = true
    needsRefetch = false

    autofillDataManager.fetchPasswords { [weak self] passwords in
      guard let self else { return }
      Task { @MainActor in
        self.updateGroups(
          allowed: passwords.filter { !$0.isBlocked },
          blocked: passwords.filter { $0.isBlocked }
        )
        self.isRefreshing = false
        if self.needsRefetch {
          self.fetchPasswords()
        }
      }
    }
  }

  func deletePasswords(_ credentials: [CWVPassword]) {
    guard !credentials.isEmpty else { return }
    for credential in credentials {
      autofillDataManager.delete(credential)
    }
  }

  func deletePasswords(forGroupIds groupIds: Set<GroupID>) {
    // Snapshot the current groups before deletion to avoid operating on
    // potentially stale data if a fetch completes mid-deletion.
    let savedSnapshot = savedGroups
    let blockedSnapshot = blockedGroups
    let toDelete = groupIds.flatMap { groupId -> [CWVPassword] in
      let (groups, domain): ([(domain: String, credentials: [CWVPassword])], String) =
        switch groupId {
        case .saved(let d): (savedSnapshot, d)
        case .blocked(let d): (blockedSnapshot, d)
        }
      return groups.first { $0.domain == domain }?.credentials ?? []
    }
    deletePasswords(toDelete)
  }

  private func credentials(for groupId: GroupID) -> [CWVPassword] {
    let (groups, domain): ([(domain: String, credentials: [CWVPassword])], String) =
      switch groupId {
      case .saved(let d): (savedGroups, d)
      case .blocked(let d): (blockedGroups, d)
      }
    return groups.first { $0.domain == domain }?.credentials ?? []
  }
}

extension ManagePasswordsViewModel {
  /// A lightweight selector indicating which password list to operate on: saved or blocked.
  enum CredentialGroupType: Equatable {
    case saved
    case blocked
  }

  /// A typed identifier for a single domain group as it appears in the UI, encoding both the
  /// domain name and which list the group belongs to. This distinction matters because the same
  /// domain can appear independently in both the saved and blocked lists, so a plain domain string
  /// would be ambiguous as an identifier.
  enum GroupID: Hashable {
    case saved(domain: String)
    case blocked(domain: String)

    /// The domain string regardless of which list this group belongs to. Useful at the call site
    /// when only the display label is needed and list membership is irrelevant.
    var domain: String {
      switch self {
      case .saved(let domain), .blocked(let domain): return domain
      }
    }
  }
}

extension Array where Element == CWVPassword {
  /// Returns an alphabetically sorted list of (domain, credentials) tuples, where each tuple
  /// represents a base domain and all passwords associated with it. For example, credentials for
  /// `accounts.google.com` and `mail.google.com` will both appear under the single key `"google.com"`.
  ///
  /// Passwords whose `site` cannot be parsed into a valid URL, or whose URL yields no base domain,
  /// are silently excluded rather than grouped under a catch-all key. This prevents malformed or
  /// internal entries from surfacing in the UI.
  ///
  /// The sort uses `localizedCaseInsensitiveCompare` so that ordering respects the user's locale
  /// (e.g. accented characters sort naturally) and is case-insensitive. Ordering of credentials
  /// within each group is not guaranteed — callers should apply their own sort if display order
  /// within a domain matters.
  func groupedByDomain() -> [(domain: String, credentials: [CWVPassword])] {
    let grouped = Dictionary(
      grouping: self,
      by: { URL(string: $0.site)?.baseDomain ?? "" }
    )
    return
      grouped
      .filter { !$0.key.isEmpty }
      .map { (domain: $0.key, credentials: $0.value) }
      .sorted { $0.domain.localizedCaseInsensitiveCompare($1.domain) == .orderedAscending }
  }
}
