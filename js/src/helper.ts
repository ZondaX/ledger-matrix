import { errorCodeToString } from "@zondax/ledger-js";
import { PKLEN } from "./consts";
import { ResponseAddress } from "./types";

export function processGetAddrResponse(response: Buffer): ResponseAddress {
  const errorCodeData = response.subarray(-2);
  const returnCode = errorCodeData[0] * 256 + errorCodeData[1];

  const publicKey = Buffer.from(response.subarray(0, PKLEN));
  response = response.subarray(PKLEN);

  // Address length is variable
  const address = response.subarray(0, -2).toString();

  return {
    publicKey,
    address,
    returnCode,
    errorMessage: errorCodeToString(returnCode),
  };
}
