import 'core-js/actual';
import { listen } from "@ledgerhq/logs";
import MatrixAIApp from '@zondax/ledger-matrix'

// Keep this import if you want to use a Ledger Nano S/X/S Plus with the USB protocol and delete the @ledgerhq/hw-transport-webhid import
import TransportWebUSB from "@ledgerhq/hw-transport-webusb";
// Keep this import if you want to use a Ledger Nano S/X/S Plus with the HID protocol and delete the @ledgerhq/hw-transport-webusb import
import TransportWebHID from "@ledgerhq/hw-transport-webhid";

//Display the header in the div which has the ID "main"
const initial = "<h1>Connect your Nano and open the Bitcoin app. Click anywhere to start...</h1>";
const $main = document.getElementById("main");
$main.innerHTML = initial;

document.body.addEventListener("click", async () => {
  $main.innerHTML = initial;
  try {

    // Select transport type: true = webusb | false = webhid
    const use_webusb = true;
    const transport = use_webusb ? await TransportWebUSB.create() : await TransportWebHID.create();

    //listen to the events which are sent by the Ledger packages in order to debug the app
    listen(log => console.log(log))

    const PATH = "m/44'/318'/0'/0/0"
    const BLOB = Buffer.from(
      'f84d8710000000000000850430e2340083033450a14d414e2e3246745a483939796f533747695564516a64514c553534364e65474739880de0b6b3a76400008001808080808464c3d5f5c4c38080c0',
      'hex',
    )
    const MatrixAI = new MatrixAIApp(transport)

    // Retrieve pubkey and address and log them
    const addressCmd = await MatrixAI.getAddressAndPubKey(PATH)
    console.log(addressCmd.address)
    console.log(addressCmd.publicKey.toString('hex'))

    // Try to sign a transaction and log the response
    const signResponse = (await MatrixAI.sign(PATH, BLOB))
    console.log(`Return code ${signResponse.returnCode}`)
    console.log(`Error message ${signResponse.errorMessage}`)
    console.log(`DER signature ${signResponse.signatureDER.toString('hex')}`)
    console.log(`RSV signature ${signResponse.signatureRSV.toString('hex')}`)

    //Display your bitcoin address on the screen
    const h2 = document.createElement("h2");
    h2.textContent = signResponse.returnCode === 0x9000 ? "Signature OK" : "Sign error or rejected";
    $main.innerHTML = "<h1>Result from sign command:</h1>";
    $main.appendChild(h2);

  } catch (e) {

    //Catch any error thrown and displays it on the screen
    const $err = document.createElement("code");
    $err.style.color = "#f66";
    $err.textContent = String(e.message || e);
    $main.appendChild($err);
  }
});
