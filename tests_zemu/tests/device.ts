import TransportNodeHid from '@ledgerhq/hw-transport-node-hid'
// import TransportWebUSB from "@ledgerhq/hw-transport-webusb";
import MatrixAIApp from '@zondax/ledger-matrix'

const PATH = "m/44'/318'/0'/0/0"
const BLOB = Buffer.from(
  'f84d8710000000000000850430e2340083033450a14d414e2e3246745a483939796f533747695564516a64514c553534364e65474739880de0b6b3a76400008001808080808464c3d5f5c4c38080c0',
  'hex',
)

async function main() {
  const transport = await TransportNodeHid.create()
  const MatrixAI = new MatrixAIApp(transport)
  console.log(await MatrixAI.getVersion())
  console.log(await MatrixAI.getAddressAndPubKey(PATH))
  console.log(await MatrixAI.sign(PATH, BLOB))
}

main()
