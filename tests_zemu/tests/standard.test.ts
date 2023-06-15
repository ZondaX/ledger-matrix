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

import Zemu, { ButtonKind, zondaxMainmenuNavigation } from '@zondax/zemu'
import MatrixAIApp from '@zondax/ledger-matrix'
import { PATH, defaultOptions, models } from './common'

jest.setTimeout(60000)

describe('Standard', function () {
  test.concurrent.each(models)('can start and stop container', async function (m) {
    const sim = new Zemu(m.path)
    try {
      await sim.start({ ...defaultOptions, model: m.name })
    } finally {
      await sim.close()
    }
  })

  test.concurrent.each(models)('main menu', async function (m) {
    const sim = new Zemu(m.path)
    try {
      await sim.start({ ...defaultOptions, model: m.name })
      const nav = zondaxMainmenuNavigation(m.name)
      await sim.navigateAndCompareSnapshots('.', `${m.prefix.toLowerCase()}-mainmenu`, nav.schedule)
    } finally {
      await sim.close()
    }
  })

  test.concurrent.each(models)('get app version', async function (m) {
    const sim = new Zemu(m.path)
    try {
      await sim.start({ ...defaultOptions, model: m.name })
      const app = new MatrixAIApp(sim.getTransport())
      const resp = await app.getVersion()

      console.log(resp)

      expect(resp.returnCode).toEqual(0x9000)
      expect(resp.errorMessage).toEqual('No errors')
      expect(resp).toHaveProperty('testMode')
      expect(resp).toHaveProperty('major')
      expect(resp).toHaveProperty('minor')
      expect(resp).toHaveProperty('patch')
    } finally {
      await sim.close()
    }
  })

  test.concurrent.each(models)('get address', async function (m) {
    const sim = new Zemu(m.path)
    try {
      await sim.start({ ...defaultOptions, model: m.name })
      const app = new MatrixAIApp(sim.getTransport())

      //Define HDPATH
      const resp = await app.getAddressAndPubKey(PATH)

      console.log(resp)

      expect(resp.returnCode).toEqual(0x9000)
      expect(resp.errorMessage).toEqual('No errors')

      const expected_pk =
        '0491c5822f1e8e096d5834c19f53933d9e1d9c653a52c7b7f27e35a202bb4d7d7585f3fdd3d697185b9cd78a5d571281d7d96225042aa4bf26fec7b32d130416e7'
      const expected_address = 'MAN.cUTaQZsmCAdpshzWnFiatff8QZHv'

      expect(resp.publicKey).toEqual(Buffer.from(expected_pk, 'hex'))
      expect(resp.address).toEqual(expected_address)
    } finally {
      await sim.close()
    }
  })

  test.concurrent.each(models)('show address', async function (m) {
    const sim = new Zemu(m.path)
    try {
      await sim.start({
        ...defaultOptions,
        model: m.name,
        approveKeyword: m.name === 'stax' ? 'QR' : '',
        approveAction: ButtonKind.ApproveTapButton,
      })
      const app = new MatrixAIApp(sim.getTransport())

      const respRequest = app.showAddressAndPubKey(PATH)
      // Wait until we are not in the main menu
      await sim.waitUntilScreenIsNot(sim.getMainMenuSnapshot())
      await sim.compareSnapshotsAndApprove('.', `${m.prefix.toLowerCase()}-show_address`)

      const resp = await respRequest
      console.log(resp)

      expect(resp.returnCode).toEqual(0x9000)
      expect(resp.errorMessage).toEqual('No errors')
    } finally {
      await sim.close()
    }
  })

  test.concurrent.each(models)('show address - reject', async function (m) {
    const sim = new Zemu(m.path)
    try {
      await sim.start({ ...defaultOptions, model: m.name, rejectKeyword: m.name === 'stax' ? 'QR' : '' })
      const app = new MatrixAIApp(sim.getTransport())

      const respRequest = app.showAddressAndPubKey(PATH)
      // Wait until we are not in the main menu
      await sim.waitUntilScreenIsNot(sim.getMainMenuSnapshot())
      await sim.compareSnapshotsAndReject('.', `${m.prefix.toLowerCase()}-show_address_reject`)

      const resp = await respRequest
      console.log(resp)

      expect(resp.returnCode).toEqual(0x6986)
      expect(resp.errorMessage).toEqual('Transaction rejected')
    } finally {
      await sim.close()
    }
  })
})
