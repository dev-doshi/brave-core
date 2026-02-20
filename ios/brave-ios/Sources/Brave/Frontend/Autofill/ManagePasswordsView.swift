// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import BraveCore
import BraveStrings
import BraveUI
import Favicon
import Preferences
import SwiftUI
import UIKit

struct ManagePasswordsView: View {
  @Environment(\.editMode) private var editMode
  @ObservedObject private var saveLogins = Preferences.General.saveLogins
  @State private var isSceneActive = true
  @State private var viewModel: ManagePasswordsViewModel
  @State private var searchText: String = ""
  @State private var isSearchActive: Bool = false
  @State private var selectedDomainIds: Set<String> = []
  @State private var isDeleteSelectionDialogPresented: Bool = false
  @State private var isDeletePasswordDialogPresented: Bool = false
  @ScaledMetric var infoIconSize: CGFloat = 24

  private let passwordAPI: BravePasswordAPI
  private let windowProtection: WindowProtection?

  private static func domainId(saved: Bool, domain: String) -> String {
    saved ? "saved:\(domain)" : "blocked:\(domain)"
  }

  private var isContentUnavailable: Bool {
    viewModel.credentialList.isEmpty && viewModel.blockedList.isEmpty && !viewModel.isRefreshing
      && !isSearchActive
  }

  init(
    passwordAPI: BravePasswordAPI,
    windowProtection: WindowProtection?
  ) {
    self.passwordAPI = passwordAPI
    self.windowProtection = windowProtection
    self._viewModel = State(initialValue: ManagePasswordsViewModel(passwordAPI: passwordAPI))
  }

  var body: some View {
    List(selection: $selectedDomainIds) {
      if !isSearchActive {
        Section {
          Text(Strings.Autofill.managePasswordsInstructions)
            .foregroundStyle(Color(braveSystemName: .textTertiary))
            .font(.footnote)
            .listRowBackground(Color.clear)
            .listRowSeparator(.hidden)
        }
      }

      if !isSearchActive {
        Toggle(Strings.Autofill.managePasswordsOfferToSavePasswords, isOn: $saveLogins.value)
          .tint(Color(braveSystemName: .primary40))
      }

      Section {
        ForEach(viewModel.groupedCredentialList, id: \.domain) { domain, credentials in
          let id = ManagePasswordsView.domainId(saved: true, domain: domain)
          ManagePasswordListRow(
            domain: domain,
            credentials: credentials,
            isSaved: true,
            passwordAPI: passwordAPI,
          )
          .swipeActions(edge: .trailing, allowsFullSwipe: true) {
            Button(role: .destructive) {
              isDeletePasswordDialogPresented = true
            } label: {
              Label(
                Strings.Autofill.managePasswordsDeleteCredentialButtonTitle,
                braveSystemImage: "leo.trash"
              )
              .labelStyle(.iconOnly)
            }
          }
        }
      } header: {
        Text(Strings.Autofill.managePasswordsListHeaderTitle)
          .font(.subheadline)
      }

      if !viewModel.blockedList.isEmpty {
        Section {
          ForEach(Array(viewModel.groupedBlockedList), id: \.domain) { domain, credentials in
            let id = ManagePasswordsView.domainId(saved: false, domain: domain)
            ManagePasswordListRow(
              domain: domain,
              credentials: credentials,
              isSaved: false,
              passwordAPI: passwordAPI,
            )
            .swipeActions(edge: .trailing, allowsFullSwipe: true) {
              Button(role: .destructive) {
                isDeletePasswordDialogPresented = true
              } label: {
                Label(
                  Strings.Autofill.managePasswordsDeleteCredentialButtonTitle,
                  braveSystemImage: "leo.trash"
                )
                .labelStyle(.iconOnly)
              }
            }
          }
        } header: {
          Text(Strings.Autofill.managePasswordsNeverSavedListHeaderTitle)
            .font(.subheadline)
        }
      }
    }
    .overlay {
      if isContentUnavailable {
        VStack {
          Text(Strings.Autofill.managePasswordsInstructions)
            .foregroundStyle(Color(braveSystemName: .textTertiary))
            .font(.footnote)
            .padding(.horizontal, 32)
            .padding(.top, 16)

          Toggle(Strings.Autofill.managePasswordsOfferToSavePasswords, isOn: $saveLogins.value)
            .tint(Color(braveSystemName: .primary40))
            .padding(.horizontal, 32)
            .padding(.top, 24)

          ContentUnavailableView {
            Label {
              Text(Strings.Autofill.managePasswordsEmptyListTitle)
                .foregroundStyle(Color(braveSystemName: .textPrimary))
                .font(.title3)
            } icon: {
              Image(braveSystemName: "leo.info.ios-only")
                .resizable()
                .frame(width: infoIconSize, height: infoIconSize)
                .foregroundStyle(Color(braveSystemName: .iconDefault))
            }
          } description: {
            Text(Strings.Autofill.managePasswordsEmptyListDetail)
              .foregroundStyle(Color(braveSystemName: .textPrimary))
              .font(.callout)
              .padding(16)
          }
        }
        .background(Color(.braveGroupedBackground))
      }
    }
    .background(Color(.braveGroupedBackground))
    .searchable(
      text: $searchText,
      isPresented: $isSearchActive,
      placement: .navigationBarDrawer(displayMode: .always),
      prompt: Strings.Autofill.managePasswordsListSearchWebsitesPrompt
    )
    .overlay {
      if !isSceneActive {
        Color(.braveGroupedBackground)
          .ignoresSafeArea()
      }
    }
    .toolbarBackground(.visible, for: .navigationBar)
    .navigationTitle(Strings.Autofill.managePasswordsTitle)
    .navigationBarTitleDisplayMode(.inline)
    .toolbar {
      ToolbarItemGroup(placement: .bottomBar) {
        if !isContentUnavailable {
          if editMode?.wrappedValue == .active {
            Button(Strings.Autofill.managePasswordsDeleteCredentialButtonTitle) {
              isDeleteSelectionDialogPresented = true
            }
            .foregroundStyle(
              selectedDomainIds.isEmpty ? Color(braveSystemName: .textSecondary) : .red
            )
            .disabled(selectedDomainIds.isEmpty)
            .confirmationDialog(
              Strings.Autofill.managePasswordsDeleteCredentialsAlertTitle,
              isPresented: $isDeleteSelectionDialogPresented
            ) {
              Button(
                Strings.CancelString,
                role: .cancel
              ) {}
              Button(
                Strings.Autofill.managePasswordsDeleteCredentialButtonTitle,
                role: .destructive
              ) {
                deleteSelectedDomains()
              }
            } message: {
              Text(
                String.localizedStringWithFormat(
                  Strings.Autofill.managePasswordsDeleteCredentialConfirmMessage,
                  selectedDomainsString
                )
              )
            }
            .foregroundStyle(Color(braveSystemName: .textInteractive))
          }
          Spacer()
          EditButton()
            .disabled(viewModel.groupedCredentialList.isEmpty)
        }
      }
    }
    .onAppear {
      if !searchText.isEmpty {
        viewModel.performSearch(query: searchText.lowercased())
      } else {
        viewModel.fetchCredentials()
      }
    }
    .onChange(of: searchText) {
      viewModel.performSearch(query: searchText.lowercased())
    }
    .onReceive(NotificationCenter.default.publisher(for: UIScene.willDeactivateNotification)) { _ in
      isSceneActive = false
    }
    .onReceive(NotificationCenter.default.publisher(for: UIScene.didActivateNotification)) { _ in
      isSceneActive = true
    }
  }

  private var selectedDomainsString: String {
    selectedDomainIds.reduce(into: "") { result, domainId in
      let savedDomainPrefix = "saved:"
      let blockedDomainPrefix = "blocked:"
      let domain: String

      if domainId.hasPrefix(savedDomainPrefix) {
        domain = String(domainId.dropFirst(savedDomainPrefix.count))
      } else if domainId.hasPrefix(blockedDomainPrefix) {
        domain = String(domainId.dropFirst(blockedDomainPrefix.count))
      } else {
        domain = domainId
      }
      if !result.isEmpty { result += ", " }
      result += domain
    }
  }

  private func deleteSelectedDomains() {
    var credentialsToRemove: [PasswordForm] = []
    for id in selectedDomainIds {
      if id.hasPrefix("saved:") {
        let domain = String(id.dropFirst(6))
        if let group = viewModel.groupedCredentialList.first(where: { $0.domain == domain }) {
          credentialsToRemove.append(contentsOf: group.credentials)
        }
      } else if id.hasPrefix("blocked:") {
        let domain = String(id.dropFirst(8))
        if let group = viewModel.groupedBlockedList.first(where: { $0.domain == domain }) {
          credentialsToRemove.append(contentsOf: group.credentials)
        }
      }
    }
    viewModel.removeCredentials(credentialsToRemove)
    selectedDomainIds.removeAll()
    editMode?.wrappedValue = .inactive
  }

  private func deleteDomain(_ domainId: String) {
    var credentialsToRemove: [PasswordForm] = []
    if domainId.hasPrefix("saved:") {
      let domain = String(domainId.dropFirst(6))
      if let group = viewModel.groupedCredentialList.first(where: { $0.domain == domain }) {
        credentialsToRemove.append(contentsOf: group.credentials)
      }
    } else if domainId.hasPrefix("blocked:") {
      let domain = String(domainId.dropFirst(8))
      if let group = viewModel.groupedBlockedList.first(where: { $0.domain == domain }) {
        credentialsToRemove.append(contentsOf: group.credentials)
      }
    }
    viewModel.removeCredentials(credentialsToRemove)
    selectedDomainIds.remove(domainId)
  }
}

private struct ManagePasswordListRow: View {
  let domain: String
  let credentials: [PasswordForm]
  let isSaved: Bool
  let passwordAPI: BravePasswordAPI

  private var resolvedRealmURL: URL {
    credentials.first.flatMap { URL(string: $0.signOnRealm) } ?? URL(string: "about:blank")!
  }

  private var resolvedDomain: String {
    domain.isEmpty ? Strings.Autofill.managePasswordsUnknownDomainText : domain
  }

  @ViewBuilder
  var body: some View {
    NavigationLink {
      //TODO: Navigation Link to Detail or Group List
    } label: {
      Label {
        VStack(alignment: .leading, spacing: 2) {
          Text(resolvedDomain)
          if credentials.count > 1 {
            Text("\(credentials.count) \(Strings.Autofill.managePasswordMultipleAccounts)")
              .font(.footnote)
              .foregroundStyle(Color(braveSystemName: .textSecondary))
          }
        }
      } icon: {
        FaviconImage(url: resolvedRealmURL, isPrivateBrowsing: false)
          .frame(width: 24, height: 24)
          .clipShape(RoundedRectangle(cornerRadius: 6, style: .continuous))
      }
    }
  }
}
