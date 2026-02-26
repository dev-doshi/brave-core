// Copyright 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

import BraveCore
import BraveUI
import Preferences
import SwiftUI
import UIKit

class ManagePasswordsViewController: UIHostingController<ManagePasswordsView> {

  private let autofillDataManager: CWVAutofillDataManager
  private let windowProtection: WindowProtection?

  init(autofillDataManager: CWVAutofillDataManager, windowProtection: WindowProtection?) {
    self.autofillDataManager = autofillDataManager
    self.windowProtection = windowProtection

    let managePasswordsView = ManagePasswordsView(
      autofillDataManager: autofillDataManager,
      windowProtection: windowProtection
    )

    super.init(rootView: managePasswordsView)
  }

  @available(*, unavailable)
  required init(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }

  override func viewWillAppear(_ animated: Bool) {
    super.viewWillAppear(animated)
    // UINavigationController hides the bottom toolbar by default
    navigationController?.setToolbarHidden(false, animated: animated)
  }
}
