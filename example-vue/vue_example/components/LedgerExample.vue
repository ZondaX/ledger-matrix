<template>
  <div class="MatrixLedger">
    <input id="webusb" v-model="transportChoice" type="radio" value="WebUSB" />
    <label for="webusb">WebUSB</label>
    <input id="webhid" v-model="transportChoice" type="radio" value="WebHID" />
    <label for="webhid">WebHID</label>
    <br />
    <!--
        Commands
    -->
    <button @click="getVersion">Get Version</button>

    <button @click="appInfo">AppInfo</button>

    <button @click="getAddress">Get Address and Pubkey</button>

    <button @click="showAddress">Show Address and Pubkey</button>

    <button @click="signExampleTx">Sign Example TX</button>
    <!--
        Commands
    -->
    <ul id="ledger-status">
      <li v-for="item in ledgerStatus" :key="item.index">
        {{ item.msg }}
      </li>
    </ul>
  </div>
</template>

<script>
import TransportWebUSB from '@ledgerhq/hw-transport-webusb'
import TransportWebHID from '@ledgerhq/hw-transport-webhid'
import MatrixAI from '@zondax/ledger-matrix'

const path = "m/44'/318'/0'/0/0"

export default {
  name: 'MatrixLedger',
  props: {},
  data() {
    return {
      deviceLog: [],
      transportChoice: 'WebUSB',
    }
  },
  computed: {
    ledgerStatus() {
      return this.deviceLog
    },
  },
  methods: {
    log(msg) {
      this.deviceLog.push({
        index: this.deviceLog.length,
        msg,
      })
    },
    async getTransport() {
      let transport = null

      this.log(`Trying to connect via ${this.transportChoice}...`)
      if (this.transportChoice === 'WebUSB') {
        try {
          transport = await TransportWebUSB.create()
        } catch (e) {
          this.log(e)
        }
      }

      if (this.transportChoice === 'WebHID') {
        try {
          transport = await TransportWebHID.create()
        } catch (e) {
          this.log(e)
        }
      }

      return transport
    },
    async getVersion() {
      this.deviceLog = []

      // Given a transport (WebHID/WebUSB) it is possible instantiate the app
      const transport = await this.getTransport()
      const app = new MatrixAI(transport)

      // now it is possible to access all commands in the app
      const response = await app.getVersion()
      if (response.returnCode !== 0x9000) {
        this.log(`Error [${response.returnCode}] ${response.errorMessage}`)
        return
      }

      this.log('Response received!')
      this.log(`App Version ${response.major}.${response.minor}.${response.patch}`)
      this.log(`Device Locked: ${response.deviceLocked}`)
      this.log(`Test mode: ${response.testMode}`)
      this.log('Full response:')
      this.log(response)
    },
    async appInfo() {
      this.deviceLog = []

      // Given a transport (WebHID/WebUSB) it is possible instantiate the app
      const transport = await this.getTransport()
      const app = new MatrixAI(transport)

      // now it is possible to access all commands in the app
      const response = await app.appInfo()
      if (response.returnCode !== 0x9000) {
        this.log(`Error [${response.returnCode}] ${response.errorMessage}`)
        return
      }

      this.log('Response received!')
      this.log(response)
    },
    async getAddress() {
      this.deviceLog = []

      // Given a transport (WebHID/WebUSB) it is possible instantiate the app
      const transport = await this.getTransport()
      const app = new MatrixAI(transport)

      let response = await app.getVersion()
      this.log(`App Version ${response.major}.${response.minor}.${response.patch}`)
      this.log(`Device Locked: ${response.deviceLocked}`)
      this.log(`Test mode: ${response.testMode}`)

      // now it is possible to access all commands in the app
      response = await app.getAddressAndPubKey(path)
      if (response.returnCode !== 0x9000) {
        this.log(`Error [${response.returnCode}] ${response.errorMessage}`)
        return
      }

      this.log('Response received!')
      this.log('Full response:')
      this.log(response)
    },
    async showAddress() {
      this.deviceLog = []

      // Given a transport (WebHID/WebUSB) it is possible instantiate the app
      const transport = await this.getTransport()
      const app = new MatrixAI(transport)

      let response = await app.getVersion()
      this.log(`App Version ${response.major}.${response.minor}.${response.patch}`)
      this.log(`Device Locked: ${response.deviceLocked}`)
      this.log(`Test mode: ${response.testMode}`)

      // now it is possible to access all commands in the app
      this.log('Please click in the device')
      response = await app.showAddressAndPubKey(path, 'Matrix')
      if (response.returnCode !== 0x9000) {
        this.log(`Error [${response.returnCode}] ${response.errorMessage}`)
        return
      }

      this.log('Response received!')
      this.log('Full response:')
      this.log(response)
    },
    async signExampleTx() {
      this.deviceLog = []

      // Given a transport (WebHID/WebUSB) it is possible instantiate the app
      const transport = await this.getTransport()
      const app = new MatrixAI(transport)

      let response = await app.getVersion()
      this.log(`App Version ${response.major}.${response.minor}.${response.patch}`)
      this.log(`Device Locked: ${response.deviceLocked}`)
      this.log(`Test mode: ${response.testMode}`)

      // now it is possible to access all commands in the app
      const message = Buffer.from(
        'f84d8710000000000000850430e2340083033450a14d414e2e3246745a483939796f533747695564516a64514c553534364e65474739880de0b6b3a76400008001808080808464c3d5f5c4c38080c0',
        'hex',
      )
      response = await app.sign(path, message)

      this.log('Response received!')
      this.log('Full response:')
      this.log(response)
    },
  },
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h3 {
  margin: 40px 0 0;
}

button {
  padding: 5px;
  font-weight: bold;
  font-size: medium;
}

ul {
  padding: 10px;
  text-align: left;
  alignment: left;
  list-style-type: none;
  background: black;
  font-weight: bold;
  color: greenyellow;
}
</style>
