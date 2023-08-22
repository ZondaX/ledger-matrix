/** ******************************************************************************
 *  (c) 2018 - 2023 Zondax AG
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************* */

import Zemu from '@zondax/zemu'
import MatrixAIApp from '@zondax/ledger-matrix'
import { MATRIX_TRANSACTIONS, PATH, defaultOptions, models } from './common'
import { ecdsaVerify, signatureImport } from 'secp256k1'
import keccak256 from 'keccak256'

jest.setTimeout(60000)

describe.each(MATRIX_TRANSACTIONS)('Transactions', function (data) {
  test.concurrent.each(models)(`Test: ${data.name}`, async function (m) {
    const sim = new Zemu(m.path)
    try {
      await sim.start({ ...defaultOptions, model: m.name })
      const app = new MatrixAIApp(sim.getTransport())

      const responseAddr = await app.getAddressAndPubKey(PATH)

      // do not wait here.. we need to navigate
      const signatureRequest = app.sign(PATH, data.blob)

      // Wait until we are not in the main menu
      await sim.waitUntilScreenIsNot(sim.getMainMenuSnapshot())
      await sim.compareSnapshotsAndApprove('.', `${m.prefix.toLowerCase()}-${data.name}`)

      const signatureResponse = await signatureRequest
      console.log(signatureResponse)

      expect(signatureResponse.returnCode).toEqual(0x9000)
      expect(signatureResponse.errorMessage).toEqual('No errors')

      // Now verify the signature
      const msgHash = keccak256(data.blob)

      const emptyBuffer = Buffer.from([])
      const pubKey = responseAddr.publicKey ?? emptyBuffer

      const signatureDER = signatureImport(signatureResponse.signatureDER ?? emptyBuffer)
      const signatureRS = signatureResponse.signatureRSV?.subarray(0, -1)
      const signatureOk = ecdsaVerify(signatureRS ?? emptyBuffer, msgHash, pubKey) && ecdsaVerify(signatureDER, msgHash, pubKey)

      expect(signatureOk).toEqual(true)
    } finally {
      await sim.close()
    }
  })
})

describe('Custom', function () {

  test.concurrent.each(models)('Transaction error', async function (m) {
    const sim = new Zemu(m.path)
    try {
      await sim.start({ ...defaultOptions, model: m.name })
      const app = new MatrixAIApp(sim.getTransport())

      const PATH = "m/44'/318'/0'/0/0";
      const BLOB = Buffer.from("f84d8710000000000000850430e2340083033450a14d414e2e3246745a483939796f533747695564516a64514c553534364e65474739880de0b6b3a76400008001808080808464c3d5f5c4c38080c0", 'hex');

      const responseAddr = await app.getAddressAndPubKey(PATH)

      // do not wait here.. we need to navigate
      const signatureRequest = app.sign(PATH, BLOB)

      // Wait until we are not in the main menu
      await sim.waitUntilScreenIsNot(sim.getMainMenuSnapshot())
      await sim.compareSnapshotsAndApprove('.', `${m.prefix.toLowerCase()}-custom`)

      const signatureResponse = await signatureRequest
      console.log(signatureResponse)

      expect(signatureResponse.returnCode).toEqual(0x9000)
      expect(signatureResponse.errorMessage).toEqual('No errors')

      // Now verify the signature
      const msgHash = keccak256(BLOB)

      const emptyBuffer = Buffer.from([])
      const pubKey = responseAddr.publicKey ?? emptyBuffer

      const signatureDER = signatureImport(signatureResponse.signatureDER ?? emptyBuffer)
      const signatureRS = signatureResponse.signatureRSV?.subarray(0, -1)
      const signatureOk = ecdsaVerify(signatureRS ?? emptyBuffer, msgHash, pubKey) && ecdsaVerify(signatureDER, msgHash, pubKey)

      expect(signatureOk).toEqual(true)


    } finally {
      await sim.close()
    }
  })
})
