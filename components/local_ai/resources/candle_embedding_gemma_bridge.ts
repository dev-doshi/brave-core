// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import {
  LocalAIService,
  EmbeddingGemmaInterfaceReceiver,
} from 'gen/brave/components/local_ai/core/local_ai.mojom.m.js'

// Initialize connection to the browser-side LocalAIService
const localAIService = LocalAIService.getRemote()

// Implement the EmbeddingGemmaInterface Mojo observer
class EmbeddingGemmaInterfaceImpl {
  receiver: EmbeddingGemmaInterfaceReceiver

  constructor() {
    this.receiver = new EmbeddingGemmaInterfaceReceiver(this)
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

// Create and register the EmbeddingGemmaInterface implementation
const embeddingGemmaImpl = new EmbeddingGemmaInterfaceImpl()
localAIService.bindEmbeddingGemma(embeddingGemmaImpl.getPendingRemote())

console.log('[Candle WASM] Embedding Gemma WASM bridge initialized!')
