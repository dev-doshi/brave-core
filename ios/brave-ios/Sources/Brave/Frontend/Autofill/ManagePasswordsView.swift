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
  private typealias GroupID = ManagePasswordsViewModel.GroupID

  @Environment(\.editMode) private var editMode
  @ObservedObject private var saveLogins = Preferences.General.saveLogins
  @State private var isSceneActive = true
  @State private var viewModel: ManagePasswordsViewModel
  @State private var isSearchActive: Bool = false
  @State private var selectedGroupIds: Set<GroupID> = []
  @State private var isDeleteSelectionDialogPresented: Bool = false
  @ScaledMetric var infoIconSize: CGFloat = 24

  private let windowProtection: WindowProtection?

  private var isContentUnavailable: Bool {
    viewModel.allowedGroups.isEmpty && viewModel.blockedGroups.isEmpty && !viewModel.isRefreshing
  }

  init(
    autofillDataManager: CWVAutofillDataManager,
    windowProtection: WindowProtection?
  ) {
    self.windowProtection = windowProtection
    self._viewModel = State(
      initialValue: ManagePasswordsViewModel(autofillDataManager: autofillDataManager)
    )
  }

  var body: some View {
    List(selection: $selectedGroupIds) {
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
        ForEach(viewModel.allowedGroups, id: \.domain) { domain, credentials in
          ManagePasswordListRow(
            domain: domain,
            credentials: credentials,
            isSaved: true
          )
          .tag(GroupID.saved(domain: domain))
          .swipeActions(edge: .trailing, allowsFullSwipe: true) {
            Button(role: .destructive) {
              viewModel.deletePasswords(credentials)
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

      if !viewModel.blockedGroups.isEmpty {
        Section {
          ForEach(viewModel.blockedGroups, id: \.domain) { domain, credentials in
            ManagePasswordListRow(
              domain: domain,
              credentials: credentials,
              isSaved: false
            )
            .tag(GroupID.blocked(domain: domain))
            .swipeActions(edge: .trailing, allowsFullSwipe: true) {
              Button(role: .destructive) {
                viewModel.deletePasswords(credentials)
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
      text: $viewModel.searchText,
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
              selectedGroupIds.isEmpty ? Color(braveSystemName: .textSecondary) : .red
            )
            .disabled(selectedGroupIds.isEmpty)
            .confirmationDialog(
              Strings.Autofill.managePasswordsDeleteCredentialsAlertTitle,
              isPresented: $isDeleteSelectionDialogPresented
            ) {
              Button(Strings.CancelString, role: .cancel) {}
              Button(
                Strings.Autofill.managePasswordsDeleteCredentialButtonTitle,
                role: .destructive
              ) {
                deleteSelectedGroups()
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
            .disabled(viewModel.allowedGroups.isEmpty)
        }
      }
    }
    .onReceive(NotificationCenter.default.publisher(for: UIScene.willDeactivateNotification)) { _ in
      isSceneActive = false
    }
    .onReceive(NotificationCenter.default.publisher(for: UIScene.didActivateNotification)) { _ in
      isSceneActive = true
    }
  }

  private var selectedDomainsString: String {
    selectedGroupIds.map { $0.domain }.sorted().joined(separator: ", ")
  }

  private func deleteSelectedGroups() {
    viewModel.deletePasswords(forGroupIds: selectedGroupIds)
    selectedGroupIds.removeAll()
    editMode?.wrappedValue = .inactive
  }
}

private struct ManagePasswordListRow: View {
  let domain: String
  let credentials: [CWVPassword]
  let isSaved: Bool

  private var resolvedRealmURL: URL {
    credentials.first.flatMap { URL(string: $0.site) } ?? URL(string: "about:blank")!
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
