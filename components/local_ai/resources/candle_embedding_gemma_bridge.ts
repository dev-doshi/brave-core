// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import {
  LocalAIService,
  OnDeviceModelWorkerReceiver,
  ModelFiles,
} from 'gen/brave/components/local_ai/core/local_ai.mojom.m.js'

// Minimal type for the WASM Gemma3Embedder class
interface Gemma3EmbedderInstance {
  embed(input: string): Float32Array
}

// Initialize connection to the browser-side LocalAIService
const localAIService = LocalAIService.getRemote()

// Implement the OnDeviceModelWorker Mojo interface
class OnDeviceModelWorkerImpl {
  receiver: OnDeviceModelWorkerReceiver
  embedder: Gemma3EmbedderInstance | null
  isInitialized: boolean
  initTime: number

  constructor() {
    this.receiver = new OnDeviceModelWorkerReceiver(this)
    this.embedder = null
    this.isInitialized = false
    this.initTime = 0
  }

  // Implementation of OnDeviceModelWorker::Init
  async init(modelFiles: ModelFiles): Promise<{ success: boolean }> {
    if (this.isInitialized) {
      return { success: true }
    }

    const startTime = performance.now()

    try {
      // Helper function to extract data from BigBuffer
      const extractBigBuffer = (
        buffer: any,
        name: string,
      ): Uint8Array | null => {
        if (buffer.bytes) {
          return new Uint8Array(buffer.bytes)
        } else if (buffer.sharedMemory) {
          const sharedMem = buffer.sharedMemory
          const mapResult = sharedMem.bufferHandle.mapBuffer(0, sharedMem.size)
          if (!mapResult.buffer) {
            console.error(`Failed to map ${name} shared buffer`)
            return null
          }
          return new Uint8Array(mapResult.buffer)
        } else {
          console.error(`Invalid ${name} BigBuffer`)
          return null
        }
      }

      const weightsData = extractBigBuffer(modelFiles.weights, 'Weights')
      const weightsDense1Data = extractBigBuffer(
        modelFiles.weightsDense1,
        'WeightsDense1',
      )
      const weightsDense2Data = extractBigBuffer(
        modelFiles.weightsDense2,
        'WeightsDense2',
      )
      const tokenizerData = extractBigBuffer(modelFiles.tokenizer, 'Tokenizer')
      const configData = extractBigBuffer(modelFiles.config, 'Config')

      if (
        !weightsData
        || !weightsDense1Data
        || !weightsDense2Data
        || !tokenizerData
        || !configData
      ) {
        console.error('Failed to extract model files')
        return { success: false }
      }

      // Dynamically import the WASM Gemma3Embedder class
      const module = await import(
        // @ts-ignore - Served by the same WebUI data source
        'chrome-untrusted://on-device-model-worker/candle_embedding_gemma.bundle.js'
      )
      const { Gemma3Embedder } = module

      this.embedder = new Gemma3Embedder(
        weightsData,
        weightsDense1Data,
        weightsDense2Data,
        tokenizerData,
        configData,
      )
      this.isInitialized = true
      this.initTime = performance.now() - startTime
      console.log(
        `Model loaded successfully in ` + `${this.initTime.toFixed(2)}ms`,
      )
      return { success: true }
    } catch (error) {
      console.error('Failed to load model:', error)
      return { success: false }
    }
  }

  // Implementation of OnDeviceModelWorker::GenerateEmbeddings
  async generateEmbeddings(input: string): Promise<{ output: number[] }> {
    if (!this.isInitialized || !this.embedder) {
      console.error('Model not initialized')
      return { output: [] }
    }

    try {
      const embedding = this.embedder.embed(input)
      // Convert Float32Array to number[] (array<double> in mojo)
      return { output: Array.from(embedding) }
    } catch (error) {
      console.error('Error running embedding:', error)
      return { output: [] }
    }
  }

  getPendingRemote() {
    return this.receiver.$.bindNewPipeAndPassRemote()
  }
}

// Create and register the OnDeviceModelWorker implementation
const modelWorkerImpl = new OnDeviceModelWorkerImpl()
localAIService.registerOnDeviceModelWorker(modelWorkerImpl.getPendingRemote())

console.log('[Candle WASM] On-device model worker bridge initialized')
