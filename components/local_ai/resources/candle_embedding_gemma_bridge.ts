// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import {
  LocalAIService,
  OnDeviceModelWorkerReceiver,
} from 'gen/brave/components/local_ai/core/local_ai.mojom.m.js'

// Initialize connection to the browser-side LocalAIService
const localAIService = LocalAIService.getRemote()

// Implement the OnDeviceModelWorker Mojo interface
class OnDeviceModelWorkerImpl {
  receiver: OnDeviceModelWorkerReceiver

  constructor() {
    this.receiver = new OnDeviceModelWorkerReceiver(this)
  }

  // Implementation of OnDeviceModelWorker::GenerateEmbeddings
  async generateEmbeddings(input: string): Promise<{ output: number[] }> {
    // TODO(https://github.com/brave/brave-browser/issues/52722):
    // stub until model loading is wired up
    return { output: [] }
  }

  getPendingRemote() {
    return this.receiver.$.bindNewPipeAndPassRemote()
  }
}

// Create and register the OnDeviceModelWorker implementation
const modelWorkerImpl = new OnDeviceModelWorkerImpl()
localAIService.registerOnDeviceModelWorker(modelWorkerImpl.getPendingRemote())

console.log('[Candle WASM] On-device model worker bridge initialized')
